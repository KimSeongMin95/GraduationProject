#include "pch.h"

#include "MyConsole.h"


void MyConsole::AllocConsole()
{
#if TEMP_BUILD_CONFIG_DEBUG

	// �̹� �Ҵ�Ǿ� ������ �ܼ��� �� �Ҵ����� �ʽ��ϴ�.
	if (fp_console)
		return;

	if (::AllocConsole())
	{
		freopen_s(&fp_console, "CONOUT$", "w", stdout);
	}
#endif
}

void MyConsole::FreeConsole()
{
#if TEMP_BUILD_CONFIG_DEBUG
	// �̹� �Ҵ�Ǿ� ���� ���� �Ҹ��ŵ�ϴ�.
	if (fp_console)
	{
		fclose(fp_console);
		fp_console = nullptr;

		::FreeConsole();
	}
#endif
}

MyConsole* MyConsole::GetSingleton()
{
	static MyConsole console;
	return &console;
}

void MyConsole::Log(const char* format, ...)
{
#if TEMP_BUILD_CONFIG_DEBUG
	char buff[8192];

	va_list arglist;
	va_start(arglist, format);
	vsprintf_s(buff, format, arglist);
	va_end(arglist);

	printf_s(buff);
#endif
}