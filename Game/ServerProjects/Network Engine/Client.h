#pragma once

#include "Packet.h"

class CClient
{
public:
	///////////////////////////////////////////
	// Basic Functions
	///////////////////////////////////////////
	CClient();
	~CClient();

private:
	SOCKET	ServerSocket;		// ������ ������ ����	

	bool	bAccept;			// Ŭ���̾�Ʈ ���� Ȯ�ο�
	CRITICAL_SECTION csAccept;	// ����ȭ
	HANDLE	hMainHandle;		// Ŭ���̾�Ʈ ������ �ڵ�

	deque<char*> RecvDeque;		// ������ �����͸� ���� ���� ����


	class CNetworkComponent* NetworkComponent = nullptr;


private:
	///////////////////////////////////////////
	// Main Functions
	///////////////////////////////////////////
	// �ʱ�ȭ ���н� ����
	void CloseServerSocketAndCleanupWSA();

	/// public: ���� ��� �� ���� ���� �õ�
	/// bool Initialize(const char* IPv4, USHORT Port);

	// Ŭ���̾�Ʈ ������ ����
	bool CreateClientThread();

	/// public: Ŭ���̾�Ʈ ������ ����
	/// void ClientThread();

	/// public: Ŭ���̾�Ʈ ����
	/// void CloseClient();

	/// Not Member Function
	/// void CALLBACK SendCompletionRoutine(IN DWORD dwError, IN DWORD cbTransferred, IN LPWSAOVERLAPPED lpOverlapped, IN DWORD dwFlags);

	/// public: �������� �۽�
	/// void Send(stringstream& SendStream);

	/// public: ��Ŷ�� ũ�Ⱑ (MAX_BUFFER - 6)�� ���� �� ���� �� ����մϴ�.
	///void SendHugePacket(stringstream& SendStream, SOCKET Socket);


	///////////////////////////////////////////
	// Sub Functions
	///////////////////////////////////////////
	// ���� ���� ũ�� ����
	void SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf);

	// recvDeque�� ������ �����͸� ����
	bool LoadUpReceivedDataToRecvDeque(char* RecvBuffer, int RecvLen);

	// ������ �����͸� �����ϴ� ������ �����͸� ȹ��
	void GetDataFromRecvDeque(char* DataBuffer);

	// ������ ȹ���� �����͸� ��Ŷ��� �����ϰ� ���������� ��Ŷ�� ó���մϴ�.
	void DivideDataToPacketAndProcessThePacket(char* DataBuffer);

	// ��Ŷ�� ó���մϴ�.
	void ProcessThePacket(char* DataBuffer);

	// stringstream�� �� �տ� size�� �߰�
	bool AddSizeInStream(stringstream& DataStream, stringstream& FinalStream);

	// (������) ��Ŷ ������� ���� ���� ������ �Լ�
	void VerifyPacket(char* DataBuffer, bool send);


public:
	///////////////////////////////////////////
	// Thread Call
	///////////////////////////////////////////
	// Ŭ���̾�Ʈ ������ ����
	void ClientThread();

	///////////////////////////////////////////
	// NetworkComponent
	///////////////////////////////////////////
	// �̱��� ��ü ��������
	static CClient* GetSingleton();

	// ��Ʈ��ũ ���۳�Ʈ ����
	void SetNetworkComponent(class CNetworkComponent* NC);

	// ���� ��� �� ���� ���� �õ�
	bool Initialize(const char* IPv4, USHORT Port);

	// Ŭ���̾�Ʈ ����
	void CloseClient();

	// �������� �۽�
	void Send(stringstream& SendStream);

	// ��Ŷ�� ũ�Ⱑ (MAX_BUFFER - 6)�� ���� �� ���� �� ����մϴ�.
	void SendHugePacket(stringstream& SendStream);

	// Ŭ���̾�Ʈ�� CCompletionKey�� ȹ��
	CCompletionKey GetCompletionKey();

	// ���� ���� Ȯ��
	bool IsClientOn();
};