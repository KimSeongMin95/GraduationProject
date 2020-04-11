#pragma once

#include "Packet.h"

class test
{
public:
	test();
	~test();

public:
	class CNetworkComponent* Server;
	class CNetworkComponent* Client;

public:
	static void SampleServer(CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket);
	static void SampleClient(CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket = 0);
	void Send();

	static void SampleHugeServer(CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket);
	static void SampleHugeClient(CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket = 0);
	void SendHuge();
};