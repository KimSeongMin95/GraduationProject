#include "pch.h"

#include "Client.h"

#include "NetworkComponent.h"


///////////////////////////////////////////
// Call Thread Functions
///////////////////////////////////////////
unsigned int WINAPI CallClientThread(LPVOID p)
{
	CClient* client = (CClient*)p;
	client->ClientThread();

	return 0;
}


///////////////////////////////////////////
// Basic Functions
///////////////////////////////////////////
CClient::CClient()
{
	///////////////////
	// 멤버 변수 초기화
	///////////////////
	ServerSocket = NULL;

	bAccept = false;
	InitializeCriticalSection(&csAccept);
	hMainHandle = NULL;

	InitializeCriticalSection(&csServer);

	NetworkComponent = nullptr;
}
CClient::~CClient()
{
	CloseClient();


	DeleteCriticalSection(&csAccept);

	DeleteCriticalSection(&csServer);
}


///////////////////////////////////////////
// Main Functions
///////////////////////////////////////////
void CClient::CloseServerSocketAndCleanupWSA()
{
	if (ServerSocket != NULL && ServerSocket != INVALID_SOCKET)
	{
		closesocket(ServerSocket);
		ServerSocket = NULL;
	}

	WSACleanup();
}

bool CClient::Initialize(const char* IPv4, USHORT Port)
{
	/// 안정성을 보장하기 위하여, 구동중인 클라이언트를 닫아줍니다.
	CloseClient();

	if (IsClientOn())
	{
		CONSOLE_LOG("[Info] <CClient::Initialize()> if (IsClientOn()) \n");

		return true;
	}
	CONSOLE_LOG("\n\n/********** CClient **********/ \n");
	CONSOLE_LOG("[Start] <CClient::Initialize()> \n");


	WSADATA wsaData;

	// 윈속 버전을 2.2로 초기화
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		CONSOLE_LOG("[Fail]  WSAStartup(...); \n");
		return false;
	}
	CONSOLE_LOG("\t [Success] WSAStartup(...) \n");


	// TCP 소켓 생성	
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET)
	{
		CONSOLE_LOG("[Error] WSASocket(...); \n");
		WSACleanup();
		return false;
	}
	CONSOLE_LOG("\t [Success] WSASocket(...)\n");
	SetSockOpt(ServerSocket, 1048576, 1048576);


	// 접속할 서버 정보를 저장할 구조체
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	if (inet_pton(AF_INET, IPv4, &serverAddr.sin_addr.s_addr) != 1)
	{
		CONSOLE_LOG("[Fail] inet_pton(...) \n");
		CloseServerSocketAndCleanupWSA();
		return false;
	}
	serverAddr.sin_port = htons(Port);

	char bufOfIPv4Addr[32] = { 0, };
	CONSOLE_LOG("\t IPv4: %s \n", inet_ntop(AF_INET, &serverAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr)));
	CONSOLE_LOG("\t Port: %d \n", ntohs(serverAddr.sin_port));


	// 접속 시도
	if (connect(ServerSocket, (sockaddr*)& serverAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		CONSOLE_LOG("[Fail] connect(...) \n");
		CloseServerSocketAndCleanupWSA();
		return false;
	}
	CONSOLE_LOG("\t [Success] connect(...) \n");


	// 클라이언트 스레드 생성
	if (CreateClientThread() == false)
	{
		CONSOLE_LOG("[Fail] CreateClientThread()\n");
		CloseServerSocketAndCleanupWSA();
		return false;
	}
	CONSOLE_LOG("\t [Success] CreateClientThread()\n");


	// 서버의 정보를 저장
	EnterCriticalSection(&csServer);
	Server.socket = ServerSocket;
	Server.IPv4Addr = string(IPv4);
	Server.Port = (int)Port;
	LeaveCriticalSection(&csServer);


	CONSOLE_LOG("[End] <CClient::Initialize()> \n");
	return true;
}

bool CClient::CreateClientThread()
{
	unsigned int threadId;

	// _beginthreadex()는 ::CloseHandle을 내부에서 호출하지 않기 때문에, 스레드 종료시 사용자가 직접 CloseHandle()해줘야 합니다.
	// 스레드가 종료되면 _endthreadex()가 자동호출됩니다.
	hMainHandle = (HANDLE*)_beginthreadex(NULL, 0, &CallClientThread, this, CREATE_SUSPENDED, &threadId);
	if (hMainHandle == NULL)
	{
		CONSOLE_LOG("[Error] <CClient::CreateClientThread()> if (hMainHandle == NULL)\n");
		return false;
	}
	// 클라이언트 구동
	EnterCriticalSection(&csAccept);
	bAccept = true;
	LeaveCriticalSection(&csAccept);

	// 스레드 재개
	ResumeThread(hMainHandle);

	return true;
}

