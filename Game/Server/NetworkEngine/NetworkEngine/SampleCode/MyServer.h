#pragma once

#include "../NetworkComponent/NetworkHeader.h"
#include "MyPacketHeader.h"
#include "MyPacket.h"

class CMyServer final
{
public:
	CMyServer();
	~CMyServer();

	static CMyServer* GetSingleton();

public:
	static unique_ptr<class CNetworkComponent> Server;

	static unordered_map<SOCKET, CPlayerPacket> Players;
	static CRITICAL_SECTION csPlayers;

public:
	static bool Initialize(const char* const IPv4, const USHORT& Port);
	static bool IsNetworkOn();
	static void Close();

	static void ConnectCBF(CCompletionKey CompletionKey);
	static void DisconnectCBF(CCompletionKey CompletionKey);

	static void Data(stringstream& RecvStream, const SOCKET& Socket);
	static void BigData(stringstream& RecvStream, const SOCKET& Socket);
};