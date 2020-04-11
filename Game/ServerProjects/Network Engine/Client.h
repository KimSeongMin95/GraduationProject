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
	SOCKET	ServerSocket;		// 서버에 연결할 소켓	

	bool	bAccept;			// 클라이언트 구동 확인용
	CRITICAL_SECTION csAccept;	// 동기화
	HANDLE	hMainHandle;		// 클라이언트 스레드 핸들

	deque<char*> RecvDeque;		// 수신한 데이터를 덱에 전부 적재


	class CNetworkComponent* NetworkComponent = nullptr;


private:
	///////////////////////////////////////////
	// Main Functions
	///////////////////////////////////////////
	// 초기화 실패시 실행
	void CloseServerSocketAndCleanupWSA();

	/// public: 소켓 등록 및 서버 접속 시도
	/// bool Initialize(const char* IPv4, USHORT Port);

	// 클라이언트 스레드 생성
	bool CreateClientThread();

	/// public: 클라이언트 스레드 실행
	/// void ClientThread();

	/// public: 클라이언트 종료
	/// void CloseClient();

	/// Not Member Function
	/// void CALLBACK SendCompletionRoutine(IN DWORD dwError, IN DWORD cbTransferred, IN LPWSAOVERLAPPED lpOverlapped, IN DWORD dwFlags);

	/// public: 서버에게 송신
	/// void Send(stringstream& SendStream);

	/// public: 패킷의 크기가 (MAX_BUFFER - 6)를 넘을 것 같을 때 사용합니다.
	///void SendHugePacket(stringstream& SendStream, SOCKET Socket);


	///////////////////////////////////////////
	// Sub Functions
	///////////////////////////////////////////
	// 소켓 버퍼 크기 변경
	void SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf);

	// recvDeque에 수신한 데이터를 적재
	bool LoadUpReceivedDataToRecvDeque(char* RecvBuffer, int RecvLen);

	// 수신한 데이터를 저장하는 덱에서 데이터를 획득
	void GetDataFromRecvDeque(char* DataBuffer);

	// 덱에서 획득한 데이터를 패킷들로 분할하고 최종적으로 패킷을 처리합니다.
	void DivideDataToPacketAndProcessThePacket(char* DataBuffer);

	// 패킷을 처리합니다.
	void ProcessThePacket(char* DataBuffer);

	// stringstream의 맨 앞에 size를 추가
	bool AddSizeInStream(stringstream& DataStream, stringstream& FinalStream);

	// (디버깅용) 패킷 사이즈와 실제 길이 검증용 함수
	void VerifyPacket(char* DataBuffer, bool send);


public:
	///////////////////////////////////////////
	// Thread Call
	///////////////////////////////////////////
	// 클라이언트 스레드 실행
	void ClientThread();

	///////////////////////////////////////////
	// NetworkComponent
	///////////////////////////////////////////
	// 싱글턴 객체 가져오기
	static CClient* GetSingleton();

	// 네트워크 컴퍼넌트 설정
	void SetNetworkComponent(class CNetworkComponent* NC);

	// 소켓 등록 및 서버 접속 시도
	bool Initialize(const char* IPv4, USHORT Port);

	// 클라이언트 종료
	void CloseClient();

	// 서버에게 송신
	void Send(stringstream& SendStream);

	// 패킷의 크기가 (MAX_BUFFER - 6)를 넘을 것 같을 때 사용합니다.
	void SendHugePacket(stringstream& SendStream);

	// 클라이언트의 CCompletionKey을 획득
	CCompletionKey GetCompletionKey();

	// 서버 구동 확인
	bool IsClientOn();
};