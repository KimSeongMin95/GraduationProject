// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Packet.h"

#include "CoreMinimal.h"

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

/**
 * 게임 클라이언트와 접속 및 패킷 처리를 담당하는 클래스 (게임 서버)
 */
class GAME_API cGameServer
{
private:
	FuncProcess	fnProcess[100];	// 패킷 처리 구조체

	static int ServerPort;
	static CRITICAL_SECTION csServerPort;

protected:
	SOCKET			 ListenSocket;			// 서버 리슨 소켓
	HANDLE			 hIOCP;					// IOCP 객체 핸들

	bool			 bAccept;				// 요청 동작 플래그
	CRITICAL_SECTION csAccept;				//
	HANDLE			 hAcceptThreadHandle;	// Accept 스레드 핸들	

	bool			 bIOThread;
	HANDLE*			 hIOThreadHandle;		// IO 스레드 핸들		
	DWORD			 nIOThreadCnt;			// IO 스레드 개수


public:
	/** 게임서버의 임시 소켓 */
	static SOCKET SocketID;
	static CRITICAL_SECTION csSocketID;

	static int PossessedID;
	static CRITICAL_SECTION csPossessedID;


	// WSAAccept(...)한 모든 클라이언트의 new stCompletionKey()를 저장
	static unordered_map<SOCKET, stCompletionKey*> GameClients;
	static CRITICAL_SECTION csGameClients;

	// 수신한 데이터를 덱에 전부 적재
	static unordered_map<SOCKET, deque<char*>*> MapOfRecvDeque;
	static CRITICAL_SECTION csMapOfRecvDeque;

	// WSASend(...)를 실행하면 ++, 실행이 완료되거나 실패하면 --
	static unsigned int CountOfSend;
	static CRITICAL_SECTION csCountOfSend;

	/**************************************************/

	// Connected 클라이언트의 InfoOfPlayer 저장
	static unordered_map<SOCKET, cInfoOfPlayer> InfoOfClients;
	static CRITICAL_SECTION csInfoOfClients;

	static unordered_map<SOCKET, cInfoOfScoreBoard> InfosOfScoreBoard;
	static CRITICAL_SECTION csInfosOfScoreBoard;

	static unordered_map<SOCKET, SOCKET> Observers;
	static CRITICAL_SECTION csObservers;


	///////////////////////////////////////////
	// Pioneer 세분화
	///////////////////////////////////////////
	static unordered_map<int, cInfoOfPioneer_Socket> InfosOfPioneer_Socket;
	static CRITICAL_SECTION csInfosOfPioneer_Socket;

	static unordered_map<int, cInfoOfPioneer_Animation> InfosOfPioneer_Animation;
	static CRITICAL_SECTION csInfosOfPioneer_Animation;

	static unordered_map<int, cInfoOfPioneer_Stat> InfosOfPioneer_Stat;
	static CRITICAL_SECTION csInfosOfPioneer_Stat;


	///////////////////////////////////////////
	// Building 세분화
	///////////////////////////////////////////
	static unordered_map<int, cInfoOfBuilding_Spawn> InfoOfBuilding_Spawn;
	static CRITICAL_SECTION csInfoOfBuilding_Spawn;

	static unordered_map<int, cInfoOfBuilding_Stat> InfoOfBuilding_Stat;
	static CRITICAL_SECTION csInfoOfBuilding_Stat;


	///////////////////////////////////////////
	// Enemy 세분화
	///////////////////////////////////////////
	static unordered_map<int, cInfoOfEnemy_Spawn> InfoOfEnemies_Spawn;
	static CRITICAL_SECTION csInfoOfEnemies_Spawn;

	static unordered_map<int, cInfoOfEnemy_Animation> InfoOfEnemies_Animation;
	static CRITICAL_SECTION csInfoOfEnemies_Animation;

	static unordered_map<int, cInfoOfEnemy_Stat> InfoOfEnemies_Stat;
	static CRITICAL_SECTION csInfoOfEnemies_Stat;


public:
	////////////////////////
	// 기본
	////////////////////////
	cGameServer();
	~cGameServer();

	// 초기화 실패시 실행
	void CloseListenSocketAndCleanupWSA();

	// 소켓 등록 및 서버 정보 설정
	bool Init();

	// Accept 스레드 생성
	bool CreateAcceptThread();

	// 서버 시작
	void AcceptThread();

	// IO 스레드 생성
	bool CreateIOThread();

	// 작업 스레드
	void IOThread();

	// 클라이언트 접속 종료
	static void CloseSocket(SOCKET Socket, stOverlappedMsg* OverlappedMsg);

	// 서버 종료
	void Close();

