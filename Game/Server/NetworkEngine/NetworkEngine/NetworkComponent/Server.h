// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NetworkInterface.h"

class CServer sealed : public INetworkInterface
{
public:
	CServer();
	virtual ~CServer();

	static CServer* GetSingleton();

private:
	CProcessingFuncPtr   ProcFuncs[MAX_HEADER]; // ������ ��Ŷ�� ó���ϴ� �Լ� ������
	CCallBackFuncPtr	 ConCBF;				// Ŭ���̾�Ʈ�� �����ϸ� ������ �ݹ��Լ� ������
	CCallBackFuncPtr	 DisconCBF;			    // Ŭ���̾�Ʈ�� ������ ����Ǹ� ������ �ݹ��Լ� ������

	SOCKET				 ListenSocket;		    // ������ ���� ����
	HANDLE				 hIOCP;				    // IOCP ��ü �ڵ�

	bool				 bAccept;			    // ���� ���� Ȯ�ο�
	CRITICAL_SECTION	 csAccept;			    // ���� ���� Ȯ�� ����ȭ
	HANDLE				 hAcceptThreadHandle;   // Accept ������ �ڵ�	

	unique_ptr<HANDLE[]> hIOThreadHandle;	    // IO ������ �ڵ�		
	DWORD				 nIOThreadCnt;		    // IO ������ ����

	// ������ ��� Ŭ���̾�Ʈ�� ������ �����մϴ�.
	unordered_set <SOCKET> Clients;
	CRITICAL_SECTION csClients;

	// ������ ��� Ŭ���̾�Ʈ�� CCompletionKey�� �����մϴ�.
	unordered_map<SOCKET, shared_ptr<CCompletionKey>> ClientsCK; // CreateIoCompletionPort(...)�� �Ѱ��־�� �ϱ� ������ CCompletionKey�� �����Ҵ��ؾ� �մϴ�.
	CRITICAL_SECTION csClientsCK;

	// ������ ��� Ŭ���̾�Ʈ�� COverlappedMsg�� �����մϴ�.
	unordered_map<SOCKET, shared_ptr<COverlappedMsg>> ClientsOM; // WSARecv(...)�� �Ѱ��־�� �ϱ� ������ COverlappedMsg�� �����Ҵ��ؾ� �մϴ�.
	CRITICAL_SECTION csClientsOM;

	// ������ ��� Ŭ���̾�Ʈ�κ��� ������ ��� �����͸� �����մϴ�.
	unordered_map<SOCKET, shared_ptr<deque<unique_ptr<char[]>>>> RecvDeques;
	CRITICAL_SECTION csRecvDeques;

	/** �۽��� ��� ��Ŷ�� �ϷḦ Ȯ���ϱ� ���� WSASend(...)�� �����ϸ� ++, ������ �Ϸ�ǰų� �����ϸ� -- �ǰ� �մϴ�.
	�޸� ����ȭ�� interlock�� ����մϴ�.*/
	uint32_t CountOfSend;

private:
	//////////////////////////////////////////
	// INetworkInterface virtual Functions
	//////////////////////////////////////////
	// ������ �ʱ�ȭ�� Accept ������ ������ �õ��մϴ�.
	virtual bool Initialize(const char* const IPv4, const USHORT& Port) final;

	// ������ �������� ������ Ȯ���մϴ�.
	virtual bool IsNetworkOn() final;

	// Socket�� �ش��ϴ� Ŭ���̾�Ʈ�� CCompletionKey�� ȹ���մϴ�.
	virtual CCompletionKey GetCompletionKey(const SOCKET& Socket) final;

	// ������ ������ �����մϴ�.
	virtual void Close() final;

	// ��Ŷ�� ����� �����Ͽ� ������ ���� �Լ��� ����մϴ�.
	virtual void RegisterHeaderAndStaticFunc(const uint16_t& PacketHeader, void(*StaticFunc)(stringstream&, const SOCKET&)) final;

	// Ŭ���̾�Ʈ�� �����ϸ� ������ ���� �ݹ��Լ��� ����մϴ�.
	virtual void RegisterConCBF(void(*StaticCBFunc)(CCompletionKey)) final;

