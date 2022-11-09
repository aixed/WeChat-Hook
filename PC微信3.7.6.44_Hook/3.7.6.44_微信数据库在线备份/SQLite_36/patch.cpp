#include "pch.h"
#include "offsets.h"

void PatchSqlite3BackupInit()
{
	BYTE JmpCode[1] = { 0 };
	JmpCode[0] = 0x84;
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)(baseAddress + offset_sqlite3_backup_init_patch), JmpCode, 1, 0);
}
