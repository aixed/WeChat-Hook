#pragma once
#include <winsock2.h>
#include <vector>

size_t ScanPattern(HANDLE hProcess, BYTE* pattern, int pLen, std::vector<LPVOID>& results);