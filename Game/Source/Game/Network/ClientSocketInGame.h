// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/*** 언리얼엔진 헤더 선언 : Start ***/
#include "DateTime.h"
/*** 언리얼엔진 헤더 선언 : End ***/

#include "Packet.h"

#include "CoreMinimal.h"


/**
 * 게임 서버와 접속 및 패킷 처리를 담당하는 클래스 (게임 클라이언트)
 */
class GAME_API cClientSocketInGame
{
private:
	SOCKET	ServerSocket;			// 서버와 연결할 소켓	

	bool	bAccept;				// 요청 동작 플래그 (메인 스레드)
	CRITICAL_SECTION csAccept;		// 크리티컬 섹션
	HANDLE	hMainHandle;			// 메인 스레드 핸들	

	bool bIsInitialized;
	bool bIsConnected;
	bool bIsClientSocketOn;

	class cClientSocket* ClientSocket = nullptr;

	deque<char*> RecvDeque;

	bool bServerOn;
	CRITICAL_SECTION csServerOn;

	class cInfoOfScoreBoard MyInfoOfScoreBoard;
	CRITICAL_SECTION csMyInfoOfScoreBoard;

	// Ping 시간 측정
	FDateTime StartTime;
	int Ping;
	CRITICAL_SECTION csPing;

	int PossessedID;

protected:
	

public:
	/////////////////////////////////////
	// cClientSocket
	/////////////////////////////////////
	cClientSocketInGame();
	~cClientSocketInGame();

	// 소켓 등록 및 설정
	bool InitSocket();

	// 서버와 연결
	bool Connect(const char * pszIP, int nPort);

	// 소켓 종료
	void CloseSocket();
	
	// 송신
	void Send(stringstream& SendStream);


	///////////////////////////////////////////
	// 소켓 버퍼 크기 변경
	///////////////////////////////////////////
	void SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf);

	///////////////////////////////////////////
	// stringstream의 맨 앞에 size를 추가
	///////////////////////////////////////////
	bool AddSizeInStream(stringstream& DataStream, stringstream& FinalStream);

	///////////////////////////////////////////
	// recvDeque에 수신한 데이터를 적재
	///////////////////////////////////////////
	void PushRecvBufferInDeque(char* RecvBuffer, int RecvLen);

	///////////////////////////////////////////
	// 수신한 데이터를 저장하는 덱에서 데이터를 획득
	///////////////////////////////////////////
	void GetDataInRecvDeque(char* DataBuffer);

	///////////////////////////////////////////
	// 패킷을 처리합니다.
	///////////////////////////////////////////
	void ProcessReceivedPacket(char* DataBuffer);


	// 스레드 시작 및 종료
	bool BeginMainThread();
	void RunMainThread();

	// 싱글턴 객체 가져오기
	static cClientSocketInGame* GetSingleton()
	{
		static cClientSocketInGame ins;
		return &ins;
	}

	bool IsInitialized() { return bIsInitialized; }
	bool IsConnected() { return bIsConnected; }
	bool IsClientSocketOn() { return bIsClientSocketOn; }

	bool IsServerOn();

	/////////////////////////////////////
	// Game Server / Game Clients
	/////////////////////////////////////
	void SendConnected();
	void RecvConnected(stringstream& RecvStream);

	void SendScoreBoard();
	void RecvScoreBoard(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfScoreBoard> tsqScoreBoard;

	void RecvSpaceShip(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfSpaceShip> tsqSpaceShip;

	void SendObservation();

	void RecvSpawnPioneer(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfPioneer> tsqSpawnPioneer;

	void SendDiedPioneer(int ID);
	void RecvDiedPioneer(stringstream& RecvStream);
	cThreadSafetyQueue<int> tsqDiedPioneer; // ID 저장?

	void SendInfoOfPioneer_Animation(class APioneer* PioneerOfPlayer);
	void RecvInfoOfPioneer_Animation(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfPioneer_Animation> tsqInfoOfPioneer_Animation;

	void SendPossessPioneer(cInfoOfPioneer_Socket Socket);
	void RecvPossessPioneer(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfPioneer_Socket> tsqPossessPioneer;

	void RecvInfoOfPioneer_Socket(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfPioneer_Socket> tsqInfoOfPioneer_Socket;

	void SendInfoOfPioneer_Stat(class APioneer* PioneerOfPlayer);
	void RecvInfoOfPioneer_Stat(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfPioneer_Stat> tsqInfoOfPioneer_Stat;

	void SendInfoOfProjectile(cInfoOfProjectile InfoOfProjectile);
	void RecvInfoOfProjectile(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfProjectile> tsqInfoOfProjectile;

	void RecvInfoOfResources(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfResources> tsqInfoOfResources;

	void SendInfoOfBuilding_Spawn(cInfoOfBuilding_Spawn InfoOfBuilding_Spawn);
	void RecvInfoOfBuilding_Spawn(stringstream& RecvStream);
	cThreadSafetyQueue<cInfoOfBuilding_Spawn> tsqInfoOfBuilding_Spawn;

	/////////////////////////////////////
	// Set-Get
	/////////////////////////////////////
	void SetMyInfoOfScoreBoard(cInfoOfScoreBoard& InfoOfScoreBoard);
	cInfoOfScoreBoard CopyMyInfoOfScoreBoard();
	void InitMyInfoOfScoreBoard();




	////////////////////////////////////////////////
	// (임시) 패킷 사이즈와 실제 길이 검증용 함수
	////////////////////////////////////////////////
	void VerifyPacket(char* DataBuffer, bool send)
	{
		if (!DataBuffer)
		{
			printf_s("[ERROR] <cClientSocketInGame::VerifyPacket(...)> if (!DataBuffer) \n");
			return;
		}

		int len = (int)strlen(DataBuffer);

		if (len < 4)
		{
			printf_s("[ERROR] <cClientSocketInGame::VerifyPacket(...)> if (len < 4) \n");
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
		CopyMemory(sizeBuffer, buffer, 4); // 앞 4자리 데이터만 sizeBuffer에 복사합니다.
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