void CClient::ClientThread()
{
	// 수신 버퍼 스트림 (최대 MAX_BUFFER 사이즈의 데이터를 저장하기 때문에, 마지막 '\0'용 사이즈가 필요)
	char recvBuffer[MAX_BUFFER + 1];

	// recv while loop 시작
	// StopTaskCounter 클래스 변수를 사용해 Thread Safety하게 해줌
	while (true)
	{
		// Alertable Wait 상태를 만들기 위해
		SleepEx(1, true);

		// 클라이언트 스레드 종료 확인
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			CONSOLE_LOG("[Info] <CClient::ClientThread()> if (!bAccept) \n");
			CONSOLE_LOG("[Info] <CClient::ClientThread()> Client thread is closed! \n");

			LeaveCriticalSection(&csAccept);
			return;
		}
		LeaveCriticalSection(&csAccept);


		// 수신 바이트 길이
		int nRecvLen = 0;

		/*
		FIONREAD
		네트웍 입력 버퍼에서 기다리고 있는, 소켓 s로부터 읽을 수 있는 데이터의 크기(amount)를 얻어내는데 사용됩니다.
		argp 매개변수는 데이터의 크기를 의미하는 unsigned long 형태로 포인트 합니다.
		다시 말하자면, 만약 s 매개변수가 연결지향형(stream oriented) 소켓(예:SOCK_STREAM) 일 경우,
		FIONREAD 컴맨드에 의한 ioctlsocket 함수의 호출은 recv 함수의 호출로 읽을 수 있는
		데이터의 크기(amount)를 반환하게 되는거죠. 만약 소켓이 메시지 지향형(message oriented) 소켓(예:SOCK_DGRAM) 일 경우
		FIONREAD 컴맨드는 소켓에 큐된 첫 번째 데이터그램의 크기를 반환 합니다.
		*/
		u_long amount = 0;
		if (ioctlsocket(ServerSocket, FIONREAD, &amount) == -1)
		{
			CONSOLE_LOG("[Error] if (ioctlsocket(ServerSocket, FIONREAD, &amount) == -1) \n");
			continue;
		}

		// recv 버퍼에 데이터가 4바이트 미만으로 쌓여있는 상황이면 recv 하지 않습니다.
		if (amount < 4)
		{
			// recv 버퍼에 데이터가 0바이트 존재하면 아직 아무 패킷도 받지 않은것이므로 회문합니다.
			if (amount == 0)
				continue;

			//CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n [Error] amount: %d \n\n\n\n\n\n\n\n\n\n\n", (int)amount);
			continue;
		}

		// 수신
		nRecvLen = recv(ServerSocket, (CHAR*)& recvBuffer, MAX_BUFFER, 0);
		recvBuffer[nRecvLen] = '\0';


		///////////////////////////////////////////
		// recvDeque에 수신한 데이터를 적재
		///////////////////////////////////////////
		if (LoadUpReceivedDataToRecvDeque(recvBuffer, nRecvLen) == false)
		{
			CONSOLE_LOG("[Error] <CClient::ClientThread()> if (LoadUpReceivedDataToRecvDeque(...) == false) \n");
			continue;
		}

		/**************************************************************************/

		char dataBuffer[MAX_BUFFER + 1];
		dataBuffer[0] = '\0'; // GetDataFromRecvDeque(...)를 해도 덱이 비어있는 상태면 오류가 날 수 있으므로 초기화
		dataBuffer[MAX_BUFFER] = '\0';

		///////////////////////////////////////////
		// 수신한 데이터를 저장하는 덱에서 데이터를 획득
		///////////////////////////////////////////
		GetDataFromRecvDeque(dataBuffer);


		///////////////////////////////////////////
		// 덱에서 획득한 데이터를 패킷들로 분할하고 최종적으로 패킷을 처리합니다.
		///////////////////////////////////////////
		DivideDataToPacketAndProcessThePacket(dataBuffer);


		// 게임클라이언트를 종료하면 남아있던 WSASend(...)를 다 보내기 위해 Alertable Wait 상태로 만듭니다.
		SleepEx(1, true);
	}
}

