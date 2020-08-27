#pragma once

#include "../NetworkComponent/NetworkHeader.h"
#include "MyPacketHeader.h"
#include "MyPacket.h"
#include "../NetworkComponent/ThreadSafetyQueue.h"

class CMyClient final
{
private:
	CMyClient();
	~CMyClient();

public:
	static CMyClient* GetSingleton();

public:
	static unique_ptr<class CNetworkComponent> Client;

	static CThreadSafetyQueue<CPlayerPacket> tsqPlayerPacket;

public:
	static bool Initialize(const char* const IPv4, const USHORT& Port);
	static bool IsNetworkOn();
	static void Close();

	static void ConnectCBF(CCompletionKey CompletionKey);
	static void DisconnectCBF(CCompletionKey CompletionKey);

	static void SendData();
	static void RecvData(stringstream& RecvStream, const SOCKET& Socket);
	static void SendBigData();
	static void RecvBigData(stringstream& RecvStream, const SOCKET& Socket);
};