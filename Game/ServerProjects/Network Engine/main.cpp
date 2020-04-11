
// [B477004 김성민 (KimSeongMin)]

#pragma once

#include "pch.h"

#include <stdio.h>
#include <tchar.h>

#include "NetworkComponent.h"
#include "test.h"

int main()
{
	test t;

	CPacket::RegisterTypeAndStaticFunc("Sample", ENetworkComponentType::NCT_Server, t.SampleServer);
	CPacket::RegisterTypeAndStaticFunc("Sample", ENetworkComponentType::NCT_Client, t.SampleClient);
	CPacket::RegisterTypeAndStaticFunc("SampleHuge", ENetworkComponentType::NCT_Server, t.SampleHugeServer);
	CPacket::RegisterTypeAndStaticFunc("SampleHuge", ENetworkComponentType::NCT_Client, t.SampleHugeClient);

	printf_s("%d \n", CPacket::GetNumberOfType("Sample"));
	printf_s("%d \n", CPacket::GetNumberOfType("SampleHuge"));

	t.Server->Initialize("127.0.0.1", 8000);


	while (true)
	{
		Sleep(1000);
		t.Client->Initialize("127.0.0.1", 8000);

		t.Send();
		t.SendHuge();

		Sleep(5000);
		t.Client->Close();
	}

	return 0;
}