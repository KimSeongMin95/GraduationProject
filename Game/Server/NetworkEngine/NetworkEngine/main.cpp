#pragma once

#include "NetworkComponent/NetworkComponent.h"

#include "SampleCode/MyServer.h"
#include "SampleCode/MyClient.h"

int main()
{
	unsigned int choice = -1;
	cout << "\n [Choice] 1: Server, 2: Client \n" << endl;
	cin >> choice;

	switch (choice)
	{
	case 1:
	{
		// 서버
		CMyServer* server = CMyServer::GetSingleton();
		//while (server->Server->Initialize("127.0.0.1", 8000) == false);
		while (server->Initialize(nullptr, 8000) == false);

		while (true) Sleep(3600000); // 메인 스레드가 바쁜대기를 하도록 합니다.
		break;
	}
	case 2:
	{
		// 클라이언트
		CMyClient* client = CMyClient::GetSingleton();
		while (client->Initialize("127.0.0.1", 8000) == false);

		while (true)
		{
			Sleep(2000);

			unsigned int choose = -1;

			cout << "\n [Choose] 1: SendData(), 2: SendBigData(), 3: Exit \n" << endl;

			cin >> choose;

			switch (choose)
			{
			case 1:
			{
				client->SendData();
				break;
			}
			case 2:
			{
				client->SendBigData();
				break;
			}
			case 3:
			{
				return -1;
			}
			default:
				break;
			}
		}
		break;
	}
	default:
		cout << "선택지를 제대로 고르지 않아서 종료합니다." << endl;
		break;
	}

	return 0;
}
