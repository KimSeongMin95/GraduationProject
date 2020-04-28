#include "pch.h"

#include "MyConsole.h"


void CMyConsole::AllocConsole()
{
#if TEMP_BUILD_CONFIG_DEBUG

	// 이미 할당되어 있으면 콘솔을 더 할당하지 않습니다.
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
	// 이미 할당되어 있을 때만 소멸시킵니다.
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