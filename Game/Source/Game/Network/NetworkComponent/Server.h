// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NetworkInterface.h"

class CServer final : public INetworkInterface
{
public:
	CServer();
	virtual ~CServer();

private:
	CProcessingFuncPtr  ProcFuncs[MAX_HEADER]; // 수신한 패킷을 처리하는 함수 포인터
	CCallBackFuncPtr    ConCBF;		   // 클라이언트가 접속하면 실행할 콜백함수 포인터
	CCallBackFuncPtr    DisconCBF;		   // 클라이언트와 접속이 종료되면 실행할 콜백함수 포인터

	SOCKET ListenSocket;		      // 서버의 리슨 소켓
	HANDLE hIOCP;			      // IOCP 객체 핸들

	bool bAccept;			      // 서버 구동 확인용
	CRITICAL_SECTION csAccept;	      // 서버 구동 확인 동기화
	HANDLE hAcceptThreadHandle;           // Accept 스레드 핸들	

	unique_ptr<HANDLE[]> hIOThreadHandle; // IO 스레드 핸들		
	DWORD nIOThreadCnt;		      // IO 스레드 개수

	// 접속한 모든 클라이언트의 소켓을 저장합니다.
	unordered_set <SOCKET> Clients;
	CRITICAL_SECTION csClients;

	// 접속한 모든 클라이언트의 CCompletionKey를 저장합니다.
	unordered_map<SOCKET, shared_ptr<CCompletionKey>> ClientsCK; // CreateIoCompletionPort(...)에 넘겨주어야 하기 때문에 CCompletionKey를 동적할당해야 합니다.
	CRITICAL_SECTION csClientsCK;

	// 접속한 모든 클라이언트의 COverlappedMsg를 저장합니다.
	unordered_map<SOCKET, shared_ptr<COverlappedMsg>> ClientsOM; // WSARecv(...)에 넘겨주어야 하기 때문에 COverlappedMsg를 동적할당해야 합니다.
	CRITICAL_SECTION csClientsOM;

	// 접속한 모든 클라이언트로부터 수신한 모든 데이터를 적재합니다.
	unordered_map<SOCKET, shared_ptr<deque<unique_ptr<char[]>>>> RecvDeques;
	CRITICAL_SECTION csRecvDeques;

	// 송신한 모든 패킷의 완료를 확인하기 위해 WSASend(...)를 실행하면 ++, 실행이 완료되거나 실패하면 -- 되게 합니다.
	uint32_t CountOfSend;
	CRITICAL_SECTION csCountOfSend;

private:
	//////////////////////////////////////////
	// INetworkInterface virtual Functions
	//////////////////////////////////////////
	// 서버의 초기화와 Accept 스레드 생성을 시도합니다.
	virtual bool Initialize(const char* const IPv4, const USHORT& Port) final;

	// 서버의 정상적인 구동을 확인합니다.
	virtual bool IsNetworkOn() final;

	// Socket에 해당하는 클라이언트의 CCompletionKey를 획득합니다.
	virtual CCompletionKey GetCompletionKey(const SOCKET& Socket) final;

	// 서버의 구동을 종료합니다.
	virtual void Close() final;

	// 패킷의 헤더에 대응하여 실행할 정적 함수를 등록합니다.
	virtual void RegisterHeaderAndStaticFunc(const uint16_t& PacketHeader, void(*StaticFunc)(stringstream&, const SOCKET&)) final;

	// 클라이언트가 접속하면 실행할 정적 콜백함수를 등록합니다.
	virtual void RegisterConCBF(void(*StaticCBFunc)(CCompletionKey)) final;

	// 클라이언트가 접속을 종료하면 실행할 정적 콜백함수를 등록합니다.
	virtual void RegisterDisconCBF(void(*StaticCBFunc)(CCompletionKey)) final;

	// 클라이언트에게 패킷을 송신합니다.
	virtual void Send(CPacket& Packet, const SOCKET& Socket = NULL) final;

	// 모든 클라이언트에게 패킷을 송신합니다.
	virtual void Broadcast(CPacket& Packet) final;

	// 해당 클라이언트를 제외하고 모든 클라이언트에게 패킷을 송신합니다.
	virtual void BroadcastExceptOne(CPacket& Packet, const SOCKET& Except) final;

	////////////////////
	// Main Functions
	////////////////////
	// Accept 스레드를 생성합니다.
	bool CreateAcceptThread();

	// IO 스레드를 생성합니다.
	bool CreateIOThread();

	// 클라이언트의 접속 종료에 대한 처리를 진행합니다.
	void CloseSocket(const SOCKET& Socket);

	// WSASend(...)함수로 패킷인 OverlappedMsg를 비동기 중첩 방식으로 송신합니다. 
	void Send(COverlappedMsg* OverlappedMsg, const SOCKET& Socket);

	// 클라이언트 패킷의 수신완료 통지를 기다립니다.
	void Recv(const SOCKET& Socket, COverlappedMsg* OverlappedMsg);

	///////////////////
	// Sub Functions
	///////////////////
	// WSASend로 인해 송신 완료 통지를 받으면 동적할당한 overlappedMsg을 해제합니다.
	bool ProcessingSendingInIOThread(const DWORD& BytesTransferred, COverlappedMsg* OverlappedMsg);

	// RecvDeque의 공유 포인터를 획득합니다.
	shared_ptr<deque<unique_ptr<char[]>>> GetRecvDeque(const SOCKET& Socket);

	// RecvDeque의 뒷부분에 수신한 데이터를 적재합니다.
	void LoadUpReceivedDataToRecvDeque(const SOCKET& Socket, COverlappedMsg* OverlappedMsg, const int& RecvLen, shared_ptr<deque<unique_ptr<char[]>>> RecvDeque);

	// RecvDeque으로부터 패킷들을 획득합니다.
	void GetPacketsFromRecvDeque(char* const BufOfPackets, shared_ptr<deque<unique_ptr<char[]>>> RecvDeque);

	// 획득한 패킷들을 패킷으로 분할하고 최종적으로 패킷을 처리합니다.
	void DividePacketsAndProcessThePacket(const char* const BufOfPackets, const SOCKET& Socket);

	// 패킷을 처리합니다.
	void ProcessThePacket(const char* const BufOfPacket, const SOCKET& Socket);

	// 송신하려는 데이터의 크기를 조절합니다. 
	void SetSizeOfDataForSend(const uint32_t& IdxOfStart, uint32_t& IdxOfEnd, const uint16_t& MaxSizeOfData, const char* const C_StrOfData);

	// 비동기 중첩 방식으로 송신할 OverlappedMsg을 동적할당하고 획득합니다.
	COverlappedMsg* GetOverlappedMsgForSend(const string& StrOfLengthAndHeader, const size_t& LenOfLengthAndHeader, const char* const C_StrOfData, const uint32_t& IdxOfStart, const uint16_t& sizeOfData);

public:
	//////////////////
	// Thread Call
	//////////////////
	// Accept 스레드를 실행합니다.
	void RunAcceptThread();

	// IO 스레드를 실행합니다.
	void RunIOThread();
};
