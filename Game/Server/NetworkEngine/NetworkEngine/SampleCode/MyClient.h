#pragma once

#include "../NetworkComponent/NetworkHeader.h"
#include "MyPacketHeader.h"
#include "MyPacket.h"
#include "../NetworkComponent/ThreadSafetyQueue.h"

class CMyClient final
{
public:
	CMyClient();
	~CMyClient();

	static CMyClient* GetSingleton();

public:
	static unique_ptr<class CNetworkComponent> Client;

	static CInfoOfClient MyInfoOfClient;
	static CRITICAL_SECTION csMyInfoOfClient;

	static CThreadSafetyQueue<bool> AcceptTSQ;
	static CThreadSafetyQueue<bool> RejectTSQ;
	static CThreadSafetyQueue<CInfoOfClient> CreateTSQ;
	static CThreadSafetyQueue<CInfoOfClient> MoveTSQ;
	static CThreadSafetyQueue<CInfoOfClient> ExitTSQ;

public:
	static bool Initialize(const char* const IPv4, const USHORT& Port);
	static bool IsNetworkOn();
	static void Close();

	static void ConnectCBF(CCompletionKey CompletionKey);
	static void DisconnectCBF(CCompletionKey CompletionKey);

	static void SendLogin();
	static void RecvAccept(stringstream& RecvStream, const SOCKET& Socket);
	static void RecvReject(stringstream& RecvStream, const SOCKET& Socket);
	static void RecvCreate(stringstream& RecvStream, const SOCKET& Socket);
	static void SendMove();
	static void RecvMove(stringstream& RecvStream, const SOCKET& Socket);
	static void RecvExit(stringstream& RecvStream, const SOCKET& Socket);

	void SetID(const char* c_str);
	void SetRandomPos();
};