
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
		while (true);
	}

	//while (true)
	//{
	//	if (server->Initialize())
	//	{
	//		Sleep(10000);
	//		server->CloseServer();
	//	}
	//}

	return 0;
}