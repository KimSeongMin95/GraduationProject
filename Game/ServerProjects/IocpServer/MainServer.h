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
	virtual void CloseSocket(stSOCKETINFO* pSocketInfo) override;

	// Ŭ���̾�Ʈ���� �۽�
	static void Send(stSOCKETINFO* pSocketInfo);


private:
	/////////////////////////////////////
	// ��Ŷ ó�� �Լ�
	/////////////////////////////////////
	static void Login(stringstream& RecvStream, stSOCKETINFO* pSocketInfo);
	
	static void CreateGame(stringstream& RecvStream, stSOCKETINFO* pSocketInfo);

	static void FindGames(stringstream& RecvStream, stSOCKETINFO* pSocketInfo);

	static void JoinWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void DestroyWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void ExitWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void ModifyWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void StartyWaitingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);



	// ��ε�ĳ��Ʈ �Լ�
	static void Broadcast(stringstream& SendStream);
	static void BroadcastExcept(stringstream& SendStream, SOCKET Except);
};
