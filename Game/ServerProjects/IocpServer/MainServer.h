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



	////////////////////////////////////////////////
	// (�ӽ�) ��Ŷ ������� ���� ���� ������ �Լ�
	////////////////////////////////////////////////
	static void VerifyPacket(char* DataBuffer, bool send)
	{
		if (!DataBuffer)
		{
			printf_s("[ERROR] <MainServer::VerifyPacket(...)> if (!DataBuffer) \n");
			return;
		}

		int len = (int)strlen(DataBuffer);

		if (len < 4)
		{
			printf_s("[ERROR] <MainServer::VerifyPacket(...)> if (len < 4) \n");
			return;
		}

		char buffer[MAX_BUFFER + 1];
		CopyMemory(buffer, DataBuffer, len);
		buffer[len] = '\0';

		for (int i = 0; i < len; i++)
		{
			if (buffer[i] == '\n')
				buffer[i] = '_';
		}

		char sizeBuffer[5]; // [1234\0]
		CopyMemory(sizeBuffer, buffer, 4); // �� 4�ڸ� �����͸� sizeBuffer�� �����մϴ�.
		sizeBuffer[4] = '\0';

		stringstream sizeStream;
		sizeStream << sizeBuffer;
		int sizeOfPacket = 0;
		sizeStream >> sizeOfPacket;

		if (sizeOfPacket != len)
		{
			printf_s("\n\n\n\n\n\n\n\n\n\n type: %s \n packet: %s \n sizeOfPacket: %d \n len: %d \n\n\n\n\n\n\n\n\n\n\n", send ? "Send" : "Recv", buffer, sizeOfPacket, len);
		}
	}
};
