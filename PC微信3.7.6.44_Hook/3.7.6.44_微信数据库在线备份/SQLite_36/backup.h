#pragma once
#include "sqlite3.h"
#include <string>
#include <Windows.h>

//º¯ÊýÖ¸Õë
typedef int(__cdecl* p_sqlite3_exec)(sqlite3*, const char*, int (*callback)(void*, int, char**, char**), void*, char**);
typedef int(__cdecl* p_sqlite3_open)(const char*, sqlite3**);
typedef sqlite3_backup* (__cdecl* p_sqlite3_backup_init)(sqlite3*, const char*, sqlite3*, const char*);
typedef int(__cdecl* p_sqlite3_backup_step)(sqlite3_backup*, int);
typedef int(__cdecl* p_sqlite3_backup_finish)(sqlite3_backup*);
typedef int(__cdecl* p_sqlite3_backup_remaining)(sqlite3_backup*);
typedef int(__cdecl* p_sqlite3_backup_pagecount)(sqlite3_backup*);
typedef int(__cdecl* p_sqlite3_sleep)(int);
typedef int(__cdecl* p_sqlite3_errcode)(sqlite3*);
typedef int(__cdecl* p_sqlite3_close)(sqlite3*);

extern std::string backupFileName2;

int BackupSQLite(sqlite3* pDb, const char* zFilename, void(*xProgress)(int, int));
VOID XProgress(int pos, int all);
VOID RunBackupSQLite();
VOID BackupSQLiteDB();
VOID RunSQLByHandle(std::string, const char*, sqlite3_callback, void*, char**);
