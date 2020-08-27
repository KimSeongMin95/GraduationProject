#pragma once

#include "NetworkHeader.h"
#include "NetworkConfig.h"

class CConsole final
{
private:
	CConsole();

public:
	static CConsole* GetSingleton();

private:
	FILE* fp_console;

public:
	// 콘솔창을 활성화합니다.
	void AllocConsole();
	void _AllocConsole();

	// 콘솔창을 비활성화합니다. (제거하지는 않습니다.)
	void FreeConsole();
	void _FreeConsole();
	
	// printf_s(...)의 래퍼 함수입니다.
	static void Log(const char* format, ...);
	static void _Log(const char* format, ...);

	// 소켓 함수 오류를 메세지 박스로 출력합니다.
	static void ErrorMessageDisplay(const char* msg);
	static void _ErrorMessageDisplay(const char* msg);
};

#if ACTIVATE_CONSOLE
#define CONSOLE_ALLOC() CConsole::GetSingleton()->AllocConsole()
#define CONSOLE_FREE() CConsole::GetSingleton()->FreeConsole()
#else
#define CONSOLE_ALLOC() CConsole::GetSingleton()->_AllocConsole()
#define CONSOLE_FREE() CConsole::GetSingleton()->_FreeConsole()
#endif

#if BUILD_CONFIG_DEBUG && BUILD_CONFIG_GAME
#define CONSOLE_LOG CConsole::Log
#define CONSOLE_MSG CConsole::ErrorMessageDisplay
#else
#define CONSOLE_LOG CConsole::_Log
#define CONSOLE_MSG CConsole::_ErrorMessageDisplay
#endif

#if BUILD_CONFIG_DEBUG && BUILD_CONFIG_EDITOR
#define MY_LOG(CategoryName, Verbosity, Format, ...) UE_LOG(CategoryName, Verbosity, Format, ##__VA_ARGS__)
#else
#define MY_LOG(CategoryName, Verbosity, Format, ...) 
#endif

#if BUILD_CONFIG_NETWORK
#define CONSOLE_LOG_NETWORK CConsole::Log
#define CONSOLE_MSG_NETWORK CConsole::ErrorMessageDisplay
#else
#define CONSOLE_LOG_NETWORK CConsole::_Log
#define CONSOLE_MSG_NETWORK CConsole::_ErrorMessageDisplay
#endif

#if BUILD_CONFIG_TEST
#define CONSOLE_LOG_TEST CConsole::Log
#define CONSOLE_MSG_TEST CConsole::ErrorMessageDisplay
#else
#define CONSOLE_LOG_TEST CConsole::_Log
#define CONSOLE_MSG_TEST CConsole::_ErrorMessageDisplay
#endif



//void CheckBuildConriguration()
//{
//	if (!fp_console)
//		return;
//
//	printf_s("\n\n\n\n\n");
//
//	char config[16];
//	switch (FApp::GetBuildConfiguration())
//	{
//	case 0:
//		strcpy_s(config, "Unknown");
//		break;
//	case 1:
//		strcpy_s(config, "Debug");
//		break;
//	case 2:
//		strcpy_s(config, "DebugGame");
//		break;
//	case 3:
//		strcpy_s(config, "Development");
//		break;
//	case 4:
//		strcpy_s(config, "Shipping");
//		break;
//	case 5:
//		strcpy_s(config, "Test");
//		break;
//	}
//
//	printf_s("GetBuildConfiguration: %s \n", config);
//	printf_s("UE_BUILD_DEBUG: %d \n", UE_BUILD_DEBUG);
//	printf_s("UE_BUILD_DEVELOPMENT: %d \n", UE_BUILD_DEVELOPMENT);
//	printf_s("UE_BUILD_SHIPPING: %d \n", UE_BUILD_SHIPPING);
//	printf_s("UE_GAME: %d \n", UE_GAME);
//	printf_s("UE_EDITOR: %d \n", UE_EDITOR);
//	printf_s("WITH_EDITOR: %d \n", WITH_EDITOR);
//	printf_s("UE_BUILD_SHIPPING_WITH_EDITOR: %d \n", UE_BUILD_SHIPPING_WITH_EDITOR);
//	printf_s("UE_SERVER: %d \n", UE_SERVER);
//	printf_s("UE_BUILD_TEST: %d \n", UE_BUILD_TEST);
//	printf_s("\n\n\n\n\n");
//}