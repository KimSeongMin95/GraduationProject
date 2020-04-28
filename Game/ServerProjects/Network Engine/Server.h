// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Packet.h"

class CServer
{
public:
	///////////////////////////////////////////
	// Basic Functions
	///////////////////////////////////////////
	CServer();
	~CServer();


private:
	SOCKET			 ListenSocket;			// ���� ���� ����
	HANDLE			 hIOCP;					// IOCP ��ü �ڵ�

	bool			 bAccept;				// ���� ���� Ȯ�ο�
	CRITICAL_SECTION csAccept;				// ����ȭ
	HANDLE			 hAcceptThreadHandle;	// Accept ������ �ڵ�	

	bool			 bIOThread;				// IO ������ ���� ������
	HANDLE*			 hIOThreadHandle;		// IO ������ �ڵ�		
	DWORD			 nIOThreadCnt;			// IO ������ ����


	// WSAAccept(...)�� ��� Ŭ���̾�Ʈ�� new stCompletionKey()�� ����
	map<SOCKET, class CCompletionKey*> Clients;
	CRITICAL_SECTION csClients;

	// ������ �����͸� ���� ���� ����
	map<SOCKET, deque<char*>*> MapOfRecvDeque;
	CRITICAL_SECTION csMapOfRecvDeque;

	// WSASend(...)�� �����ϸ� ++, ������ �Ϸ�ǰų� �����ϸ� --
	unsigned int CountOfSend;
	CRITICAL_SECTION csCountOfSend;


	class CNetworkComponent* NetworkComponent = nullptr;


private:
	///////////////////////////////////////////
	// Main Functions
	///////////////////////////////////////////
	// �ʱ�ȭ ���н� ����
	void CloseListenSocketAndCleanupWSA();

	/// public: ���� ��� �� ���� ���� ����
	///bool Initialize(const char* IPv4, USHORT Port);

	// Accept ������ ����
	bool CreateAcceptThread();

	/// public: Accept ������ ����
	///void AcceptThread();

	// IO ������ ����
	bool CreateIOThread();

	/// public: IO ������ ����
	///void IOThread();

	// Ŭ���̾�Ʈ ���� ����
	void CloseSocket(SOCKET Socket, class COverlappedMsg* OverlappedMsg);

	/// public: ���� ����
	///void CloseServer();

	/// public: Ŭ���̾�Ʈ���� �۽�
	///void Send(stringstream& SendStream, SOCKET Socket);

	/// public: �۽��Ϸ��� �������� ũ�Ⱑ (MAX_BUFFER - 6)�� ���� �� ���� �� ����մϴ�.
	///void SendHugeData(stringstream& SendStream, SOCKET Socket);

	// Ŭ���̾�Ʈ ���� ���
	void Recv(SOCKET Socket, class COverlappedMsg* ReceivedOverlappedMsg);


	///////////////////////////////////////////
	// Sub Functions
	///////////////////////////////////////////
	// ���� ���� ũ�� ����
	void SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf);

	// recvDeque�� ������ �����͸� ����
	bool LoadUpReceivedDataToRecvDeque(SOCKET Socket, class COverlappedMsg* OverlappedMsg, int NumberOfBytesTransferred, deque<char*>*& RecvDeque);

	// ������ �����͸� �����ϴ� ������ �����͸� ȹ��
	void GetDataFromRecvDeque(char* DataBuffer, deque<char*>* RecvDeque);

	// ������ ȹ���� �����͸� ��Ŷ��� �����ϰ� ���������� ��Ŷ�� ó���մϴ�.
	void DivideDataToPacketAndProcessThePacket(char* DataBuffer, deque<char*>* RecvDeque, SOCKET Socket);

	// ��Ŷ�� ó���մϴ�.
	void ProcessThePacket(char* DataBuffer, SOCKET Socket);

	// stringstream�� �� �տ� size�� �߰�
	bool AddSizeInStream(stringstream& DataStream, stringstream& FinalStream);

	// (������) ��Ŷ ������� ���� ���� ������ �Լ�
	void VerifyPacket(char* DataBuffer, bool bSend);

public:
	///////////////////////////////////////////
	// Thread Call
	///////////////////////////////////////////
	// Accept ������ ����
	void AcceptThread();

	// IO ������ ����
	void IOThread();


	///////////////////////////////////////////
	// NetworkComponent
	///////////////////////////////////////////
	// �̱��� ��ü ��������
	static CServer* GetSingleton();

	// ��Ʈ��ũ ���۳�Ʈ ����
	void SetNetworkComponent(class CNetworkComponent* NC);

	// ���� ��� �� ���� ���� ����
	bool Initialize(const char* IPv4, USHORT Port);

	// ���� ����
	void CloseServer();

	// Ŭ���̾�Ʈ���� �۽�
	void Send(stringstream& SendStream, SOCKET Socket);

	// �۽��Ϸ��� �������� ũ�Ⱑ (MAX_BUFFER - 6)�� ���� �� ���� �� ����մϴ�.
	void SendHugeData(stringstream& SendStream, SOCKET Socket);

	// Socket�� �ش��ϴ� Ŭ���̾�Ʈ�� CCompletionKey�� ȹ��
	CCompletionKey GetCompletionKey(SOCKET Socket);

	// ���� ���� Ȯ��
	bool IsServerOn();


	////////////////////////
	// ���
	////////////////////////
	void Broadcast(stringstream& SendStream);
	void BroadcastExceptOne(stringstream& SendStream, SOCKET Except);
};
