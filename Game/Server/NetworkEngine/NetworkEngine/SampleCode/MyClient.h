#pragma once

#include "../NetworkComponent/NetworkHeaders.h"
#include "MyPacketHeader.h"
#include "MyPackets.h"
#include "../NetworkComponent/ThreadSafetyQueue.h"

class CMyClient sealed
{
public:
	CMyClient();
	~CMyClient();

public:
	static class CNetworkComponent* Client;

	static CInfoOfClient MyInfoOfClient;
	static CRITICAL_SECTION csMyInfoOfClient;

	static CThreadSafetyQueue<bool> AcceptTSQ;
	static CThreadSafetyQueue<bool> RejectTSQ;
	static CThreadSafetyQueue<CInfoOfClient> CreateTSQ;
	static CThreadSafetyQueue<CInfoOfClient> MoveTSQ;
	static CThreadSafetyQueue<CInfoOfClient> ExitTSQ;

public:
	static void SendLogin();
	static void RecvAccept(stringstream& RecvStream, const SOCKET& Socket);
	static void RecvReject(stringstream& RecvStream, const SOCKET& Socket);
	static void RecvCreate(stringstream& RecvStream, const SOCKET& Socket);
	static void SendMove();
	static void RecvMove(stringstream& RecvStream, const SOCKET& Socket);
	static void RecvExit(stringstream& RecvStream, const SOCKET& Socket);

	static CMyClient* GetSingleton();
	void SetID(const char* c_str);
	void SetRandomPos();
};