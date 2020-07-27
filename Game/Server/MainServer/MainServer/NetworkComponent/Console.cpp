
#include "Console.h"

CConsole::CConsole()
{
	fp_console = nullptr;
}

void CConsole::AllocConsole()
{
#if BUILD_CONFIG_DEBUG
	if (fp_console) // 이미 할당되어 있으면 콘솔을 더 할당하지 않습니다.
		return;

	if (::AllocConsole())
	{
		freopen_s(&fp_console, "CONOUT$", "w", stdout);
	}
#endif
}

void CConsole::FreeConsole()
{
#if BUILD_CONFIG_DEBUG
	if (fp_console) // 할당되어 있을 때만 소멸시킵니다.
	{
		fclose(fp_console);
		fp_console = nullptr;

		::FreeConsole();
	}
#endif
}

CConsole* CConsole::GetSingleton()
{
	static CConsole console;
	return &console;
}

void CConsole::Log(const char* format, ...)
{
#if BUILD_CONFIG_DEBUG
	char buff[MAX_BUFFER];

	va_list arglist;
	va_start(arglist, format);
	vsprintf_s(buff, format, arglist);
	va_end(arglist);

	printf_s(buff);
#endif
}

void CConsole::ErrorMessageQuit(const char* msg)
{
#if BUILD_CONFIG_DEBUG
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
#endif
}

void CConsole::ErrorMessageDisplay(const char* msg)
{
#if BUILD_CONFIG_DEBUG
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	CONSOLE_LOG("%s %s\n", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
#endif
}