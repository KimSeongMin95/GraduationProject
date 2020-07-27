﻿#pragma once

#include "NetworkHeader.h"
#include "NetworkConfig.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#if BUILD_CONFIG_DEBUG && BUILD_CONFIG_EDITOR
#define MY_LOG(CategoryName, Verbosity, Format, ...) UE_LOG(CategoryName, Verbosity, Format, ##__VA_ARGS__)
#else
#define MY_LOG(CategoryName, Verbosity, Format, ...) 
#endif

class CConsole final
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