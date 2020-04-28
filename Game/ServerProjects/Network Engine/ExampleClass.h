#pragma once

#include "Packet.h"

class CExampleClass
{
public:
	/////////////////////
	// ±âº»
	/////////////////////
	CExampleClass();
	~CExampleClass();

public:
	////////////////////////
	// NetworkComponent
	////////////////////////
	class CNetworkComponent* Server;
	class CNetworkComponent* Clients[TEST_MAX_CLIENT];

public:
	////////////////////////
	// NetworkComponent
	////////////////////////
	static void ConnectCBF(CCompletionKey CompletionKey);
	static void DisconnectCBF(CCompletionKey CompletionKey);

	static void SampleServer(CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket);
	static void SampleClient(CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket = 0);
	void Send();

	static void SampleHugeServer(CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket);
	static void SampleHugeClient(CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket = 0);
	void SendHuge();
};