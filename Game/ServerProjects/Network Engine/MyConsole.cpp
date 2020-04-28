#include "pch.h"

#include "MyConsole.h"


void CMyConsole::AllocConsole()
{
#if TEMP_BUILD_CONFIG_DEBUG

	// �̹� �Ҵ�Ǿ� ������ �ܼ��� �� �Ҵ����� �ʽ��ϴ�.
	if (fp)
		return;

	if (::AllocConsole())
	{
		freopen_s(&fp, "CONOUT$", "w", stdout);
	}
#endif
}

void CMyConsole::FreeConsole()
{
#if TEMP_BUILD_CONFIG_DEBUG
	// �̹� �Ҵ�Ǿ� ���� ���� �Ҹ��ŵ�ϴ�.
	if (fp)
	{
		fclose(fp);
		fp = nullptr;

		::FreeConsole();
	}
#endif
}

CMyConsole* CMyConsole::GetSingleton()
{
	static CMyConsole console;
	return &console;
}

void CMyConsole::Log(const char* format, ...)
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