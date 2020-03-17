#pragma once

#include "Packet.h"

using namespace std;

// 패킷 처리 함수 포인터
struct FuncProcess
{
	// RecvStream은 수신한 정보, pSocket은 Overlapped I/O 작업이 발생한 IOCP 소켓 구조체 정보
	void(*funcProcessPacket)(stringstream& RecvStream, SOCKET Socket);
	FuncProcess()
	{
		funcProcessPacket = nullptr;
	}
};

class IocpServerBase
{
protected:
	stSOCKETINFO*	SocketInfo = nullptr;	 // 소켓 정보, 동적할당하고 Clients에 넘겨주는 용도
	SOCKET			ListenSocket;			 // 서버 리슨 소켓
	HANDLE			hIOCP;					 // IOCP 객체 핸들

	bool			bAccept;				 // 요청 동작 플래그

	bool			bWorkerThread;			 // 작업 스레드 동작 플래그
	HANDLE*			hWorkerHandle = nullptr; // 작업 스레드 핸들		
	DWORD			nThreadCnt;				 // 작업 스레드 개수

public:
	// WSAAccept한 모든 클라이언트의 new stSOCKETINFO()를 저장, (서버가 닫힐 때 할당 해제용도)
	static queue<stSOCKETINFO*> GC_SocketInfo;
	static CRITICAL_SECTION csGC_SocketInfo;

	// WSAAccept한 모든 클라이언트의 new stSOCKETINFO()를 저장, (delete 금지)
	static map<SOCKET, stSOCKETINFO*> Clients;
	static CRITICAL_SECTION csClients;

	// 수신한 데이터를 덱에 전부 적재
	static map<SOCKET, deque<char*>*> MapOfRecvDeque;
	static CRITICAL_SECTION csMapOfRecvDeque;


public:
	IocpServerBase();
	virtual ~IocpServerBase();

	// 소켓 등록 및 서버 정보 설정
	bool Initialize();

	// 서버 시작
	virtual void StartServer();

	// 작업 스레드 생성
	virtual bool CreateWorkerThread();

	// 작업 스레드
	virtual void WorkerThread();

	// 클라이언트 접속 종료
	virtual void CloseSocket(SOCKET Socket);

	// 클라이언트에게 송신
	virtual void Send(stringstream& SendStream, SOCKET Socket);

	// 클라이언트 수신 대기
	virtual void Recv(SOCKET Socket);

	///////////////////////////////////////////
	// stringstream의 맨 앞에 size를 추가
	///////////////////////////////////////////
	static bool AddSizeInStream(stringstream& DataStream, stringstream& FinalStream);

	///////////////////////////////////////////
	// 소켓 버퍼 크기 변경
	///////////////////////////////////////////
	void SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf);
};