#pragma once

#include "IocpServerBase.h"

class MainServer : public IocpServerBase
{
private:
	FuncProcess	fnProcess[100];	// ��Ŷ ó�� ����ü


	// Login�� Ŭ���̾�Ʈ�� InfoOfPlayer ����
	static std::map<SOCKET, cInfoOfPlayer> InfoOfClients;
	static CRITICAL_SECTION csInfoOfClients;

	// CreateGame�� Ŭ���̾�Ʈ�� cInfoOfGame ����
	static std::map<SOCKET, cInfoOfGame> InfoOfGames;
	static CRITICAL_SECTION csInfoOfGames;

public:
	MainServer();
	virtual ~MainServer();

	// ���� ����
	virtual void StartServer() override;

	// �۾� ������ ����
	virtual bool CreateWorkerThread() override;

	// �۾� ������
	virtual void WorkerThread() override;

	// Ŭ���̾�Ʈ ���� ����
	static void CloseSocket(SOCKET Socket);

	// Ŭ���̾�Ʈ���� �۽�
	static void Send(stringstream& SendStream, SOCKET Socket);
	
	// Ŭ���̾�Ʈ ���� ���
	static void Recv(SOCKET Socket);

	///////////////////////////////////////////
	// ������ �����͸� �����ϴ� ������ �����͸� ȹ��
	///////////////////////////////////////////
	void GetDataInRecvDeque(deque<char*>* RecvDeque, char* DataBuffer);

	///////////////////////////////////////////
	// ��Ŷ�� ó���մϴ�.
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
