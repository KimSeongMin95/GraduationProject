#pragma once

#include "Packet.h"


// Ŭ���̾�Ʈ ���� / ����� ������ �ݹ��Լ��� ���� �����մϴ�.
typedef void* (*NC_CallBackFunc)(CCompletionKey);

class CNetworkComponent : public CPacket
{
public:
	/////////////////////
	// �⺻
	/////////////////////
	/** ���� or Ŭ���̾�Ʈ�� �����ϰ� 
	��Ŷ Ŭ������ �⺻�� ����ϰų� ���� ��Ŷ �������̽� Ŭ������ ��ӹ޾� ������ Ŭ������ ����մϴ�. */
	CNetworkComponent(
		ENetworkComponentType NCT = ENetworkComponentType::NCT_None, 
		class CPacket* PacketClass = nullptr);
	~CNetworkComponent();


private:
	ENetworkComponentType NCT;

	class CPacket* Packet = nullptr;
	class CServer* Server = nullptr;
	class CClient* Client = nullptr;


	/////////////////////
	// only ����
	/////////////////////
	NC_CallBackFunc Connect = nullptr; // Ŭ���̾�Ʈ�� ������ �� ������ �ݹ��Լ��Դϴ�.
	NC_CallBackFunc Disconnect = nullptr; // Ŭ���̾�Ʈ�� ������ �� ������ �ݹ��Լ��Դϴ�.


public:
	/////////////////////
	// ��Ŷ �������̽� ���
	/////////////////////
	virtual void RegisterTypeAndStaticFunc(string Name, void (*Function)(class CNetworkComponent*, stringstream&, SOCKET)) final;
	virtual unsigned int GetNumberOfType(string Name) final;
	virtual void ProcessPacket(unsigned int Type, class CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket) final;


	/////////////////////
	// ���� & Ŭ�� ����
	/////////////////////
	bool Initialize(const char* IPv4, USHORT Port);
	void Close();
	void Send(stringstream& SendStream, SOCKET Socket = 0); // 0�̸� Ŭ���̾�Ʈ
	void SendHugeData(stringstream& SendStream, SOCKET Socket = 0); // 0�̸� Ŭ���̾�Ʈ
	bool IsNetworkOn();

	
	/////////////////////
	// only ����
	/////////////////////
	void RegisterCBF(void (*Connect)(CCompletionKey) = nullptr, void (*Disconnect)(CCompletionKey) = nullptr); // �ݹ��Լ��� ����մϴ�.
	CCompletionKey GetCompletionKey(SOCKET Socket);
	void ExecuteConnectCBF(CCompletionKey CompletionKey); // Ŭ���̾�Ʈ�� ������ �� �����մϴ�.
	void ExecuteDisconnectCBF(CCompletionKey CompletionKey); // Ŭ���̾�Ʈ�� ������ �� �����մϴ�.
	void Broadcast(stringstream& SendStream);
	void BroadcastExceptOne(stringstream& SendStream, SOCKET Except);


	/////////////////////
	// only Ŭ��
	/////////////////////

};