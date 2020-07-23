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
	// �ܼ�â
	void AllocConsole();
	void FreeConsole();

	static CConsole* GetSingleton();

	// printf_s(...)�� ���� �Լ�
	static void Log(const char* format, ...);

	// ���� �Լ� ���� ��� �� ����
	static void ErrorMessageQuit(const char* msg);

	// ���� �Լ� ���� ���
	static void ErrorMessageDisplay(const char* msg);
};
#define CONSOLE_LOG CConsole::Log