void CClient::CloseClient()
{
	CONSOLE_LOG("[Start] <CClient::CloseClient()>\n");


	// 게임클라이언트를 종료하면 남아있던 WSASend(...)를 다 보내기 위해 Alertable Wait 상태로 만듭니다.
	SleepEx(1, true);


	EnterCriticalSection(&csAccept);
	if (!bAccept)
	{
		CONSOLE_LOG("[Info] <CClient::CloseClient()> if (!bAccept) \n");
		LeaveCriticalSection(&csAccept);
		return;
	}
	bAccept = false;
	LeaveCriticalSection(&csAccept);


	CloseServerSocketAndCleanupWSA();


	// 서버의 정보 초기화
	EnterCriticalSection(&csServer);
	Server = CCompletionKey();
	LeaveCriticalSection(&csServer);


	////////////////////
	// 클라이언트 스레드 종료
	////////////////////
	if (hMainHandle != NULL && hMainHandle != INVALID_HANDLE_VALUE)
	{
		DWORD result = WaitForSingleObject(hMainHandle, INFINITE);

		// hMainHandle이 signal이면
		if (result == WAIT_OBJECT_0)
		{
			CloseHandle(hMainHandle);
			CONSOLE_LOG("\t CloseHandle(hMainHandle);\n");
		}
		else
		{
			CONSOLE_LOG("[Error] WaitForSingleObject(...) failed: %d\n", (int)GetLastError());
		}

		hMainHandle = NULL;
	}


	////////////////////
	// RecvDeque 초기화
	////////////////////
	while (RecvDeque.empty() == false)
	{
		if (RecvDeque.front())
		{
			delete[] RecvDeque.front();
			RecvDeque.front() = nullptr;
		}
		RecvDeque.pop_front();
	}

	/***********************************************/

	//////////////////////
	//// 멤버변수들 초기화
	//////////////////////


	CONSOLE_LOG("[END] <CClient::CloseClient()>\n");
}

