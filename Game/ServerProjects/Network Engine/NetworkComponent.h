#pragma once

#include "Packet.h"



class CNetworkComponent
{
public:
	/////////////////////
	// �⺻
	/////////////////////
	CNetworkComponent(ENetworkComponentType NCT = ENetworkComponentType::NCT_None);
	~CNetworkComponent();


private:
	ENetworkComponentType NCT;

	class CServer* Server = nullptr;
	class CClient* Client = nullptr;


public:
	/////////////////////
	// ���� & Ŭ�� ����
	/////////////////////
	bool Initialize(const char* IPv4, USHORT Port);
	void Close();
	void Send(stringstream& SendStream, SOCKET Socket = 0); // 0�̸� Ŭ���̾�Ʈ
	void SendHugePacket(stringstream& SendStream, SOCKET Socket = 0); // 0�̸� Ŭ���̾�Ʈ
	bool IsNetworkOn();


	/////////////////////
	// only ����
	/////////////////////
	CCompletionKey GetCompletionKey(SOCKET Socket);
	void Broadcast(stringstream& SendStream);
	void BroadcastExceptOne(stringstream& SendStream, SOCKET Except);


	/////////////////////
	// only Ŭ��
	/////////////////////

};