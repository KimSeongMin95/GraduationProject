
#include <stdio.h>
#include <tchar.h>

#include "MainServer.h"

int main()
{
	MainServer iocp_server;

	if (iocp_server.Initialize())
		iocp_server.StartServer();

	return 0;
}