void CALLBACK SendCompletionRoutine(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags)
{
	//CONSOLE_LOG("[Start] <CClient::CompletionROUTINE(...)> \n");


	if (dwError != 0)
	{
		CONSOLE_LOG("[Error] <CClient::CompletionROUTINE(...)> Fail to WSASend(...) : %d\n", WSAGetLastError());
	}
	//CONSOLE_LOG("[Info] <CClient::CompletionROUTINE(...)> Success to WSASend(...)\n");

	COverlappedMsg* overlappedMsg = (COverlappedMsg*)lpOverlapped;
	if (overlappedMsg)
	{
		// 전송할 데이터 사이즈와 전송된 데이터 사이즈가 다르면
		if (overlappedMsg->sendBytes != cbTransferred)
		{
			CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
			CONSOLE_LOG("[Error] <CClient::CompletionROUTINE(...)> if (overlappedMsg->sendBytes != cbTransferred) \n");
			CONSOLE_LOG("[Error] <CClient::CompletionROUTINE(...)> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);
			CONSOLE_LOG("[Error] <CClient::CompletionROUTINE(...)> cbTransferred: %d \n", (int)cbTransferred);
			CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
		}
		delete overlappedMsg;
		//CONSOLE_LOG("\t delete overlappedMsg; \n");
	}


	//CONSOLE_LOG("[END] <CClient::CompletionROUTINE(...)> \n");
}

void CClient::Send(stringstream& SendStream)
{
	/////////////////////////////
	// 소켓 유효성 검증
	/////////////////////////////
	if (ServerSocket == NULL || ServerSocket == INVALID_SOCKET)
	{
		CONSOLE_LOG("[Error] <CClient::Send(...)> if (ServerSocket == NULL || ServerSocket == INVALID_SOCKET) \n");
		return;
	}
	//CONSOLE_LOG("[Start] <CClient::Send(...)> \n");


	stringstream finalStream;
	if (AddSizeInStream(SendStream, finalStream) == false)
	{
		CONSOLE_LOG("\n\n\n\n\n [Error] <CClient::Send(...)> if (AddSizeInStream(SendStream, finalStream) == false) \n\n\n\n\n\n");
		return;
	}
	//CONSOLE_LOG("[Info] <CClient::Send(...)> finalStream.str().length(): %d \n", finalStream.str().length());

	DWORD	dwFlags = 0;

	COverlappedMsg* overlappedMsg = new COverlappedMsg();

	overlappedMsg->overlapped.hEvent = &overlappedMsg; // CompletionRoutine 기반이므로 overlapped.hEvent를 활용
	CopyMemory(overlappedMsg->messageBuffer, (CHAR*)finalStream.str().c_str(), finalStream.str().length());
	overlappedMsg->messageBuffer[finalStream.str().length()] = '\0';
	overlappedMsg->dataBuf.len = finalStream.str().length();
	overlappedMsg->dataBuf.buf = overlappedMsg->messageBuffer;
	overlappedMsg->sendBytes = overlappedMsg->dataBuf.len;

	//CONSOLE_LOG("[Info] <CClient::Send(...)> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);


	////////////////////////////////////////////////
	// (디버깅용) 패킷 사이즈와 실제 길이 검증용 함수
	////////////////////////////////////////////////
	VerifyPacket(overlappedMsg->messageBuffer, true);


	int nResult = WSASend(
		ServerSocket, // s: 연결 소켓을 가리키는 소켓 지정 번호
		&(overlappedMsg->dataBuf), // lpBuffers: WSABUF(:4300)구조체 배열의 포인터로 각각의 WSABUF 구조체는 버퍼와 버퍼의 크기를 가리킨다.
		1, // dwBufferCount: lpBuffers에 있는 WSABUF(:4300)구조체의 개수
		NULL, // lpNumberOfBytesSent: 함수의 호출로 전송된 데이터의 바이트 크기를 넘겨준다. 만약 매개 변수 lpOverlapped가 NULL이 아니라면, 이 매개 변수의 값은 NULL로 해야 한다. 그래야 (잠재적인)잘못된 반환을 피할 수 있다.
		dwFlags,// dwFlags: WSASend 함수를 어떤 방식으로 호출 할것인지를 지정한다.
		&(overlappedMsg->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)구조체의 포인터다. 비 (overlapped)중첩 소켓에서는 무시된다.
		SendCompletionRoutine // lpCompletionRoutine: 데이터 전송이 완료 되었을 때 호출할 완료 루틴 (completion routine)의 포인터. 비 중첩 소켓에서는 무시 된다.
	);

	if (nResult == 0)
	{
		//CONSOLE_LOG("[Info] <CClient::Send(...)> Success to WSASend(...) \n");
	}
	if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			CONSOLE_LOG("[Error] <CClient::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());

			delete overlappedMsg;
			overlappedMsg = nullptr;
			CONSOLE_LOG("[Error] <CClient::Send(...)> delete overlappedMsg; \n");

			/// 서버소켓을 닫아도 되는지 아직 확인이 안되었습니다.
			///CloseClient();
		}
		else
		{
			//CONSOLE_LOG("[Info] <CClient::Send(...)> WSASend: WSA_IO_PENDING \n");
		}
	}


	//CONSOLE_LOG("[END] <CClient::Send(...)>\n");
}

