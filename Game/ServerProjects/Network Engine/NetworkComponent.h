#pragma once

#include "Packet.h"



class CNetworkComponent
{
public:
	/////////////////////
	// 기본
	/////////////////////
	CNetworkComponent(ENetworkComponentType NCT = ENetworkComponentType::NCT_None);
	~CNetworkComponent();


private:
	ENetworkComponentType NCT;

	class CServer* Server = nullptr;
	class CClient* Client = nullptr;


public:
	/////////////////////
	// 서버 & 클라 공통
	/////////////////////
	bool Initialize(const char* IPv4, USHORT Port);
	void Close();
	void Send(stringstream& SendStream, SOCKET Socket = 0); // 0이면 클라이언트
	void SendHugePacket(stringstream& SendStream, SOCKET Socket = 0); // 0이면 클라이언트
	bool IsNetworkOn();


	/////////////////////
	// only 서버
	/////////////////////
	CCompletionKey GetCompletionKey(SOCKET Socket);
	void Broadcast(stringstream& SendStream);
	void BroadcastExceptOne(stringstream& SendStream, SOCKET Except);


	/////////////////////
	// only 클라
	/////////////////////

};