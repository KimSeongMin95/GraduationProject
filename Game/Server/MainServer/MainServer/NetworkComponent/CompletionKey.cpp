
#include "Console.h"

#include "CompletionKey.h"

CCompletionKey::CCompletionKey()
{
	Socket = 0;
	IPv4Addr = "0.0.0.0";
	Port = 0;
}

void CCompletionKey::PrintInfo(const char* c_str)
{
	CONSOLE_LOG("%s socket: %d, IPv4Addr: %s, Port: %d \n", c_str, (int)Socket, IPv4Addr.c_str(), Port);
}