	// 클라이언트에게 송신
	static void Send(stringstream& SendStream, SOCKET Socket);

	// 클라이언트 수신 대기
	static void Recv(SOCKET Socket, stOverlappedMsg* ReceivedOverlappedMsg);

	///////////////////////////////////////////
	// stringstream의 맨 앞에 size를 추가
	///////////////////////////////////////////
	static bool AddSizeInStream(stringstream& DataStream, stringstream& FinalStream);

	///////////////////////////////////////////
	// 소켓 버퍼 크기 변경
	///////////////////////////////////////////
	void SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf);

	///////////////////////////////////////////
	// 수신한 데이터를 저장하는 덱에서 데이터를 획득
	///////////////////////////////////////////
	void GetDataInRecvDeque(deque<char*>* RecvDeque, char* DataBuffer);

	///////////////////////////////////////////
	// 패킷을 처리합니다.
	///////////////////////////////////////////
	void ProcessReceivedPacket(char* DataBuffer, SOCKET Socket);

	////////////////////////////////////////////////
	// 대용량 패킷 분할 
	////////////////////////////////////////////////
	template<typename T>
	static void DivideHugePacket(SOCKET Socket, stringstream& SendStream, EPacketType PacketType, T& queue);

	//////////////////////////////////////////////////
	//// (임시) 패킷 사이즈와 실제 길이 검증용 함수
	//////////////////////////////////////////////////
	//static void VerifyPacket(char* DataBuffer, bool send);

	// 싱글턴 객체 가져오기
	static cGameServer* GetSingleton()
	{
		static cGameServer gameServer;
		return &gameServer;
	}


	////////////////////////
	// 확인
	////////////////////////
	bool IsServerOn();
	int GetServerPort();

	////////////////////////
	// 통신
	////////////////////////
	static void Broadcast(stringstream& SendStream);
	static void BroadcastExceptOne(stringstream& SendStream, SOCKET Except);

	static void Connected(stringstream& RecvStream, SOCKET Socket);

	static void ScoreBoard(stringstream& RecvStream, SOCKET Socket);

	static void SendSpaceShip(cInfoOfSpaceShip InfoOfSpaceShip);

	static void Observation(stringstream& RecvStream, SOCKET Socket);
	static int SizeOfObservers();
	static void InsertAtObersers(SOCKET Socket);

	static void SendSpawnPioneer(cInfoOfPioneer InfoOfPioneer);
	static void SendSpawnedPioneer(SOCKET Socket);

	static void DiedPioneer(stringstream& RecvStream, SOCKET Socket);
	static cThreadSafetyQueue<int> tsqDiedPioneer;

	static void InfoOfPioneer_Animation(stringstream& RecvStream, SOCKET Socket);
	static cThreadSafetyQueue<cInfoOfPioneer_Animation> tsqInfoOfPioneer_Animation;

	static void PossessPioneer(stringstream& RecvStream, SOCKET Socket);
	bool PossessingPioneer(cInfoOfPioneer_Socket Socket);
	static cThreadSafetyQueue<cInfoOfPioneer_Socket> tsqInfoOfPioneer_Socket;

	static void InfoOfPioneer_Stat(stringstream& RecvStream, SOCKET Socket);
	static cThreadSafetyQueue<cInfoOfPioneer_Stat> tsqInfoOfPioneer_Stat;

	static void SendInfoOfProjectile(cInfoOfProjectile InfoOfProjectile);
	static void InfoOfProjectile(stringstream& RecvStream, SOCKET Socket);
	static cThreadSafetyQueue<cInfoOfProjectile> tsqInfoOfProjectile;

	static void SendInfoOfResources(cInfoOfResources InfoOfResources);

	static void SendInfoOfBuilding_Spawn(cInfoOfBuilding_Spawn Spawn);
	static void SendInfoOfBuilding_Spawned(SOCKET Socket);
	static void RecvInfoOfBuilding_Spawn(stringstream& RecvStream, SOCKET Socket);
	static cThreadSafetyQueue<cInfoOfBuilding_Spawn> tsqInfoOfBuilding_Spawn;

	static void SendInfoOfBuilding_Stat(stringstream& RecvStream, SOCKET Socket);

	static void SendDestroyBuilding(int IDOfBuilding);


	static void SendSpawnEnemy(cInfoOfEnemy InfoOfEnemy);
	static void SendSpawnedEnemy(SOCKET Socket);

	static void SendInfoOfEnemy_Animation(stringstream& RecvStream, SOCKET Socket);

	static void SendInfoOfEnemy_Stat(stringstream& RecvStream, SOCKET Socket);

	static void SendDestroyEnemy(int IDOfEnemy, int IDOfPioneer, int Exp);
};
