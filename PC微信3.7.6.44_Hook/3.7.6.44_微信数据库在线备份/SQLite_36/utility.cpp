#include "pch.h"
#include <codecvt>
#include "utility.h"


string WStringToString(const wstring& input)
{
	wstring_convert<codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(input);
}

wstring StringToWString(const string& input)
{
	wstring_convert<codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(input);
}

unsigned long HexStringUInt(const std::string& input)
{
	return strtoul(input.c_str(), NULL, 16);
}

