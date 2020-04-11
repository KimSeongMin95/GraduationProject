
#include <stdio.h>
#include <tchar.h>

#include "MainServer.h"

int main()
{
	MainServer* server = MainServer::GetSingleton();

	if (!server)
		return -1;

	if (server->Initialize())
	{
		while (true)
		{
			// 서버 초기화 실패를 감지합니다.
			if (server->IsServerOn() == false)
				break;
		}
	}

	return 0;
}