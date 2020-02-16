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




	
	/*


	static void ModifyWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);

	
	static void ExitWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void CheckPlayerInWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);

	*/


	//// � �÷��̾ �������� ���ӿ� ���� ��
	//static void JoinPlayingGame(stringstream& RecvStream, stSOCKETINFO* pSocket);

	//// ������ ������ ������ ��
	//static void DestroyWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);



	//// ������ ���濡�� ������ ������ ��
	//static void StartWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);

	//// �÷��̾ ������ �����ϸ� �߻��մϴ�.
	//static void ExitPlayer(stringstream& RecvStream, stSOCKETINFO* pSocket);




	// ��ε�ĳ��Ʈ �Լ�
	static void Broadcast(stringstream& SendStream);
	static void BroadcastExcept(stringstream& SendStream, SOCKET Except);



	//// ��ε�ĳ��Ʈ �Լ�
	//static void Broadcast(stringstream& SendStream);
	//
	//// �ٸ� Ŭ���̾�Ʈ�鿡�� �� �÷��̾� ���� ���� ����
	//static void BroadcastNewPlayer(cCharacter& player);
	//
	//// ĳ���� ������ ���ۿ� ���
	//static void WriteCharactersInfoToSocket(stSOCKETINFO* pSocket);

	//// ���� ���� �ʱ�ȭ
	//void InitializeMonsterSet();
};
