#include <Windows.h>
#include <string>
#include <list>
#pragma once

//定义一个结构体来存储 数据库句柄-->数据库名
struct DB_HANDLE_NAME
{
	int handle;
	char name[MAX_PATH];
};

//在内存中存储一个“数据库句柄-->数据库名”的链表，
extern std::list<DB_HANDLE_NAME> wx_db_list;
extern const std::string wxVersoin;
extern DWORD hookAddress;
extern DWORD jumpBackAddress;

BOOL CheckVersion();
VOID WechatReboot();
VOID WechatHook();
VOID WechatUnhook();
VOID WechatHookJump();
VOID WechatSQLiteHandle(int dbAddress, int dbHandle);
