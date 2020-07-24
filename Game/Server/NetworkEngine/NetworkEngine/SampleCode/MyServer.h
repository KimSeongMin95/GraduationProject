#pragma once

#include "../NetworkComponent/NetworkHeader.h"
#include "MyPacketHeader.h"
#include "MyPacket.h"

class CMyServer sealed
{
public:
	CMyServer();
	~CMyServer();

public:
	static class CNetworkComponent* Server;

	static map<SOCKET, CInfoOfClient> InfoOfClients;
	static CRITICAL_SECTION csInfoOfClients;

public:
	static void ConnectCBF(CCompletionKey CompletionKey);
	static void DisconnectCBF(CCompletionKey CompletionKey);
	static void Login(stringstream& RecvStream, const SOCKET& Socket);
	static void Move(stringstream& RecvStream, const SOCKET& Socket);

	static CMyServer* GetSingleton();
};