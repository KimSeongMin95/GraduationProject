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
	ACCEPT_PLAYER, // 플레이어가 게임을 실행할 때
		// 해당 플레이어에게 너의 소켓 ID가 이거라고 답장해야 함.
	FIND_GAMES,		// MainScreenWidget에서 Online 버튼을 눌러 게임을 찾을 때
	CREATE_WAITING_ROOM, // OnlineWidget에서 CreateWaitingRoom 버튼을 눌러 대기방을 생성할 때
		// FIND_GAMES인 플레이어들에게 브로드캐스팅 해야 함.
	MODIFY_WAITING_ROOM, // 방장이 대기방에서 Title이나 Stage나 MaxOfNum을 변경할 때
		// FIND_GAMES와 방장을 제외한 대기방인 플레이어들에게 브로드캐스팅 해야 함.
	JOIN_WAITING_ROOM, // 어떤 플레이어가 대기방에 들어올 때
		// FIND_GAMES와 대기방인 플레이어들에게 브로드캐스팅 해야 함.
	JOIN_PLAYING_GAME, // 어떤 플레이어가 진행중인 게임에 들어올 때
		// 해당 게임의 플레이어들과 FIND_GAMES인 플레이어들에게 브로드캐스팅 해야 함.
	DESTROY_WAITING_ROOM, // 방장이 대기방을 종료할 때
		// FIND_GAMES와 방장을 제외한 대기방인 플레이어들에게 브로드캐스팅 해야 함.
	EXIT_WAITING_ROOM, // 방장이 아닌 대기방인 플레이어가 대기방에서 나갈 때
		// FIND_GAMES와 대기방인 플레이어들에게 브로드캐스팅 해야 함.

	START_WAITING_ROOM, // 방장이 대기방에서 게임을 시작할 때
		// 방장을 제외한 대기방인 플레이어들에게 브로드캐스팅 해야 함.
	EXIT_PLAYER, // 플레이어가 게임을 종료하면 발생합니다.
};

using namespace std;

#define	MAX_BUFFER		4096
#define SERVER_PORT		8000
#define SERVER_IP		"127.0.0.1"
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
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		std::cout << "Error : " << WSAGetLastError() << std::endl;
		return false;
	}

	std::cout << "socket initialize success." << std::endl;

	// 접속할 서버 정보를 저장할 구조체
	SOCKADDR_IN stServerAddr;

	char	szOutMsg[MAX_BUFFER];
	char	sz_socketbuf_[MAX_BUFFER];
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

	stringstream SendStream;
	SendStream << EPacketType::ACCEPT_PLAYER << endl;


	int nSend = send(clientSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0);

	int nRecv = recv(clientSocket, sz_socketbuf_, MAX_BUFFER, 0);

	stringstream RecvStream;
	bool result;
	RecvStream << sz_socketbuf_;
	RecvStream >> result;
	printf_s("%d\n", result);


	while (true) {}


	closesocket(clientSocket);
	WSACleanup();
	std::cout << "Client has been terminated..." << std::endl;

	return 0;
}

