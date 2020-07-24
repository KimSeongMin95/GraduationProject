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

		while (true) Sleep(3600000);
		break;
	}
	case 2:
	{
		// 클라
		CMyClient* client = CMyClient::GetSingleton();
		while (client->Initialize("127.0.0.1", 8000) == false);

		while (true)
		{
			unsigned int choose = -1;

			cout << "\n [Choose] 0: SetID(), 1: SendLogin(), 2: SendMove(), 3: Exit \n" << endl;

			cin >> choose;

			switch (choose)
			{
			case 0:
			{
				char id[256];
				cout << "[Input ID (띄어쓰기가 없어야 합니다.)] ";
				cin >> id;
				client->SetID(id);
				break;
			}
			case 1:
			{
				client->SendLogin();
				break;
			}
			case 2:
			{
				client->SetRandomPos();
				client->SendMove();
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
		cout << "제대로 고르지 않아서 종료합니다." << endl;
		break;
	}

	return 0;
}
