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
	SOCKET			 ListenSocket;			// 서버 리슨 소켓
	HANDLE			 hIOCP;					// IOCP 객체 핸들

	bool			 bAccept;				// 서버 구동 확인용
	CRITICAL_SECTION csAccept;				// 동기화
	HANDLE			 hAcceptThreadHandle;	// Accept 스레드 핸들	

	bool			 bIOThread;				// IO 스레드 구동 관리용
	HANDLE*			 hIOThreadHandle;		// IO 스레드 핸들		
	DWORD			 nIOThreadCnt;			// IO 스레드 개수


	// WSAAccept(...)한 모든 클라이언트의 new stCompletionKey()를 저장
	map<SOCKET, class CCompletionKey*> Clients;
	CRITICAL_SECTION csClients;

	// 수신한 데이터를 덱에 전부 적재
	map<SOCKET, deque<char*>*> MapOfRecvDeque;
	CRITICAL_SECTION csMapOfRecvDeque;

	// WSASend(...)를 실행하면 ++, 실행이 완료되거나 실패하면 --
	unsigned int CountOfSend;
	CRITICAL_SECTION csCountOfSend;


	class CNetworkComponent* NetworkComponent = nullptr;


private:
	///////////////////////////////////////////
	// Main Functions
	///////////////////////////////////////////
	// 초기화 실패시 실행
	void CloseListenSocketAndCleanupWSA();

	/// public: 소켓 등록 및 서버 정보 설정
	///bool Initialize(const char* IPv4, USHORT Port);

	// Accept 스레드 생성
	bool CreateAcceptThread();

	/// public: Accept 스레드 실행
	///void AcceptThread();

	// IO 스레드 생성
	bool CreateIOThread();

	/// public: IO 스레드 실행
	///void IOThread();

	// 클라이언트 접속 종료
	void CloseSocket(SOCKET Socket, class COverlappedMsg* OverlappedMsg);

	/// public: 서버 종료
	///void CloseServer();

	/// public: 클라이언트에게 송신
	///void Send(stringstream& SendStream, SOCKET Socket);

	/// public: 송신하려는 데이터의 크기가 (MAX_BUFFER - 6)를 넘을 것 같을 때 사용합니다.
	///void SendHugeData(stringstream& SendStream, SOCKET Socket);

	// 클라이언트 수신 대기
	void Recv(SOCKET Socket, class COverlappedMsg* ReceivedOverlappedMsg);


	///////////////////////////////////////////
	// Sub Functions
	///////////////////////////////////////////
	// 소켓 버퍼 크기 변경
	void SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf);

	// recvDeque에 수신한 데이터를 적재
	bool LoadUpReceivedDataToRecvDeque(SOCKET Socket, class COverlappedMsg* OverlappedMsg, int NumberOfBytesTransferred, deque<char*>*& RecvDeque);

	// 수신한 데이터를 저장하는 덱에서 데이터를 획득
	void GetDataFromRecvDeque(char* DataBuffer, deque<char*>* RecvDeque);

	// 덱에서 획득한 데이터를 패킷들로 분할하고 최종적으로 패킷을 처리합니다.
	void DivideDataToPacketAndProcessThePacket(char* DataBuffer, deque<char*>* RecvDeque, SOCKET Socket);

	// 패킷을 처리합니다.
	void ProcessThePacket(char* DataBuffer, SOCKET Socket);

	// stringstream의 맨 앞에 size를 추가
	bool AddSizeInStream(stringstream& DataStream, stringstream& FinalStream);

	// (디버깅용) 패킷 사이즈와 실제 길이 검증용 함수
	void VerifyPacket(char* DataBuffer, bool bSend);

public:
	///////////////////////////////////////////
	// Thread Call
	///////////////////////////////////////////
	// Accept 스레드 실행
	void AcceptThread();

	// IO 스레드 실행
	void IOThread();


	///////////////////////////////////////////
	// NetworkComponent
	///////////////////////////////////////////
	// 싱글턴 객체 가져오기
	static CServer* GetSingleton();

	// 네트워크 컴퍼넌트 설정
	void SetNetworkComponent(class CNetworkComponent* NC);

	// 소켓 등록 및 서버 정보 설정
	bool Initialize(const char* IPv4, USHORT Port);

	// 서버 종료
	void CloseServer();

	// 클라이언트에게 송신
	void Send(stringstream& SendStream, SOCKET Socket);

	// 송신하려는 데이터의 크기가 (MAX_BUFFER - 6)를 넘을 것 같을 때 사용합니다.
	void SendHugeData(stringstream& SendStream, SOCKET Socket);

	// Socket에 해당하는 클라이언트의 CCompletionKey을 획득
	CCompletionKey GetCompletionKey(SOCKET Socket);

	// 서버 구동 확인
	bool IsServerOn();


	////////////////////////
	// 통신
	////////////////////////
	void Broadcast(stringstream& SendStream);
	void BroadcastExceptOne(stringstream& SendStream, SOCKET Except);
};
