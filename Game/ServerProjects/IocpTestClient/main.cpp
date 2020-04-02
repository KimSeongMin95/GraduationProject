// 멀티바이트 집합 사용시 define
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// winsock2 사용을 위해 아래 코멘트 추가
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <iostream>
#include <sstream>
#include <map>


enum EPacketType
{
	ACCEPT_PLAYER,		  // 플레이어가 게임을 실행할 때: 해당 플레이어에게 너의 소켓 ID가 이거라고 답장해야 함.
	CREATE_WAITING_ROOM,  // OnlineWidget에서 CreateWaitingRoom 버튼을 눌러 대기방을 생성할 때: FIND_GAMES인 플레이어들에게 브로드캐스팅 해야 함.
	FIND_GAMES,			  // MainScreenWidget에서 Online 버튼을 눌러 게임을 찾을 때: 해당 플레이어에게 모든 게임들을 전송
	MODIFY_WAITING_ROOM,  // 방장이 대기방에서 Title이나 Stage나 MaxOfNum을 변경할 때: FIND_GAMES와 방장을 제외한 대기방인 플레이어들에게 브로드캐스팅 해야 함.
	JOIN_WAITING_ROOM,	  // 어떤 플레이어가 대기방에 들어올 때: FIND_GAMES와 대기방인 플레이어들에게 브로드캐스팅 해야 함.
	JOIN_PLAYING_GAME,	  // 어떤 플레이어가 진행중인 게임에 들어올 때: 해당 게임의 플레이어들과 FIND_GAMES인 플레이어들에게 브로드캐스팅 해야 함.
	DESTROY_WAITING_ROOM, // 방장이 대기방을 종료할 때: FIND_GAMES와 방장을 제외한 대기방인 플레이어들에게 브로드캐스팅 해야 함.
	EXIT_WAITING_ROOM,	  // 방장이 아닌 대기방인 플레이어가 대기방에서 나갈 때: FIND_GAMES와 대기방인 플레이어들에게 브로드캐스팅 해야 함.

	START_WAITING_ROOM,   // 방장이 대기방에서 게임을 시작할 때: 방장을 제외한 대기방인 플레이어들에게 브로드캐스팅 해야 함.
	EXIT_PLAYER,		  // 플레이어가 게임을 종료할 때: 
};


using namespace std;

#define	MAX_BUFFER		4096
#define SERVER_PORT		8000
#define SERVER_IP		"58.234.38.97"
#define MAX_CLIENTS		1000

struct stSOCKETINFO
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;
	char			messageBuffer[MAX_BUFFER];
	int				recvBytes;
	int				sendBytes;
};

bool AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{
		printf_s("[ERROR] <AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
		return false;
	}
	//printf_s("[START] <AddSizeInStream(...)> \n");

	// ex) DateStream의 크기 : 98
	//printf_s("\t DataStream size: %d\n", (int)DataStream.str().length());
	//printf_s("\t DataStream: %s\n", DataStream.str().c_str());

	// dataStreamLength의 크기 : 3 [98 ]
	stringstream dataStreamLength;
	dataStreamLength << DataStream.str().length() << endl;

	// lengthOfFinalStream의 크기 : 4 [101 ]
	stringstream lengthOfFinalStream;
	lengthOfFinalStream << (dataStreamLength.str().length() + DataStream.str().length()) << endl;

	// FinalStream의 크기 : 101 [101 DataStream]
	int sizeOfFinalStream = (int)(lengthOfFinalStream.str().length() + DataStream.str().length());
	FinalStream << sizeOfFinalStream << endl;
	FinalStream << DataStream.str(); // 이미 DataStream.str() 마지막에 endl;를 사용했으므로 여기선 다시 사용하지 않습니다.

	//printf_s("\t FinalStream size: %d\n", (int)FinalStream.str().length());
	//printf_s("\t FinalStream: %s\n", FinalStream.str().c_str());


	// 전송할 데이터가 최대 버퍼 크기보다 크거나 같으면 전송 불가능을 알립니다.
	// messageBuffer[MAX_BUFFER];에서 마지막에 '\0'을 넣어줘야 되기 때문에 MAX_BUFFER와 같을때도 무시합니다.
	if (FinalStream.str().length() >= MAX_BUFFER)
	{
		printf_s("\n\n\n\n\n\n\n\n\n\n");
		printf_s("[ERROR] <AddSizeInStream(...)> if (FinalStream.str().length() > MAX_BUFFER \n");
		printf_s("[ERROR] <AddSizeInStream(...)> FinalStream.str().length(): %d \n", (int)FinalStream.str().length());
		printf_s("[ERROR] <AddSizeInStream(...)> FinalStream.str().c_str(): %s \n", FinalStream.str().c_str());
		printf_s("\n\n\n\n\n\n\n\n\n\n");
		return false;
	}


	//printf_s("[END] <AddSizeInStream(...)> \n");

	return true;
}


int main()
{
	WSADATA wsaData;
	// 윈속 버전을 2.2로 초기화
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (nRet != 0) {
		std::cout << "Error : " << WSAGetLastError() << std::endl;
		return false;
	}

	// TCP 소켓 생성
	SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (clientSocket == INVALID_SOCKET) {
		std::cout << "Error : " << WSAGetLastError() << std::endl;
		return false;
	}

	std::cout << "socket initialize success." << std::endl;

	// 접속할 서버 정보를 저장할 구조체
	SOCKADDR_IN stServerAddr;

	char	sz_socketbuf_[MAX_BUFFER + 1];
	stServerAddr.sin_family = AF_INET;
	// 접속할 서버 포트 및 IP
	stServerAddr.sin_port = htons(SERVER_PORT);
	stServerAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	nRet = connect(clientSocket, (sockaddr*)& stServerAddr, sizeof(sockaddr));
	if (nRet == SOCKET_ERROR) {
		std::cout << "Error : " << WSAGetLastError() << std::endl;
		return false;
	}

	std::cout << "Connection success..." << std::endl;

	int nSend, nRecv;

	while (true)
	{
		Sleep(1000);

		stringstream data;
		data << EPacketType::FIND_GAMES << endl;
		stringstream sendStream;
		AddSizeInStream(data, sendStream);
		nSend = send(clientSocket, (CHAR*)sendStream.str().c_str(), sendStream.str().length(), 0);

		nRecv = recv(clientSocket, sz_socketbuf_, MAX_BUFFER, 0);
		sz_socketbuf_[nRecv] = '\0';
		printf_s("recv: %s\n", sz_socketbuf_);
	}

	closesocket(clientSocket);
	WSACleanup();
	std::cout << "Client has been terminated..." << std::endl;

	return 0;
}

