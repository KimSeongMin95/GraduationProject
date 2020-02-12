#pragma once

#include "IocpServerBase.h"

class MainServer : public IocpServerBase
{
public:
	MainServer();
	virtual ~MainServer();

	// ���� ����
	virtual void StartServer() override;

	// �۾� ������ ����
	virtual bool CreateWorkerThread() override;

	// �۾� ������
	virtual void WorkerThread() override;

	// Ŭ���̾�Ʈ���� �۽�
	static void Send(stSOCKETINFO* pSocket);

private:
	FuncProcess				fnProcess[100];	// ��Ŷ ó�� ����ü

	static map<SOCKET, stSOCKETINFO*> ClientsSocketInfo; // <Ŭ���̾�Ʈ ���� ID, ����> ����
	static CRITICAL_SECTION csClientsSocket;

	static map<SOCKET, stInfoOfGame> Games; // <������ ���� ID, �� ����> ����
	static CRITICAL_SECTION csGames;

	static void AcceptPlayer(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void CreateWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void FindGames(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void ModifyWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void JoinWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);
	
	static void ExitWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);

	static void CheckPlayerInWaitingRoom(stringstream& RecvStream, stSOCKETINFO* pSocket);




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
