#include "SundaySearch.h"

static size_t SundaySearch(const BYTE* target, int tLen, const BYTE* pattern, int pLen, std::vector<size_t>& offsets)
{
    const size_t SHIFT_SIZE = 0x100;
    BYTE shift[SHIFT_SIZE] = { 0 };
    memset(shift, pLen + 1, SHIFT_SIZE);
    for (int i = 0; i < pLen; i++)
        shift[pattern[i]] = pLen - i;
    for (int i = 0; i < tLen - pLen; i += shift[target[i + pLen]])
    {
        for (int j = 0; j < pLen; j++)
        {
            if (target[i + j] != pattern[j]) break;
            if (j == pLen - 1)
                offsets.push_back(i);
        }
    }
    return offsets.size();
}

size_t ScanPattern(HANDLE hProcess, BYTE* pattern, int pLen, std::vector<LPVOID>& results)
{
    const size_t MEM_SIZE = 0x1000;
    BYTE mem[MEM_SIZE] = { 0 };
    MEMORY_BASIC_INFORMATION mbi = { 0 };
#ifndef _WIN64
    for (size_t curAddress = 0x10000; curAddress < 0xFFFEFFFF; curAddress += mbi.RegionSize)
#else
    for (size_t curAddress = 0x10000; curAddress < 0x00007FFFFFFEFFFFULL; curAddress += mbi.RegionSize)
#endif
    {
        VirtualQueryEx(hProcess, (void*)curAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
#ifndef _WIN64
        if ((int)mbi.RegionSize <= 0) break;
#else
        if ((long long)mbi.RegionSize <= 0) break;
#endif
        if (mbi.State != MEM_COMMIT) continue;
        for (size_t memIndex = 0; memIndex < (size_t)mbi.RegionSize / MEM_SIZE; memIndex++)
        {
            size_t beginAddress = curAddress + memIndex * MEM_SIZE;
            size_t numberOfRead = 0;
            BOOL bRead = ReadProcessMemory(hProcess, (void*)(beginAddress), mem, MEM_SIZE, &numberOfRead);
            std::vector<size_t> offsets;
            size_t count = SundaySearch(mem, MEM_SIZE, pattern, pLen, offsets);
            for(auto &offset : offsets)
                results.push_back((LPVOID)(beginAddress + offset));
        }
    }
    return results.size();
}
