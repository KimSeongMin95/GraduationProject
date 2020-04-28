
// [B477004 김성민 (KimSeongMin)]

#pragma once

#include "pch.h"

#include <stdio.h>
#include <tchar.h>

#include "NetworkComponent.h"
#include "ExampleClass.h"

int main()
{
	// 테스트
	CExampleClass ex;
	while (true)
	{
		ex.Send();
		ex.SendHuge();

		Sleep(1);
		for (int i = 0; i < TEST_MAX_CLIENT; i++)
		{
			ex.Clients[i]->Close();
			ex.Clients[i]->Initialize("127.0.0.1", 8000);
		}
	}

	return 0;
}