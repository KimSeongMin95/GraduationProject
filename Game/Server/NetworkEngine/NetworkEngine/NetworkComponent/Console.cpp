
#include "Console.h"

CConsole::CConsole()
{
	fp_console = nullptr;
}

CConsole* CConsole::GetSingleton()
{
	static CConsole console;
	return &console;
}

void CConsole::AllocConsole()
{
	if (fp_console) // �̹� �Ҵ�Ǿ� ������ �ܼ��� �� �Ҵ����� �ʽ��ϴ�.
		return;

	if (::AllocConsole())
	{
		freopen_s(&fp_console, "CONOUT$", "w", stdout);
	}
}
void CConsole::_AllocConsole() {}

void CConsole::FreeConsole()
{
	if (!fp_console) // �Ҵ�Ǿ� ���� ���� �Ҹ��ŵ�ϴ�.
	{
		return;
	}

	fclose(fp_console);
	fp_console = nullptr;

	::FreeConsole();
}
void CConsole::_FreeConsole() {}

void CConsole::Log(const char* format, ...)
{
	char buff[MAX_BUFFER * 2];

	va_list arglist;
	va_start(arglist, format);
	vsprintf_s(buff, format, arglist);
	va_end(arglist);

	printf_s(buff);

}
void CConsole::_Log(const char* format, ...) {}

void CConsole::ErrorMessageDisplay(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
}
void CConsole::_ErrorMessageDisplay(const char* msg) {}