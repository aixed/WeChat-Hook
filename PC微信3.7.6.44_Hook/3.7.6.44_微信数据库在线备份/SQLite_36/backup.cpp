#include "pch.h"
#include <Windows.h>
#include <string>
#include "sqlite3.h"
#include "backup.h"
#include "resource.h"
#include "wechat.h"
#include "patch.h"
#include "utility.h"
#include "backup_ui.h"

HWND hWinDlg = 0;
DWORD selectedDbHander = 0;
string backupFileName2;
//3.7.6.44
DWORD baseAddress = 0x0;
DWORD offset_sqlite3_exec = 0x150D130;
DWORD offset_sqlite3_open = 0x1541A80;
DWORD offset_sqlite3_backup_init = 0x14D2AD0;
DWORD offset_sqlite3_backup_step = 0x14D2ED0;
DWORD offset_sqlite3_sleep = 0x15422C0;
DWORD offset_sqlite3_backup_finish = 0x14D3510;
DWORD offset_sqlite3_close = 0x153EEA0;
DWORD offset_sqlite3_backup_remaining = 0x14D3610;
DWORD offset_sqlite3_backup_pagecount = 0x14D3620;
DWORD offset_sqlite3_errcode = 0x1540720;
DWORD offset_sqlite3_backup_init_patch = offset_sqlite3_backup_init + 0x58;
DWORD offset_wechat_login = 0x2571FB8;

int BackupSQLite(sqlite3* pDb, const char* zFilename, void(*xProgress)(int, int))
{
	int rc;                     /* Function return code */
	sqlite3* pFile;             /* Database connection opened on zFilename */
	sqlite3_backup* pBackup;    /* Backup handle used to copy data */

	auto f_sqlite3_open = (p_sqlite3_open)(baseAddress + offset_sqlite3_open);
	auto f_sqlite3_backup_init = (p_sqlite3_backup_init)(baseAddress + offset_sqlite3_backup_init);
	auto f_sqlite3_backup_step = (p_sqlite3_backup_step)(baseAddress + offset_sqlite3_backup_step);
	auto f_sqlite3_backup_remaining = (p_sqlite3_backup_remaining)(baseAddress + offset_sqlite3_backup_remaining);
	auto f_sqlite3_backup_pagecount = (p_sqlite3_backup_pagecount)(baseAddress + offset_sqlite3_backup_pagecount);
	auto f_sqlite3_sleep = (p_sqlite3_sleep)(baseAddress + offset_sqlite3_sleep);
	auto f_sqlite3_backup_finish = (p_sqlite3_backup_finish)(baseAddress + offset_sqlite3_backup_finish);
	auto f_sqlite3_errcode = (p_sqlite3_errcode)(baseAddress + offset_sqlite3_errcode);
	auto f_sqlite3_close = (p_sqlite3_close)(baseAddress + offset_sqlite3_close);

	rc = f_sqlite3_open(zFilename, &pFile);
	if (rc == SQLITE_OK) {

		pBackup = f_sqlite3_backup_init(pFile, "main", pDb, "main");
		if (pBackup) {

			do {
				rc = f_sqlite3_backup_step(pBackup, 5);
				xProgress(
					f_sqlite3_backup_remaining(pBackup),
					f_sqlite3_backup_pagecount(pBackup)
				);
				if (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED) {
					f_sqlite3_sleep(1);
				}
			} while (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED);

			(void)f_sqlite3_backup_finish(pBackup);
		}
		rc = f_sqlite3_errcode(pFile);
	}
	(void)f_sqlite3_close(pFile);
	return rc;
}

VOID XProgress(int pos, int all)
{
	UpdateProgressBar(pos, all);
}

VOID RunBackupSQLite()
{
	DWORD selectedDbHander = 0;
	const char* sql = NULL;

	//清除文本框内容
	HWND edit = GetDlgItem(hWinDlg, IDC_EDIT_LOG);
	SendMessageA(edit, WM_SETTEXT, NULL, NULL);

	//获取数据库句柄
	HWND combo1 = GetDlgItem(hWinDlg, IDC_COMBO_DB);
	int index = SendMessageA(combo1, CB_GETCURSEL, NULL, 0);
	char buf[MAX_PATH] = { 0 };
	SendMessageA(combo1, CB_GETLBTEXT, index, (LPARAM)buf);

	//添加日志
	string text = "在线备份的数据库：\r\n";
	text.append(buf);
	text.append("\r\n");
	AddLogUI(text);

	//获取查询的数据库句柄
	string dbName(buf);
	char hexString[12] = { 0 };
	for (auto& db : wx_db_list)
	{
		string dbNameInList(db.name);
		if (dbNameInList == dbName)
		{
			selectedDbHander = db.handle;
			break;
		}
	}

	//SQL语句,限制为MAX_PATH个字符，可做适当调整
	BackupButtonClick();
}

VOID BackupSQLiteDB()
{
	const char* backupFile = backupFileName2.c_str();

	PatchSqlite3BackupInit();

	BackupSQLite((sqlite3*)selectedDbHander, backupFile, XProgress);

	string text = "备份完成！\r\n";
	AddLogUI(text);

	//询问是否查看文件
	int result = MessageBoxA(NULL, "备份完成，是否查看？", "完成", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1 | MB_APPLMODAL | MB_TOPMOST);
	if (result == IDYES)
	{
		//获取默认文件名
		TCHAR driver[_MAX_DRIVE] = { 0 };
		TCHAR dir[_MAX_DIR] = { 0 };
		TCHAR fname[_MAX_FNAME] = { 0 };
		TCHAR ext[_MAX_EXT] = { 0 };
		TCHAR szFile[MAX_PATH] = { 0 };
		_wsplitpath_s(StringToWString(backupFileName2).c_str(), driver, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
		wstring filePath(driver);
		filePath.append(dir);

		ShellExecute(NULL, L"explore", filePath.c_str(), NULL, NULL, SW_SHOW);
	}
}

VOID RunSQLByHandle(string handle, const char* zSql, sqlite3_callback xCallback, void* pArg, char** pzErrMsg)
{
	auto f_sqlite3_exec = (p_sqlite3_exec)(baseAddress + offset_sqlite3_exec);
	auto h = HexStringUInt(handle);

	f_sqlite3_exec((sqlite3*)h, zSql, xCallback, pArg, pzErrMsg);
}

