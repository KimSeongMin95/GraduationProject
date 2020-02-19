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

	static void DestroyWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void ExitWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void ModifyWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void StartyWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);



	// 브로드캐스트 함수
	static void Broadcast(stringstream& SendStream);
	static void BroadcastExcept(stringstream& SendStream, SOCKET Except);
};
