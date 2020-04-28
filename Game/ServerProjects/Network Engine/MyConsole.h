#pragma once

#include <Windows.h>
#include <iostream>
#include <stdarg.h>

#define TEMP_BUILD_CONFIG_DEBUG 1

class CMyConsole
{
private:
	FILE* fp = nullptr;

public:
	void AllocConsole();

	void FreeConsole();

	static CMyConsole* GetSingleton();

	/** 4096 크기 만큼의 버퍼 사이즈를 출력할 수 있습니다.
	주의: 크기가 초과하면 에러가 발생합니다. */
	static void Log(const char* format, ...);
};
#define CONSOLE_LOG CMyConsole::Log