#pragma once

#include "Console.h"

template <typename T>
class CExceptionHandler final
{
public:
	CExceptionHandler() = delete;

public:
	static T* MustDynamicAlloc();
	static T* MustDynamicAlloc(int Num);
};

template <typename T>
T* CExceptionHandler<T>::MustDynamicAlloc()
{
	while (true)
	{
		try
		{
			T* Pointer = new T;
			return Pointer;
		}
		catch (bad_alloc e)
		{
			CONSOLE_LOG("[Error] <CExceptionHandler::DynamicAlloc()> catch (bad_alloc e): %s \n", e.what());
			Sleep(1);
		}
	}
}

template <typename T>
T* CExceptionHandler<T>::MustDynamicAlloc(int Num)
{
	while (true)
	{
		try
		{
			T* Pointer = new T[Num];
			return Pointer;
		}
		catch (bad_alloc e)
		{
			CONSOLE_LOG("[Error] <CExceptionHandler::DynamicAlloc(...)> catch (bad_alloc e): %s \n", e.what());
			Sleep(1);
		}
	}
}
