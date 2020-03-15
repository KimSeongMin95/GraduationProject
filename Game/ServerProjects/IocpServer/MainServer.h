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
	static void CloseSocket(stSOCKETINFO* pSocketInfo);

	// Ŭ���̾�Ʈ���� �۽�
	static void Send(stringstream& SendStream, stSOCKETINFO* pSocketInfo);
	
	// Ŭ���̾�Ʈ ���� ���
	static void Recv(stSOCKETINFO* pSocketInfo);

	///////////////////////////////////////////
	// ��Ŷ�� ó���մϴ�.
	///////////////////////////////////////////
	void ProcessReceivedPacket(char* DataBuffer, stSOCKETINFO* pSocketInfo);

	///////////////////////////////////////////
	// ������ �����͸� �����ϴ� ť���� �����͸� ȹ��
	///////////////////////////////////////////
	void GetDataInRecvQueue(queue<char*>* RecvQueue, char* DataBuffer);

private:
	///////////////////////////////////////////
	// Main Server / Main Clients
	///////////////////////////////////////////
	static void Login(stringstream& RecvStream, stSOCKETINFO* pSocketInfo);
	
	static void CreateGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo);

	static void FindGames(stringstream& RecvStream, stSOCKETINFO* pSocketInfo);

	static void JoinOnlineGame(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void DestroyWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void ExitWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void ModifyWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void StartWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);

	///////////////////////////////////////////
	// Game Server / Game Clients
	///////////////////////////////////////////
	static void ActivateGameServer(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void RequestInfoOfGameServer(stringstream& RecvStream, stSOCKETINFO* pSocket);



	// ��ε�ĳ��Ʈ �Լ�
	static void Broadcast(stringstream& SendStream);
	static void BroadcastExcept(stringstream& SendStream, SOCKET Except);
};
