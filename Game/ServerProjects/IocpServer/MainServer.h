#pragma once

#include "IocpServerBase.h"

class MainServer : public IocpServerBase
{
public:
	MainServer();
	virtual ~MainServer();

	// 서버 시작
	virtual void StartServer() override;

	// 작업 스레드 생성
	virtual bool CreateWorkerThread() override;

	// 작업 스레드
	virtual void WorkerThread() override;

	// 클라이언트에게 송신
	static void Send(stSOCKETINFO* pSocket);

private:
	FuncProcess				fnProcess[100];	// 패킷 처리 구조체

	static map<int, SOCKET> ClientsSocket;	// <클라이언트 소켓 ID, 소켓> 저장
	static CRITICAL_SECTION csClientsSocket;

	static map<int, stInfoOfGame> Games; // <방장의 소켓 ID, 방 정보> 저장
	static CRITICAL_SECTION csGames;

	// 플레이어가 게임을 실행할 때
	static void AcceptPlayer(stringstream& RecvStream, stSOCKETINFO* pSocket);

	// OnlineWidget에서 CreateWaitingRoom 버튼을 눌러 대기방을 생성할 때
	static void CreateWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);

	// MainScreenWidget에서 Online 버튼을 눌러 게임을 찾을 때
	static void FindGames(stringstream& RecvStream, stSOCKETINFO* pSocket);

	//// 방장이 대기방에서 Title이나 Stage나 MaxOfNum을 변경할 때
	//static void ModifyWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);

	//// 어떤 플레이어가 대기방에 들어올 때
	//static void JoinWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);
	//
	//// 어떤 플레이어가 진행중인 게임에 들어올 때
	//static void JoinPlayingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);

	//// 방장이 대기방을 종료할 때
	//static void DestroyWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);

	//// 방장이 아닌 대기방인 플레이어가 대기방에서 나갈 때
	//static void ExitWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);
	//

	//// 방장이 대기방에서 게임을 시작할 때
	//static void StartWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);

	//// 플레이어가 게임을 종료하면 발생합니다.
	//static void ExitPlayer(stringstream& RecvStream, stSOCKETINFO* pSocket);








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
