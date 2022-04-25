// dllmain.cpp : 定义 DLL 应用程序的入口点。


#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include "crypto/pbkdf2_hmac.h"
#include "crypto/sha.h"
#include "crypto/aes.c"
#include "dllmain.h"


#pragma comment(lib,"shlwapi.lib")



#undef _UNICODE
#define SQLITE_FILE_HEADER "SQLite format 3" 
#define IV_SIZE 16
#define HMAC_SHA1_SIZE 20
#define KEY_SIZE 32

#define SL3SIGNLEN 20

#ifndef ANDROID_WECHAT
//4048数据 + 16IV + 20 HMAC + 12
#define DEFAULT_PAGESIZE 4096      
#define DEFAULT_ITER 64000
#else
#define NO_USE_HMAC_SHA1
#define DEFAULT_PAGESIZE 1024
#define DEFAULT_ITER 4000
#endif


//for WeChat Version 2.8.0.121
//密钥指针相对于WeChatWin.dll基地址偏移
#define KEY_OFFSET  0x161cc50
#define WXID_OFFSET 0x161cc78

char dbfilename[32] = "Emotion.db";




DWORD FindProcessId(const char* processname)
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	DWORD result = NULL;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hProcessSnap) return(FALSE);

	pe32.dwSize = sizeof(PROCESSENTRY32); // <----- IMPORTANT

										  // Retrieve information about the first process,
										  // and exit if unsuccessful
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);          // clean the snapshot object
		printf("!!! Failed to gather information on system processes! \n");
		return(NULL);
	}

	do
	{
		//printf("Checking process %ls\n", pe32.szExeFile);
		if (0 == strcmp(processname, pe32.szExeFile))
		{
			result = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return result;
}

DWORD FindProcess(const char* szProcName) {
	DWORD pid;

	while (TRUE) {
		pid = FindProcessId(szProcName);
		if (pid)
		{
			return pid;
		}
		Sleep(500);
	}

}

HMODULE GetModule(DWORD processID, CONST CHAR* szModuleName)
{
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;

	// Get a handle to the process.
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);
	if (NULL == hProcess)
		return NULL;

	// Get a list of all the modules in this process.
	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];

			// Get the full path to the module's file.
			if (GetModuleFileNameEx(hProcess, hMods[i], szModName,
				sizeof(szModName) / sizeof(TCHAR)))
			{
				// Print the module name and handle value.	
				PathStripPath(szModName);
				if (!_tcscmp(szModName, szModuleName)) {
					//_tprintf(TEXT("\t%s (0x%08X)\n"), szModName, hMods[i]);
					CloseHandle(hProcess);
					return hMods[i];
				}
			}
		}
	}

	// Release the handle to the process.
	CloseHandle(hProcess);

	return 0;
}

BOOL GetDatabaseKey(DWORD pid, LPVOID* buf) {
	DWORD pKey = 0;

	HMODULE pWeChatdll = GetModule(pid, "WeChatWin.dll");
	if (!pWeChatdll)
	{
		printf("GetModule error!\n");
		return 0;

	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	//ReadProcessMemory(hProcess, 0x0299F1C8, &pKey, 4, NULL);
	ReadProcessMemory(hProcess, (LPCVOID)0x0299F1C8, buf, 32, 0);
	CloseHandle(hProcess);

	//if (!pKey) {
	//	return 0;
	//}
	return 1;
}



//打开db文件
size_t OpenDBFile(char* szFilePath, BYTE** pDbBuffer) {

	DWORD RSize;
	HANDLE hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		//printf("open datebase file error! %d \n", GetLastError());
		return 0;
	}

	DWORD dwFileSize = GetFileSize(hFile, NULL);
	*pDbBuffer = (BYTE*)malloc(sizeof(BYTE) * dwFileSize);


	ReadFile(hFile, *pDbBuffer, dwFileSize, &RSize, NULL);
	if (dwFileSize != RSize)
	{
		//printf("read file error!\n");
		OutputDebugStringA("读取数据库文件失败");
		return 0;
	}
	CloseHandle(hFile);
	return dwFileSize;

};

