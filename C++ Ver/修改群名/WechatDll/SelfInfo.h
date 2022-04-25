#pragma once
#include<windows.h>
#include<iostream>
using namespace std;
extern "C" __declspec(dllexport) DWORD GetSelfInfoRemote();
extern "C" __declspec(dllexport) VOID DeleteSelfInfoCacheRemote();