	// Ŭ���̾�Ʈ�� ������ �����ϸ� ������ ���� �ݹ��Լ��� ����մϴ�.
	virtual void RegisterDisconCBF(void(*StaticCBFunc)(CCompletionKey)) final;

	// Ŭ���̾�Ʈ���� ��Ŷ�� �۽��մϴ�.
	virtual void Send(CPacket& Packet, const SOCKET& Socket = NULL) final;

	// ��� Ŭ���̾�Ʈ���� ��Ŷ�� �۽��մϴ�.
	virtual void Broadcast(CPacket& Packet) final;

	// �ش� Ŭ���̾�Ʈ�� �����ϰ� ��� Ŭ���̾�Ʈ���� ��Ŷ�� �۽��մϴ�.
	virtual void BroadcastExceptOne(CPacket& Packet, const SOCKET& Except) final;

	////////////////////
	// Main Functions
	////////////////////
	// Accept �����带 �����մϴ�.
	bool CreateAcceptThread();

	// IO �����带 �����մϴ�.
	bool CreateIOThread();

	// Ŭ���̾�Ʈ�� ���� ���ῡ ���� ó���� �����մϴ�.
	void CloseSocket(const SOCKET& Socket);

	// WSASend(...)�Լ��� ��Ŷ�� OverlappedMsg�� �񵿱� ��ø ������� �۽��մϴ�. 
	void Send(COverlappedMsg* OverlappedMsg, const SOCKET& Socket);

	// Ŭ���̾�Ʈ ��Ŷ�� ���ſϷ� ������ ��ٸ��ϴ�.
	void Recv(const SOCKET& Socket, COverlappedMsg* OverlappedMsg);

	///////////////////
	// Sub Functions
	///////////////////
	// WSASend�� ���� �۽� �Ϸ� ������ ������ �����Ҵ��� overlappedMsg�� �����մϴ�.
	bool ProcessingSendingInIOThread(const DWORD& BytesTransferred, COverlappedMsg* OverlappedMsg);

	// RecvDeque�� ���� �����͸� ȹ���մϴ�.
	shared_ptr<deque<unique_ptr<char[]>>> GetRecvDeque(const SOCKET& Socket);

	// RecvDeque�� �޺κп� ������ �����͸� �����մϴ�.
	void LoadUpReceivedDataToRecvDeque(const SOCKET& Socket, COverlappedMsg* OverlappedMsg, const int& RecvLen, shared_ptr<deque<unique_ptr<char[]>>> RecvDeque);

	// RecvDeque���κ��� ��Ŷ���� ȹ���մϴ�.
	void GetPacketsFromRecvDeque(char* const BufOfPackets, shared_ptr<deque<unique_ptr<char[]>>> RecvDeque);

	// ȹ���� ��Ŷ���� ��Ŷ���� �����ϰ� ���������� ��Ŷ�� ó���մϴ�.
	void DividePacketsAndProcessThePacket(const char* const BufOfPackets, const SOCKET& Socket);

	// ��Ŷ�� ó���մϴ�.
	void ProcessThePacket(const char* const BufOfPacket, const SOCKET& Socket);

	// �۽��Ϸ��� �������� ũ�⸦ �����մϴ�. 
	void SetSizeOfDataForSend(const uint32_t& IdxOfStart, uint32_t& IdxOfEnd, const uint16_t& MaxSizeOfData, const char* const C_StrOfData);

	// �񵿱� ��ø ������� �۽��� OverlappedMsg�� �����Ҵ��ϰ� ȹ���մϴ�.
	COverlappedMsg* GetOverlappedMsgForSend(const string& StrOfLengthAndHeader, const size_t& LenOfLengthAndHeader, const char* const C_StrOfData, const uint32_t& IdxOfStart, const uint16_t& sizeOfData);

public:
	//////////////////
	// Thread Call
	//////////////////
	// Accept �����带 �����մϴ�.
	void RunAcceptThread();

	// IO �����带 �����մϴ�.
	void RunIOThread();
};
