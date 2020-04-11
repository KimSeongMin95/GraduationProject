#pragma once

#include <Windows.h>
#include <iostream>
#include <stdarg.h>

#define TEMP_BUILD_CONFIG_DEBUG 1

class MyConsole
{
private:
	FILE* fp_console = nullptr;

public:
	void AllocConsole();

	void FreeConsole();

	static MyConsole* GetSingleton();

	/** 4096 ũ�� ��ŭ�� ���� ����� ����� �� �ֽ��ϴ�.
	����: ũ�Ⱑ �ʰ��ϸ� ������ �߻��մϴ�. */
	static void Log(const char* format, ...);
};
#define CONSOLE_LOG MyConsole::Log