#pragma once

#include <stdio.h>
#include <tchar.h>

#include "NetworkComponent/NetworkComponent.h"

#include "SampleCode/MyServer.h"
#include "SampleCode/MyClient.h"

#include <iostream>

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
		while (true) Sleep(3600000);
		break;
	}
	case 2:
	{
		// 클라
		CMyClient* client = CMyClient::GetSingleton();
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

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