BOOL Decryptdb(char* szFilePath, char* dbfilename, BYTE* pass)
{
	OutputDebugStringA("开始执行函数");

	BYTE* pDbBuffer = NULL;
	size_t nFileSize = OpenDBFile(szFilePath, &pDbBuffer);
	if (!nFileSize) {
		OutputDebugStringA("打开数据库文件失败");
		return 0;
	}

	//数据库文件前16字节为盐值
	BYTE salt[16] = { 0 };
	memcpy(salt, pDbBuffer, 16);

#ifndef NO_USE_HMAC_SHA1
	//HMAC验证用的盐值需要异或0x3a
	BYTE mac_salt[16] = { 0 };
	memcpy(mac_salt, salt, 16);
	for (int i = 0; i < sizeof(salt); i++)
		mac_salt[i] ^= 0x3a;
#endif

	//保留段长度,PC端每4KB有48B
	int reserve = IV_SIZE;
#ifndef NO_USE_HMAC_SHA1
	reserve += HMAC_SHA1_SIZE;
#endif
	reserve = ((reserve % AES_BLOCK_SIZE) == 0) ? reserve : ((reserve / AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;

	//密钥扩展，分别对应AES解密密钥和HMAC验证密钥
	BYTE key[KEY_SIZE] = { 0 };
	BYTE mac_key[KEY_SIZE] = { 0 };
	PKCS5_PBKDF2_HMAC((const BYTE*)pass, 32,
		salt, sizeof(salt), DEFAULT_ITER, sizeof(key), key);
#ifndef NO_USE_HMAC_SHA1
	PKCS5_PBKDF2_HMAC((const BYTE*)key, sizeof(key),
		mac_salt, sizeof(mac_salt), 2, sizeof(mac_key), mac_key);
#endif

	BYTE* pTemp = pDbBuffer;
	BYTE pDecryptPerPageBuffer[DEFAULT_PAGESIZE];
	int nPage = 1;
	int offset = 16;
	OutputDebugStringA("开始解密");

	while (pTemp < pDbBuffer + nFileSize)
	{
		//printf("解密数据页:%d/%d \n", nPage, nFileSize / DEFAULT_PAGESIZE);


		//char Info[256] = {};
		//snprintf(Info, 256, "%s %s", nPage, nFileSize / DEFAULT_PAGESIZE);

		//OutputDebugStringA(Info);


#ifndef NO_USE_HMAC_SHA1
		BYTE hash_mac[HMAC_SHA1_SIZE] = { 0 };
		sha1_context hctx;
		SHA_Init(&hctx.ctx);
		sha1_hmac_starts(&hctx, mac_key, sizeof(mac_key));
		sha1_hmac_update(&hctx, pTemp + offset, DEFAULT_PAGESIZE - reserve - offset + IV_SIZE);//4096-48-16+16
		sha1_hmac_update(&hctx, (const BYTE*)&nPage, sizeof(nPage));
		sha1_hmac_finish(&hctx, hash_mac);
		BYTE* pHMAC = pTemp + DEFAULT_PAGESIZE - reserve + IV_SIZE;
		if (0 != memcmp(hash_mac, pHMAC, sizeof(hash_mac)))
		{

			OutputDebugStringA("哈希值错误");
			getchar();
			return 0;
		}
#endif

		if (nPage == 1)
			memcpy(pDecryptPerPageBuffer, SQLITE_FILE_HEADER, offset);

		//AES解密操作
		BYTE key_schedule[40];
		aes_key_setup(key, (DWORD*)key_schedule, 256);
		aes_decrypt_cbc(
			pTemp + offset, DEFAULT_PAGESIZE - reserve - offset,
			pDecryptPerPageBuffer + offset,
			(const DWORD*)key_schedule,
			256,
			pTemp + (DEFAULT_PAGESIZE - reserve)
		);


		memcpy(pDecryptPerPageBuffer + DEFAULT_PAGESIZE - reserve,
			pTemp + DEFAULT_PAGESIZE - reserve,
			reserve);

		//将解密后的数据写入到文件中
		char decFile[1024] = { 0 };
		sprintf_s(decFile, 1024, "%s", dbfilename);

		FILE* fp;
		fopen_s(&fp, decFile, "ab+");
		fwrite(pDecryptPerPageBuffer, 1, DEFAULT_PAGESIZE, fp);
		fclose(fp);

		nPage++;
		offset = 0;
		pTemp += DEFAULT_PAGESIZE;
	}

	OutputDebugStringA("解密成功");
	printf("\n dec_%s文件可用navicat之类的软件读取！ \n", dbfilename);

	getchar();
	return TRUE;
}



BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}