void CClient::SendHugeData(stringstream& SendStream)
{
	/*
	참고: Send할 때 sendStream << 데이터 << endl;로 인해 각 데이터는 '\n'로 구분이 됩니다.
	데이터의 집합체인 클래스형 데이터는 operator<<(...) 함수에서 << endl; 대신에 << ' ';를 사용하여 데이터가 계속 이어지고
	최종적으로 '\n'로 구분이 되도록 합니다.

	송신하려는 데이터의 크기가
	[MAX_BUFFER -6(5바이트의 패킷사이즈를 넣을 공간 + 마지막에 '\0'을 넣어줘야 하는 공간)]
	보다 크다면 데이터를 분할하여 전송합니다.

	주의: 대용량 데이터는 동일한 타입형의 데이터들로만 구성되어야 분할해서 전송할 때 수신하는 클라이언트에서 버그가 발생하지 않습니다.
	*/
	//CONSOLE_LOG("[Info] <CClient::SendHugePacket(...)> SendStream.str().length(): %d \n", (int)SendStream.str().length());
	if (SendStream.str().length() <= (MAX_BUFFER - 6))
	{
		// 데이터가 크지 않으므로 바로 송신합니다.
		Send(SendStream);
		return;
	}

	unsigned int packetType = 0;
	SendStream >> packetType;

	if (packetType == 0)
	{
		CONSOLE_LOG("[Error] <CClient::SendHugePacket(...)> if (packetType == 0) \n");
		return;
	}

	/**********************************************************/

	unsigned int sendCount = 0;

	unsigned int idxOfStart = 0;
	unsigned int idxOfEnd = MAX_BUFFER - 6;

	stringstream typeStream;
	typeStream << packetType << endl;
	size_t sizeOfPacketType = 0;

	while (strlen(&SendStream.str().c_str()[idxOfStart]) > (MAX_BUFFER - 6 - sizeOfPacketType))
	{
		for (unsigned int i = (idxOfEnd - 1); i >= idxOfStart; i--)
		{
			// 이런 상황은 올 수 없으므로 발생하면 송신하지 않고 종료합니다.
			if (i == idxOfStart)
			{
				CONSOLE_LOG("[Error] <CClient::SendHugePacket(...)> if (i == idxOfStart) \n");
				return;
			}

			// 데이터 간격을 발견하면
			if (SendStream.str().c_str()[i] == '\n')
			{
				idxOfEnd = i + 1;
				break;
			}
		}

		char dividedBuffer[MAX_BUFFER];
		CopyMemory(dividedBuffer, &SendStream.str().c_str()[idxOfStart], idxOfEnd - idxOfStart);
		dividedBuffer[idxOfEnd - idxOfStart] = '\0';

		// 다시 절절히 초기화
		sizeOfPacketType = typeStream.str().length();
		idxOfStart = idxOfEnd;
		idxOfEnd = idxOfStart + (MAX_BUFFER - 6 - (unsigned int)sizeOfPacketType);

		// 패킷 사이즈와 자른 데이터 사이즈가 같다면 원본: [패킷타입 데이터], 잘린것: [패킷타입 ] 이고
		// 데이터가 쭉 이어진 값이라서 자를 수 없었던 상황이라 이런 경우 전송하지 않고 바로 종료합니다. 
		if (sizeOfPacketType == strlen(dividedBuffer))
		{
			CONSOLE_LOG("[Error] <CClient::SendHugePacket(...)> if (sizeOfPacketType == strlen(dividedBuffer)) \n");
			return;
		}

		stringstream sendStream;
		if (sendCount >= 1)
		{
			sendStream << packetType << endl;
		}
		sendStream << dividedBuffer;

		// 자른 데이터를 송신합니다.
		Send(sendStream);
		sendCount++;
	}

	//////////////////////////////////////////////////
	// 나머지 작아진 패킷을 처리
	//////////////////////////////////////////////////
	char dividedBuffer[MAX_BUFFER];
	size_t sizeOfrest = strlen(&SendStream.str().c_str()[idxOfStart]);
	if (sizeOfrest < MAX_BUFFER)
	{
		CopyMemory(dividedBuffer, &SendStream.str().c_str()[idxOfStart], sizeOfrest);
		dividedBuffer[sizeOfrest] = '\0';

		stringstream sendStream;
		sendStream << packetType << endl;
		sendStream << dividedBuffer;

		// 자른 데이터를 송신합니다.
		Send(sendStream);
	}
}


