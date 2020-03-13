#pragma once

#include "Packet.h"

using namespace std;

// 패킷 처리 함수 포인터
struct FuncProcess
{
	// RecvStream은 수신한 정보, pSocket은 Overlapped I/O 작업이 발생한 IOCP 소켓 구조체 정보
	void(*funcProcessPacket)(stringstream& RecvStream, stSOCKETINFO* pSocketInfo);
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
	// WSAAccept한 모든 클라이언트의 new stSOCKETINFO()를 저장
	static std::map<SOCKET, stSOCKETINFO*> Clients;
	static CRITICAL_SECTION csClients;

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
	virtual void CloseSocket(stSOCKETINFO* pSocketInfo);

	// 클라이언트에게 송신
	virtual void Send(stringstream& SendStream, stSOCKETINFO* pSocketInfo);

	// 클라이언트 수신 대기
	virtual void Recv(stSOCKETINFO* pSocketInfo);

	///////////////////////////////////////////
	// stringstream의 맨 앞에 size를 추가
	///////////////////////////////////////////
	static void AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
	{
		if (DataStream.str().length() == 0)
		{
			printf_s("[ERROR] <AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
			return;
		}
		printf_s("[START] <AddSizeInStream(...)> \n");

		// ex) DateStream의 크기 : 98
		printf_s("\t DataStream size: %d\n", (int)DataStream.str().length());
		printf_s("\t DataStream: %s\n", DataStream.str().c_str());

		// dataStreamLength의 크기 : 3 [98 ]
		stringstream dataStreamLength;
		dataStreamLength << DataStream.str().length() << endl;

		// lengthOfFinalStream의 크기 : 4 [101 ]
		stringstream lengthOfFinalStream;
		lengthOfFinalStream << (dataStreamLength.str().length() + DataStream.str().length()) << endl;

		// FinalStream의 크기 : 101 [101 DataStream]
		int sizeOfFinalStream = (int)(lengthOfFinalStream.str().length() + DataStream.str().length());
		FinalStream << sizeOfFinalStream << endl;
		FinalStream << DataStream.str(); // 이미 DataStream.str() 마지막에 endl;를 사용했으므로 여기선 다시 사용하지 않습니다.

		printf_s("\t FinalStream size: %d\n", (int)FinalStream.str().length());
		printf_s("\t FinalStream: %s\n", FinalStream.str().c_str());


		printf_s("[END] <AddSizeInStream(...)> \n");
	}
};