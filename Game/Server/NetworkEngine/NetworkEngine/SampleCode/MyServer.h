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

	static map<SOCKET, > ;
	static CRITICAL_SECTION ;

public:
	static bool Initialize(const char* const IPv4, const USHORT& Port);
	static bool IsNetworkOn();
	static void Close();

	static void ConnectCBF(CCompletionKey CompletionKey);
	static void DisconnectCBF(CCompletionKey CompletionKey);

	static void Login(stringstream& RecvStream, const SOCKET& Socket);
	static void Data(stringstream& RecvStream, const SOCKET& Socket);
	static void Huge(stringstream& RecvStream, const SOCKET& Socket);
};