///////////////////////////////////////////
// Sub Functions
///////////////////////////////////////////
void CClient::SetSockOpt(SOCKET Socket, int SendBuf, int RecvBuf)
{
	/*
	The maximum send buffer size is 1,048,576 bytes.
	The default value of the SO_SNDBUF option is 32,767.
	For a TCP socket, the maximum length that you can specify is 1 GB.
	For a UDP or RAW socket, the maximum length that you can specify is the smaller of the following values:
	65,535 bytes (for a UDP socket) or 32,767 bytes (for a RAW socket).
	The send buffer size defined by the SO_SNDBUF option.
	*/

	/* 검증
	1048576B == 1024KB
	TCP에선 send buffer와 recv buffer 모두 1048576 * 256까지 가능.
	*/
	CONSOLE_LOG("[Start] <CClient::SetSockOpt(...)> \n");


	int optval;
	int optlen = sizeof(optval);

	// 성공시 0, 실패시 -1 반환
	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, &optlen) == 0)
	{
		CONSOLE_LOG("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, &optlen) == 0)
	{
		CONSOLE_LOG("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
	}

	optval = SendBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, sizeof(optval)) == 0)
	{
		CONSOLE_LOG("\t Socket: %d, setsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
	}
	optval = RecvBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, sizeof(optval)) == 0)
	{
		CONSOLE_LOG("\t Socket: %d, setsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
	}

	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)& optval, &optlen) == 0)
	{
		CONSOLE_LOG("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)& optval, &optlen) == 0)
	{
		CONSOLE_LOG("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
	}


	CONSOLE_LOG("[END] <CClient::SetSockOpt(...)> \n");
}

bool CClient::LoadUpReceivedDataToRecvDeque(char* RecvBuffer, int RecvLen)
{
	if (!RecvBuffer)
	{
		CONSOLE_LOG("[Error] <CClient::PushRecvBufferInQueue(...)> if (!RecvBuffer) \n");
		return false;
	}
	/******************************************/

	// 데이터가 MAX_BUFFER 그대로 4096개 꽉 채워서 오는 경우가 있기 때문에, 대비하기 위하여 +1로 '\0' 공간을 만들어줍니다.
	char* newBuffer = new char[MAX_BUFFER + 1];
	//ZeroMemory(newBuffer, MAX_BUFFER);
	CopyMemory(newBuffer, RecvBuffer, RecvLen);
	newBuffer[RecvLen] = '\0';

	RecvDeque.push_back(newBuffer); // 뒤에 순차적으로 적재합니다.

	return true;
}

void CClient::GetDataFromRecvDeque(char* DataBuffer)
{
	if (!DataBuffer)
	{
		CONSOLE_LOG("[Error] <CClient::GetDataFromRecvDeque(...)> if (!DataBuffer) \n");
		return;
	}
	/******************************************/

	int idxOfStartInQueue = 0;
	int idxOfStartInNextQueue = 0;

	// 덱이 빌 때까지 진행 (buffer가 다 차면 반복문을 빠져나옵니다.)
	while (RecvDeque.empty() == false)
	{
		// dataBuffer를 채우려고 하는 사이즈가 최대로 MAX_BUFFER면 CopyMemory 가능.
		if ((idxOfStartInQueue + strlen(RecvDeque.front())) < MAX_BUFFER + 1)
		{
			CopyMemory(&DataBuffer[idxOfStartInQueue], RecvDeque.front(), strlen(RecvDeque.front()));
			idxOfStartInQueue += (int)strlen(RecvDeque.front());
			DataBuffer[idxOfStartInQueue] = '\0';

			delete[] RecvDeque.front();
			RecvDeque.front() = nullptr;
			RecvDeque.pop_front();
		}
		else
		{
			// 버퍼에 남은 자리 만큼 꽉 채웁니다.
			idxOfStartInNextQueue = MAX_BUFFER - idxOfStartInQueue;
			CopyMemory(&DataBuffer[idxOfStartInQueue], RecvDeque.front(), idxOfStartInNextQueue);
			DataBuffer[MAX_BUFFER] = '\0';

			// dateBuffer에 복사하고 남은 데이터들을 임시 버퍼에 복사합니다. 
			int lenOfRestInNextQueue = (int)strlen(&RecvDeque.front()[idxOfStartInNextQueue]);
			char tempBuffer[MAX_BUFFER + 1];
			CopyMemory(tempBuffer, &RecvDeque.front()[idxOfStartInNextQueue], lenOfRestInNextQueue);
			tempBuffer[lenOfRestInNextQueue] = '\0';

			// 임시 버퍼에 있는 데이터들을 다시 RecvDeque.front()에 복사합니다.
			CopyMemory(RecvDeque.front(), tempBuffer, strlen(tempBuffer));
			RecvDeque.front()[strlen(tempBuffer)] = '\0';

			break;
		}
	}
}

void CClient::DivideDataToPacketAndProcessThePacket(char* DataBuffer)
{
	if (!DataBuffer)
	{
		CONSOLE_LOG("[Error] <CClient::DivideDataToPacketAndProcessThePacket(...)> if (!DataBuffer) \n");
		return;
	}
	/******************************************/

	/////////////////////////////////////////////
	// 1. 데이터 버퍼 길이가 0이면
	/////////////////////////////////////////////
	if (strlen(DataBuffer) == 0)
	{
		//CONSOLE_LOG("\t if (strlen(DataBuffer) == 0) \n");
	}
	/////////////////////////////////////////////
	// 2. 데이터 버퍼 길이가 4미만이면
	/////////////////////////////////////////////
	if (strlen(DataBuffer) < 4)
	{
		//CONSOLE_LOG("\t if (strlen(DataBuffer) < 4): %d \n", (int)strlen(DataBuffer));

		// DataBuffer의 남은 데이터를 newBuffer에 복사합니다.
		char* newBuffer = new char[MAX_BUFFER + 1];
		CopyMemory(newBuffer, &DataBuffer, strlen(DataBuffer));
		newBuffer[strlen(DataBuffer)] = '\0';

		// 다시 덱 앞부분에 적재합니다.
		RecvDeque.push_front(newBuffer);
	}
	/////////////////////////////////////////////
	// 3. 데이터 버퍼 길이가 4이상 MAX_BUFFER + 1 미만이면
	/////////////////////////////////////////////
	else if (strlen(DataBuffer) < MAX_BUFFER + 1)
	{
		//CONSOLE_LOG("\t else if (strlen(DataBuffer) < MAX_BUFFER + 1): %d \n", (int)strlen(DataBuffer));

		size_t idxOfStartInPacket = 0;
		size_t lenOfDataBuffer = strlen(DataBuffer);

		while (idxOfStartInPacket < lenOfDataBuffer)
		{
			//CONSOLE_LOG("\t idxOfStartInPacket: %d \n", (int)idxOfStartInPacket);
			//CONSOLE_LOG("\t lenOfDataBuffer: %d \n", (int)lenOfDataBuffer);

			// 버퍼 길이가 4이하면 아직 패킷이 전부 수신되지 않은것이므로
			if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
			{
				//CONSOLE_LOG("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", (int)(lenOfDataBuffer - idxOfStartInPacket));

				// DataBuffer의 남은 데이터를 remainingBuffer에 복사합니다.
				char* newBuffer = new char[MAX_BUFFER + 1];
				CopyMemory(newBuffer, &DataBuffer[idxOfStartInPacket], strlen(&DataBuffer[idxOfStartInPacket]));
				newBuffer[strlen(&DataBuffer[idxOfStartInPacket])] = '\0';

				// 다시 덱 앞부분에 적재합니다.
				RecvDeque.push_front(newBuffer);

				// 반복문을 종료합니다.
				break;
			}

			char sizeBuffer[5]; // [1234\0]
			CopyMemory(sizeBuffer, &DataBuffer[idxOfStartInPacket], 4); // 앞 4자리 데이터만 sizeBuffer에 복사합니다.
			sizeBuffer[4] = '\0';

			stringstream sizeStream;
			sizeStream << sizeBuffer;
			size_t sizeOfPacket = 0;
			sizeStream >> sizeOfPacket;

			//CONSOLE_LOG("\t sizeOfPacket: %d \n", (int)sizeOfPacket);
			//CONSOLE_LOG("\t strlen(&DataBuffer[idxOfStartInPacket]): %d \n", (int)strlen(&DataBuffer[idxOfStartInPacket]));

			// 필요한 데이터 사이즈가 버퍼에 남은 데이터 사이즈보다 크면 아직 패킷이 전부 수신되지 않은것이므로
			if (sizeOfPacket > strlen(&DataBuffer[idxOfStartInPacket]))
			{
				//CONSOLE_LOG("\t if (sizeOfPacket > strlen(&DataBuffer[idxOfStartInPacket])) \n");

				// DataBuffer의 남은 데이터를 remainingBuffer에 복사합니다.
				char* newBuffer = new char[MAX_BUFFER + 1];
				CopyMemory(newBuffer, &DataBuffer[idxOfStartInPacket], strlen(&DataBuffer[idxOfStartInPacket]));
				newBuffer[strlen(&DataBuffer[idxOfStartInPacket])] = '\0';

				// 다시 덱 앞부분에 적재합니다.
				RecvDeque.push_front(newBuffer);

				// 반복문을 종료합니다.
				break;;
			}

			/// 오류 확인
			if (sizeOfPacket == 0)
			{
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
				CONSOLE_LOG("[Error] <MainServer::WorkerThread()> sizeOfPacket: %d \n", (int)sizeOfPacket);
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
				break;
			}

			// 패킷을 자르면서 임시 버퍼에 복사합니다.
			char cutBuffer[MAX_BUFFER + 1];
			CopyMemory(cutBuffer, &DataBuffer[idxOfStartInPacket], sizeOfPacket);
			cutBuffer[sizeOfPacket] = '\0';


			///////////////////////////////////////////
			// (디버깅용) 패킷 사이즈와 실제 길이 검증용 함수
			///////////////////////////////////////////
			VerifyPacket(cutBuffer, false);


			///////////////////////////////////////////
			// 패킷을 처리합니다.
			///////////////////////////////////////////
			ProcessThePacket(cutBuffer);


			idxOfStartInPacket += sizeOfPacket;
		}
	}
}

void CClient::ProcessThePacket(char* DataBuffer)
{
	if (!DataBuffer)
	{
		CONSOLE_LOG("[Error] <CClient::ProcessThePacket(...)> if (!DataBuffer) \n");
		return;
	}
	/******************************************/

	stringstream recvStream;
	recvStream << DataBuffer;

	// 사이즈 확인
	int sizeOfRecvStream = 0;
	recvStream >> sizeOfRecvStream;
	//CONSOLE_LOG("\t sizeOfRecvStream: %d \n", sizeOfRecvStream);

	// 패킷 종류 확인
	int packetType = -1;
	recvStream >> packetType;
	//CONSOLE_LOG("\t packetType: %d \n", packetType);


	if (NetworkComponent)
		NetworkComponent->ProcessPacket(packetType, NetworkComponent, recvStream, NULL);
	else
		CONSOLE_LOG("[Error] <CClient::ProcessThePacket(...)> if (!NetworkComponent) \n");
}

bool CClient::AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
{
	if (DataStream.str().length() == 0)
	{
		CONSOLE_LOG("[Error] <CClient::AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
		return false;
	}
	/******************************************/
	//CONSOLE_LOG("[Start] <CClient::AddSizeInStream(...)> \n");

	//// ex) DateStream의 크기 : 98
	//CONSOLE_LOG("\t DataStream size: %d\n", (int)DataStream.str().length());
	//CONSOLE_LOG("\t DataStream: %s\n", DataStream.str().c_str());


	// dataStreamLength의 크기 : 3 [98 ]
	stringstream dataStreamLength;
	dataStreamLength << DataStream.str().length() << endl;

	// lengthOfFinalStream의 크기 : 4 [101 ]
	stringstream lengthOfFinalStream;
	lengthOfFinalStream << (dataStreamLength.str().length() + DataStream.str().length()) << endl;

	// FinalStream의 크기 : 102 [101 DataStream]
	int sizeOfFinalStream = (int)(lengthOfFinalStream.str().length() + DataStream.str().length());
	FinalStream << sizeOfFinalStream << endl;
	FinalStream << DataStream.str(); // 이미 DataStream.str() 마지막에 endl;를 사용했으므로 여기선 다시 사용하지 않습니다.

	//CONSOLE_LOG("\t FinalStream size: %d\n", (int)FinalStream.str().length());
	//CONSOLE_LOG("\t FinalStream: %s\n", FinalStream.str().c_str());
	

	// 전송할 데이터가 최대 버퍼 크기보다 크거나 같으면 전송 불가능을 알립니다.
	// messageBuffer[MAX_BUFFER];에서 마지막에 '\0'을 넣어줘야 되기 때문에 MAX_BUFFER와 같을때도 무시합니다.
	if (FinalStream.str().length() >= MAX_BUFFER)
	{
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
		CONSOLE_LOG("[Error] <CClient::AddSizeInStream(...)> if (FinalStream.str().length() > MAX_BUFFER \n");
		CONSOLE_LOG("[Error] <CClient::AddSizeInStream(...)> FinalStream.str().length(): %d \n", (int)FinalStream.str().length());
		CONSOLE_LOG("[Error] <CClient::AddSizeInStream(...)> FinalStream.str().c_str(): %s \n", FinalStream.str().c_str());
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
		return false;
	}


	//CONSOLE_LOG("[END] <CClient::AddSizeInStream(...)> \n");
	return true;
}

void CClient::VerifyPacket(char* DataBuffer, bool bSend)
{
	if (!DataBuffer)
	{
		CONSOLE_LOG("[Error] <CClient::VerifyPacket(...)> if (!DataBuffer) \n");
		return;
	}
	int len = (int)strlen(DataBuffer);
	if (len < 4)
	{
		CONSOLE_LOG("[Error] <CClient::VerifyPacket(...)> if (len < 4) \n");
		return;
	}
	/**************************************************/

#if TEMP_BUILD_CONFIG_DEBUG
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
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
		CONSOLE_LOG("[ERROR] <CClient::VerifyPacket(...)> type: %s \n packet: %s \n sizeOfPacket: %d \n len: %d \n", bSend ? "Send" : "Recv", buffer, sizeOfPacket, len);
		CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
	}
#endif
}


///////////////////////////////////////////
// NetworkComponent
///////////////////////////////////////////
CClient* CClient::GetSingleton()
{
	static CClient client;
	return &client;
}

void CClient::SetNetworkComponent(class CNetworkComponent* NC)
{
	NetworkComponent = NC;
}

CCompletionKey CClient::GetCompletionKey()
{
	CCompletionKey completionKey;

	EnterCriticalSection(&csServer);
	completionKey = Server;
	LeaveCriticalSection(&csServer);

	return completionKey;
}

bool CClient::IsClientOn()
{
	EnterCriticalSection(&csAccept);
	bool bClientOn = bAccept;
	LeaveCriticalSection(&csAccept);

	return bClientOn;
}