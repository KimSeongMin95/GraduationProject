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
	static void CloseSocket(SOCKET Socket);

	// 클라이언트에게 송신
	static void Send(stringstream& SendStream, SOCKET Socket);
	
	// 클라이언트 수신 대기
	static void Recv(SOCKET Socket);

	///////////////////////////////////////////
	// 수신한 데이터를 저장하는 덱에서 데이터를 획득
	///////////////////////////////////////////
	void GetDataInRecvDeque(deque<char*>* RecvDeque, char* DataBuffer);

	///////////////////////////////////////////
	// 패킷을 처리합니다.
	///////////////////////////////////////////
	void ProcessReceivedPacket(char* DataBuffer, SOCKET Socket);



private:
	///////////////////////////////////////////
	// Main Server / Main Clients
	///////////////////////////////////////////
	static void Broadcast(stringstream& SendStream);
	static void BroadcastExcept(stringstream& SendStream, SOCKET Except);

	static void Login(stringstream& RecvStream, SOCKET Socket);
	
	static void CreateGame(stringstream& RecvStream, SOCKET Socket);

	static void FindGames(stringstream& RecvStream, SOCKET Socket);

	static void JoinOnlineGame(stringstream& RecvStream, SOCKET Socket);

	static void DestroyWaitingGame(stringstream& RecvStream, SOCKET Socket);

	static void ExitWaitingGame(stringstream& RecvStream, SOCKET Socket);

	static void ModifyWaitingGame(stringstream& RecvStream, SOCKET Socket);

	static void StartWaitingGame(stringstream& RecvStream, SOCKET Socket);

	///////////////////////////////////////////
	// Game Server / Game Clients
	///////////////////////////////////////////
	static void ActivateGameServer(stringstream& RecvStream, SOCKET Socket);

	static void RequestInfoOfGameServer(stringstream& RecvStream, SOCKET Socket);
};
