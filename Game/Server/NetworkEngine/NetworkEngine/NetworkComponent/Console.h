#pragma once

#include "NetworkHeaders.h"
#include "NetworkConfig.h"

class CConsole sealed
{
public:
	CConsole();

private:
	FILE* fp_console;

public:
	// 콘솔창
	void AllocConsole();
	void FreeConsole();

	static CConsole* GetSingleton();

	// printf_s(...)의 래퍼 함수
	static void Log(const char* format, ...);

	// 소켓 함수 오류 출력 후 종료
	static void ErrorMessageQuit(const char* msg);

	// 소켓 함수 오류 출력
	static void ErrorMessageDisplay(const char* msg);
};
#define CONSOLE_LOG CConsole::Log