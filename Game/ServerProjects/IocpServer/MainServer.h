#pragma once

#include "IocpServerBase.h"

class MainServer : public IocpServerBase
{
private:
	FuncProcess	fnProcess[100];	// 패킷 처리 구조체



	// Login한 클라이언트의 InfoOfPlayer 저장
	static std::map<SOCKET, cInfoOfPlayer> InfoOfClients;
	static CRITICAL_SECTION csInfoOfClients;

	// CreateGame한 클라이언트의 cInfoOfGame 저장
	static std::map<SOCKET, cInfoOfGame> InfoOfGames;
	static CRITICAL_SECTION csInfoOfGames;


public:
	MainServer();
	virtual ~MainServer();

	// 서버 시작
	virtual void StartServer() override;

	// 작업 스레드 생성
	virtual bool CreateWorkerThread() override;

	// 작업 스레드
	virtual void WorkerThread() override;

	// 클라이언트 접속 종료
	virtual void CloseSocket(stSOCKETINFO* pSocketInfo) override;

	// 클라이언트에게 송신
	static void Send(stSOCKETINFO* pSocketInfo);


private:
	/////////////////////////////////////
	// 패킷 처리 함수
	/////////////////////////////////////
	static void Login(stringstream& RecvStream, stSOCKETINFO* pSocketInfo);
	
	static void CreateGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo);

	static void FindGames(stringstream& RecvStream, stSOCKETINFO* pSocketInfo);

	static void JoinWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);




	
	/*


	static void ModifyWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);

	
	static void ExitWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void CheckPlayerInWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);

	*/


	//// 어떤 플레이어가 진행중인 게임에 들어올 때
	//static void JoinPlayingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);

	//// 방장이 대기방을 종료할 때
	//static void DestroyWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);



	//// 방장이 대기방에서 게임을 시작할 때
	//static void StartWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);

	//// 플레이어가 게임을 종료하면 발생합니다.
	//static void ExitPlayer(stringstream& RecvStream, stSOCKETINFO* pSocket);




	// 브로드캐스트 함수
	static void Broadcast(stringstream& SendStream);
	static void BroadcastExcept(stringstream& SendStream, SOCKET Except);



	//// 브로드캐스트 함수
	//static void Broadcast(stringstream& SendStream);
	//
	//// 다른 클라이언트들에게 새 플레이어 입장 정보 보냄
	//static void BroadcastNewPlayer(cCharacter& player);
	//
	//// 캐릭터 정보를 버퍼에 기록
	//static void WriteCharactersInfoToSocket(stSOCKETINFO* pSocket);

	//// 몬스터 정보 초기화
	//void InitializeMonsterSet();
};
