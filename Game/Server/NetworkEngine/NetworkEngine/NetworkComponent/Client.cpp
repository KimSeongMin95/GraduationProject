
#include "ExceptionHandler.h"
#include "Console.h"

#include "Client.h"

unsigned int WINAPI CallRunClientThread(LPVOID p)
{
	CClient* client = (CClient*)p;
	client->RunClientThread();

	return 0;
}


CClient::CClient()
{
	ServerSocket = NULL;

	bAccept = false;
	while (InitializeCriticalSectionAndSpinCount(&csAccept, SPIN_COUNT) == false);

	hClientThreadHandle = NULL;

	// 크리티컬 섹션에 스핀락을 걸고 초기화에 성공할때까지 시도합니다.
	while (InitializeCriticalSectionAndSpinCount(&csServer, SPIN_COUNT) == false);
}
CClient::~CClient()
{
	Close();

	DeleteCriticalSection(&csAccept);
	DeleteCriticalSection(&csServer);
}

bool CClient::Initialize(const char* const IPv4, const USHORT& Port)
{
	// 이미 클라이언트가 구동중이라면 먼저 구동을 종료합니다.
	if (IsNetworkOn())
	{
		CONSOLE_LOG_NETWORK("[Info] <CClient::Initialize()> if (IsNetworkOn()) \n");
		Close();
	}
	CONSOLE_LOG_NETWORK("\n\n/********** CClient **********/ \n");
	CONSOLE_LOG_NETWORK("[Start] <CClient::Initialize()> \n");

	WSADATA wsaData;

	// winsock 라이브러리를 2.2 버전으로 초기화합니다.
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		CONSOLE_LOG_NETWORK("[Fail]  WSAStartup(...); \n");
		return false;
	}
	CONSOLE_LOG_NETWORK("\t [Success] WSAStartup(...) \n");

	// TCP 소켓을 생성합니다.
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET)
	{
		CONSOLE_LOG_NETWORK("[Error] WSASocket(...); \n");
		WSACleanup();
		return false;
	}
	CONSOLE_LOG_NETWORK("\t [Success] WSASocket(...)\n");

	SetSockOpt(ServerSocket, 1048576, 1048576);

	// 서버의 주소 정보를 설정합니다.
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	if (inet_pton(AF_INET, IPv4, &serverAddr.sin_addr.s_addr) != 1)
	{
		CONSOLE_LOG_NETWORK("[Fail] inet_pton(...) \n");
		CloseSocketAndWSACleanup(ServerSocket);
		return false;
	}
	serverAddr.sin_port = htons(Port);

	// 서버의 주소 정보를 출력합니다.
	char bufOfIPv4Addr[32] = { 0, };
	CONSOLE_LOG_NETWORK("\t IPv4: %s \n", inet_ntop(AF_INET, &serverAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr)));
	CONSOLE_LOG_NETWORK("\t Port: %d \n", ntohs(serverAddr.sin_port));

	// 서버에 접속을 시도합니다.
	if (connect(ServerSocket, (sockaddr*)&serverAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		CONSOLE_LOG_NETWORK("[Fail] connect(...) \n");
		CloseSocketAndWSACleanup(ServerSocket);
		return false;
	}
	CONSOLE_LOG_NETWORK("\t [Success] connect(...) \n");

	// 클라이언트 스레드를 생성합니다.
	if (CreateClientThread() == false)
	{
		CONSOLE_LOG_NETWORK("[Fail] CreateClientThread()\n");
		CloseSocketAndWSACleanup(ServerSocket);
		return false;
	}
	CONSOLE_LOG_NETWORK("\t [Success] CreateClientThread()\n");

	// 이제 클라이언트 구동을 승인합니다.
	EnterCriticalSection(&csAccept);
	bAccept = true;
	LeaveCriticalSection(&csAccept);

	// 미리 생성한 스레드를 재개합니다.
	ResumeThread(hClientThreadHandle);

	// 서버의 정보를 저장
	EnterCriticalSection(&csServer);
	Server.Socket = ServerSocket;
	Server.IPv4Addr = string(IPv4);
	Server.Port = (unsigned int)Port;
	ConCBF.ExecuteFunc(Server); // 서버에 접속하면 실행할 콜백함수 실행합니다.
	LeaveCriticalSection(&csServer);

	CONSOLE_LOG_NETWORK("[End] <CClient::Initialize()> \n");
	return true;
}

bool CClient::CreateClientThread()
{
	unsigned int threadId;

	// _beginthreadex()는 ::CloseHandle을 내부에서 호출하지 않기 때문에, 스레드 종료시 사용자가 직접 CloseHandle()해주어야 합니다. 스레드가 종료되면 _endthreadex()가 자동으로 호출됩니다.
	hClientThreadHandle = (HANDLE*)_beginthreadex(NULL, 0, &CallRunClientThread, this, CREATE_SUSPENDED, &threadId);
	if (hClientThreadHandle == NULL || hClientThreadHandle == INVALID_HANDLE_VALUE)
	{
		CONSOLE_LOG_NETWORK("[Error] <CClient::CreateClientThread()> if (hClientThreadHandle == NULL || hClientThreadHandle == INVALID_HANDLE_VALUE)\n");
		return false;
	}

	return true;
}

void CClient::RunClientThread()
{
	int nRecvLen = 0; // 수신한 바이트 크기를 저장합니다.
	char recvBuffer[MAX_BUFFER + 1]; // 수신할 데이터를 저장할 버퍼입니다.
	char bufOfPackets[2 * MAX_BUFFER + 1]; // RecvDeque으로부터 획득하는 패킷들을 저장합니다.

	// 클라이언트를 정상적으로 구동합니다.
	while (true)
	{
		nRecvLen = 0;

		// SendCompletionRoutine을 위해 잠시 Alertable Wait 상태로 만듭니다.
		SleepEx(1, true);

		// 클라이언트 스레드 종료 확인
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			LeaveCriticalSection(&csAccept);
			CONSOLE_LOG_NETWORK("[Info] <CClient::ClientThread()> if (!bAccept) \n");
			CONSOLE_LOG_NETWORK("[Info] <CClient::ClientThread()> Client thread is closed! \n");
			return;
		}
		LeaveCriticalSection(&csAccept);

		///* FIONREAD 설명
		//FIONREAD는 네트워크 입력 버퍼에서 기다리고 있는 소켓 s로부터 읽을 수 있는 데이터의 크기(amount)를 얻어내는데 사용됩니다.
		//ioctlsocket(...) 함수의 argp 매개변수로 읽을 수 있는 데이터의 크기를 획득할 수 있습니다.
		//연결 지향형(stream oriented) 소켓(예:SOCK_STREAM)일 경우, recv 함수의 호출로 읽을 수 있는 데이터의 크기(amount)를 획득할 수 있습니다.
		//만약 소켓이 메시지 지향형(message oriented) 소켓(예:SOCK_DGRAM)일 경우, 소켓에 큐된 첫 번째 데이터그램의 크기를 반환 합니다.
		//*/
		//u_long amount = 0;
		//if (ioctlsocket(ServerSocket, FIONREAD, &amount) == -1)
		//{
		//	CONSOLE_LOG_NETWORK("[Error] if (ioctlsocket(ServerSocket, FIONREAD, &amount) == -1) \n");
		//	continue;
		//}

		// 수신
		nRecvLen = recv(ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0);
		if (nRecvLen == SOCKET_ERROR || nRecvLen == 0)
		{
			// 서버와 접속이 종료되면 실행할 콜백함수를 실행합니다.
			DisconCBF.ExecuteFunc(GetCompletionKey());
			continue;
		}
		recvBuffer[nRecvLen] = '\0';

		// RecvDeque이 비어있고 수신한 데이터가 온전한 패킷이라면 RecvDeque을 거치지 않고 바로 처리하여 성능을 향상시킵니다.
		if (RecvDeque.empty() == true && recvBuffer[nRecvLen - 1] == (char)3)
		{
			// 수신한 패킷들을 패킷으로 분할하고 최종적으로 패킷을 처리합니다.
			DividePacketsAndProcessThePacket(recvBuffer);
		}
		else // 그렇지 않다면 다음과 같은 처리를 진행합니다.
		{
			// RecvDeque의 뒷부분에 수신한 데이터를 적재합니다.
			LoadUpReceivedDataToRecvDeque(recvBuffer, nRecvLen);

			// RecvDeque으로부터 패킷들을 획득합니다.
			GetPacketsFromRecvDeque(bufOfPackets);

			// 획득한 패킷들을 패킷으로 분할하고 최종적으로 패킷을 처리합니다.
			DividePacketsAndProcessThePacket(bufOfPackets);
		}


		// 남아있던 WSASend(...)를 다 보내기 위해 Alertable Wait 상태로 만듭니다.
		SleepEx(1, true);
	}
}

bool CClient::IsNetworkOn()
{
	EnterCriticalSection(&csAccept);
	bool bClientOn = bAccept;
	LeaveCriticalSection(&csAccept);

	return bClientOn;
}

CCompletionKey CClient::GetCompletionKey(const SOCKET& Socket /*= NULL*/)
{
	EnterCriticalSection(&csServer);
	CCompletionKey completionKey = Server;
	LeaveCriticalSection(&csServer);

	return completionKey;
}

void CClient::Close()
{
	CONSOLE_LOG_NETWORK("[Start] <CClient::Close()>\n");

	// 클라이언트를 종료하면 남아있던 WSASend(...)를 다 보내기 위해 Alertable Wait 상태로 만듭니다.
	SleepEx(1, true);

	// 서버가 구동중일때만 종료하도록 합니다.
	EnterCriticalSection(&csAccept);
	if (!bAccept)
	{
		LeaveCriticalSection(&csAccept);
		CONSOLE_LOG_NETWORK("[Info] <CClient::Close()> if (!bAccept) \n");
		return;
	}
	bAccept = false;
	LeaveCriticalSection(&csAccept);
	/****************************************/

	// 서버와 접속이 종료되면 실행할 콜백함수를 실행합니다.
	DisconCBF.ExecuteFunc(GetCompletionKey());

	// 서버의 정보를 초기화합니다.
	EnterCriticalSection(&csServer);
	Server = CCompletionKey();
	LeaveCriticalSection(&csServer);

	// 먼저 더이상 통신하지 못하도록 ServerSocket을 닫습니다.
	CloseSocketWithCheck(ServerSocket);

	// 그 다음, 클라이언트 스레드의 종료를 확인합니다.
	if (hClientThreadHandle != NULL && hClientThreadHandle != INVALID_HANDLE_VALUE)
	{
		DWORD result = WaitForSingleObject(hClientThreadHandle, INFINITE);

		if (result == WAIT_OBJECT_0) // hClientThreadHandle이 signal이면
		{
			CloseHandle(hClientThreadHandle);
			CONSOLE_LOG_NETWORK("\t CloseHandle(hClientThreadHandle);\n");
		}
		else if (result == WAIT_TIMEOUT)
		{
			CONSOLE_LOG_NETWORK("\t WaitForSingleObject(...) result: WAIT_TIMEOUT\n");
		}
		else
		{
			CONSOLE_LOG_NETWORK("[Error] WaitForSingleObject(...) failed: %d\n", (int)GetLastError());
		}

		hClientThreadHandle = NULL;
	}

	// winsock 라이브러리를 해제합니다.
	WSACleanup();

	/****************************************/

	// 적재된 수신한 모든 데이터를 제거합니다.
	RecvDeque.clear();

	CONSOLE_LOG_NETWORK("[END] <CClient::Close()>\n");
}

void CClient::RegisterHeaderAndStaticFunc(const uint16_t& PacketHeader, void(*StaticFunc)(stringstream&, const SOCKET&))
{
	if (PacketHeader >= MAX_HEADER || PacketHeader < 0)
	{
		CONSOLE_LOG_NETWORK("[ERROR] <CClient::RegisterHeaderAndStaticFunc(...)> if (PacketHeader >= MAX_HEADER || PacketHeader < 0) \n");
		return;
	}

	ProcFuncs[PacketHeader].SetFunc(StaticFunc);
}

void CClient::RegisterConCBF(void(*StaticCBFunc)(CCompletionKey))
{
	ConCBF.SetFunc(StaticCBFunc);
}

void CClient::RegisterDisconCBF(void(*StaticCBFunc)(CCompletionKey))
{
	DisconCBF.SetFunc(StaticCBFunc);
}

void CALLBACK SendCompletionRoutine(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags)
{
	CONSOLE_LOG_NETWORK("[Start] <CClient::SendCompletionRoutine()> \n");

	if (dwError != 0)
	{
		CONSOLE_LOG_NETWORK("[Error] <CClient::SendCompletionRoutine()> Fail to WSASend(...) : %d\n", WSAGetLastError());
	}
	CONSOLE_LOG_NETWORK("[Info] <CClient::SendCompletionRoutine()> Success to WSASend(...)\n");

	if (COverlappedMsg* om = (COverlappedMsg*)lpOverlapped)
	{
		if (om->SendBytes != cbTransferred) // 사이즈가 다르다면 제대로 전송이 되지 않은것이므로 일단 콘솔에 알립니다.
		{
			CONSOLE_LOG_NETWORK("\n\n\n\n\n");
			CONSOLE_LOG_NETWORK("[Error] <CClient::SendCompletionRoutine()> if (overlappedMsg->sendBytes != cbTransferred) \n");
			CONSOLE_LOG_NETWORK("[Error] <CClient::SendCompletionRoutine()> overlappedMsg->SendBytes: %d \n", om->SendBytes);
			CONSOLE_LOG_NETWORK("[Error] <CClient::SendCompletionRoutine()> cbTransferred: %d \n", (int)cbTransferred);
			CONSOLE_LOG_NETWORK("\n\n\n\n\n");
		}

		// 할당을 해제합니다.
		delete om;
		om = nullptr;
	}

	CONSOLE_LOG_NETWORK("[END] <CClient::SendCompletionRoutine()> \n");
}

void CClient::Send(CPacket& Packet, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG_NETWORK("[Start] <CClient::Send(...)>\n");

	// 데이터는 (idxOfStart, idxOfEnd]의 범위를 가지는 것으로 정의합니다.
	uint32_t idxOfStart = 0;
	uint32_t idxOfEnd = 0;

	const string& strOfData = Packet.GetData().str(); // stringstream.str()은 return by value이기 때문에 어쩔 수 없이 성능이 좋지 않습니다.
	const char* c_StrOfData = strOfData.c_str(); // c_str()은 string의 수명을 따라가기 때문에 이 점을 유의하여 사용해야 합니다.

	// 최대크기의 패킷: [("4095") ("256") (Data)(End)('\0')] 이므로 MAX_BUFFER - (4 + 1 + 3 + 1) - 1 - 1
	uint16_t maxSizeOfData = MAX_BUFFER - (uint16_t)(to_string(MAX_BUFFER).length() + 1 + to_string(MAX_HEADER).length() + 1) - 1 - 1;
	size_t totalSizeOfData = strlen(c_StrOfData);

	do
	{
		SetSizeOfDataForSend(idxOfStart, idxOfEnd, maxSizeOfData, c_StrOfData);

		const uint16_t sizeOfData = idxOfEnd - idxOfStart;

		// 남은 데이터에 '\n'만 존재할 때에 대한 예외처리를 진행합니다.
		if (1 <= sizeOfData && sizeOfData <= 4)
		{
			// 모두 같다면 전송할 필요가 없으므로 송신하지 않고 종료합니다.
			if (strncmp(&c_StrOfData[idxOfStart], "\n\n\n\n", sizeOfData) == 0)
			{
				return;
			}
		}

		Packet.CalculateLen(sizeOfData);

		const string& strOfLengthAndHeader = Packet.GetStrOfLengthAndHeader();
		const size_t& lenOfLengthAndHeader = strOfLengthAndHeader.length();

		COverlappedMsg* overlappedMsg = GetOverlappedMsgForSend(strOfLengthAndHeader, lenOfLengthAndHeader, c_StrOfData, idxOfStart, sizeOfData);

		// 처리가 끝난 패킷을 송신합니다.
		Send(overlappedMsg);

		idxOfStart = idxOfEnd;

	} while (idxOfStart < totalSizeOfData);

	CONSOLE_LOG_NETWORK("[End] <CClient::Send(...)>\n");
}

void CClient::Send(COverlappedMsg* OverlappedMsg)
{
	if (!OverlappedMsg)
	{
		CONSOLE_LOG_NETWORK("[Error] <CClient::Send(...)> if (!OverlappedMsg) \n");
		return;
	}
	/****************************************/

	DWORD dwFlags = 0;

	int nResult = WSASend(
		ServerSocket,				  // s: 연결 소켓을 가리키는 소켓 지정 번호입니다.
		&(OverlappedMsg->DataBuf),	  // lpBuffers: WSABUF(:4300)구조체 배열의 포인터로 각각의 WSABUF 구조체는 버퍼와 버퍼의 크기를 가리킵니다.
		1,							  // dwBufferCount: lpBuffers에 있는 WSABUF(:4300)구조체의 개수입니다.
		NULL,						  // lpNumberOfBytesSent: 함수의 호출로 전송된 데이터의 바이트 크기를 넘겨줍니다. 만약 매개 변수 lpOverlapped가 NULL이 아니라면, 이 매개 변수의 값은 NULL로 해야 (잠재적인)잘못된 반환을 피할 수 있습니다.
		dwFlags,					  // dwFlags: WSASend 함수를 어떤 방식으로 호출 할것인지를 지정합니다.
		&(OverlappedMsg->Overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)구조체의 포인터입니다. 비 (overlapped)중첩 소켓에서는 무시됩니다.
		SendCompletionRoutine		  // lpCompletionRoutine: 데이터 전송이 완료 되었을 때 호출할 완료 루틴 (completion routine)의 포인터입니다. 비 중첩 소켓에서는 무시됩니다.
	);

	if (nResult == 0)
	{
		CONSOLE_LOG_NETWORK("[Info] <CClient::Send(...)> Success to WSASend(...) \n");
	}
	else if (nResult == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			CONSOLE_LOG_NETWORK("[Error] <CClient::Send(...)> Fail to WSASend(...) : %d \n", WSAGetLastError());

			// 할당을 해제합니다.
			delete OverlappedMsg;
			OverlappedMsg = nullptr;
		}
		else
		{
			CONSOLE_LOG_NETWORK("[Info] <CClient::Send(...)> WSASend: WSA_IO_PENDING \n");
		}
	}
}

void CClient::LoadUpReceivedDataToRecvDeque(const char* const RecvBuffer, const int& RecvLen)
{
	if (!RecvBuffer)
	{
		CONSOLE_LOG_NETWORK("[Error] <CClient::LoadUpReceivedDataToRecvDeque(...)> if (!RecvBuffer) \n");
		return;
	}
	/****************************************/

	// 데이터가 MAX_BUFFER 그대로 4096개 꽉 채워서 오는 경우에 대비하기 위하여 +1로 '\0' 공간을 만들어줍니다. (그래도 송신할때는 4095개까지만 채워서 갑니다.)
	RecvDeque.emplace_back(make_unique<char[]>(MAX_BUFFER + 1)); // 뒷부분에 순차적으로 적재합니다.
	RecvDeque.back().get()[MAX_BUFFER] = '\0';
	CopyMemory(RecvDeque.back().get(), RecvBuffer, RecvLen);
	RecvDeque.back().get()[RecvLen] = '\0';
}

void CClient::GetPacketsFromRecvDeque(char* const BufOfPackets)
{
	if (!BufOfPackets)
	{
		CONSOLE_LOG_NETWORK("[Error] <CClient::GetPacketsFromRecvDeque(...)> if (!BufOfPackets) \n");
		return;
	}
	/****************************************/

	// 초기화
	BufOfPackets[0] = '\0';
	BufOfPackets[MAX_BUFFER] = '\0';
	BufOfPackets[MAX_BUFFER * 2] = '\0';

	size_t idxOfCur = 0;
	size_t idxOfEnd = 0;

	while (RecvDeque.empty() == false)
	{
		// 무조건 꺼냅니다.
		CopyMemory(&BufOfPackets[idxOfCur], RecvDeque.front().get(), strlen(RecvDeque.front().get()));
		idxOfCur += strlen(RecvDeque.front().get());
		BufOfPackets[idxOfCur] = '\0';
		RecvDeque.pop_front();

		// 
		if (idxOfCur >= MAX_BUFFER)
		{
			break;
		}
	}

	// 3가지 경우의 수가 존재합니다.
	if (idxOfCur == 0) // 패킷이 존재하지 않으므로 바로 종료합니다.
	{
		return;
	}
	else if (idxOfCur < MAX_BUFFER) // RecvDeque이 비어있는 상태이므로 idxOfCur를 기준으로 패킷의 끝을 찾습니다.
	{
		idxOfEnd = idxOfCur - 1;
	}
	else if (idxOfCur >= MAX_BUFFER) // MAX_BUFFER 크기를 채운 상태이므로 MAX_BUFFER를 기준으로 패킷의 끝을 찾습니다.
	{
		idxOfEnd = MAX_BUFFER - 1;
	}

	// MAX_BUFFER 이내에 존재하는 패킷의 끝을 체크합니다.
	for (; idxOfEnd > 0; idxOfEnd--)
	{
		if (BufOfPackets[idxOfEnd] == (char)3)
		{
			idxOfEnd++;
			break;
		}
	}

	// 같다면, 온전한 패킷들만 존재하므로 나머지가 없어서 다시 RecvDeque에 적재할 필요가 없습니다.
	if (idxOfEnd == idxOfCur)
	{
		return;
	}

	// 나머지 부분을 다시 적재합니다.
	size_t remaining = idxOfCur - idxOfEnd;
	if (remaining <= MAX_BUFFER)
	{
		RecvDeque.emplace_front(make_unique<char[]>(MAX_BUFFER + 1));
		RecvDeque.front().get()[MAX_BUFFER] = '\0';
		CopyMemory(RecvDeque.front().get(), &BufOfPackets[idxOfEnd], remaining);
		RecvDeque.front().get()[remaining] = '\0';

		BufOfPackets[idxOfEnd] = '\0';
	}
	else // 나머지 부분의 크기가 MAX_BUFFER를 초과하면 분할해서 다시 적재해야 합니다.
	{
		size_t halfOfRemaining = (size_t)(remaining * 0.5);

		// 뒤의 절반을 먼저 적재합니다.
		RecvDeque.emplace_front(make_unique<char[]>(MAX_BUFFER + 1));
		RecvDeque.front().get()[MAX_BUFFER] = '\0';
		CopyMemory(RecvDeque.front().get(), &BufOfPackets[idxOfEnd + halfOfRemaining], remaining - halfOfRemaining);
		RecvDeque.front().get()[remaining - halfOfRemaining] = '\0';

		// 그다음 앞의 절반을 적재합니다.
		RecvDeque.emplace_front(make_unique<char[]>(MAX_BUFFER + 1));
		RecvDeque.front().get()[MAX_BUFFER] = '\0';
		CopyMemory(RecvDeque.front().get(), &BufOfPackets[idxOfEnd], halfOfRemaining);
		RecvDeque.front().get()[halfOfRemaining] = '\0';

		BufOfPackets[idxOfEnd] = '\0';
	}

	CONSOLE_LOG_NETWORK("[Final] <CClient::GetPacketsFromRecvDeque(...)> %s \n", BufOfPackets);
}

void CClient::DividePacketsAndProcessThePacket(const char* const BufOfPackets)
{
	if (!BufOfPackets)
	{
		CONSOLE_LOG_NETWORK("[Error] <CClient::DivideDataToPacketAndProcessThePacket(...)> if (!BufOfPackets) \n");
		return;
	}
	/****************************************/

	size_t idxOfCur = 0;

	char bufOfPacket[MAX_BUFFER + 1];

	while (idxOfCur < strlen(BufOfPackets))
	{
		char sizeBuffer[5]; // [1234\0]
		CopyMemory(sizeBuffer, &BufOfPackets[idxOfCur], 4); // 앞 4자리 데이터만 sizeBuffer에 복사합니다.
		sizeBuffer[4] = '\0';

		stringstream sizeStream;
		sizeStream << sizeBuffer;
		uint16_t sizeOfPacket = 0;
		sizeStream >> sizeOfPacket;

		// 패킷의 전체크기가 0이거나 남은 버퍼 크기보다 크거나 끝이 없는 경우, 오류가 발생한 것이므로 패킷 처리를 중단합니다.
		if (sizeOfPacket == 0)
		{
			CONSOLE_LOG_NETWORK("\n\n\n\n\n[Error] <CClient::IOThread()> if (sizeOfPacket == 0) \n\n\n\n\n\n");
			break;;
		}
		if (sizeOfPacket > strlen(&BufOfPackets[idxOfCur]))
		{
			CONSOLE_LOG_NETWORK("\n\n\n\n\n[Error] <CClient::IOThread()> if (sizeOfPacket > strlen(&BufOfPackets[idxOfCur])) sizeOfPacket: %d \n\n\n\n\n\n", (int)sizeOfPacket);
			break;;
		}
		if (BufOfPackets[idxOfCur + sizeOfPacket - 1] != (char)3)
		{
			CONSOLE_LOG_NETWORK("\n\n\n\n\n[Error] <CClient::IOThread()> if (BufOfPackets[sizeOfPacket - 1] != (char)3) sizeOfPacket: %d \n\n\n\n\n\n", (int)sizeOfPacket);
			break;;
		}

		// 패킷을 자르면서 임시 버퍼에 복사합니다.(복사할 때 패킷의 끝은 제거합니다.)
		CopyMemory(bufOfPacket, &BufOfPackets[idxOfCur], sizeOfPacket - 1);
		bufOfPacket[sizeOfPacket - 1] = '\0';

		// 패킷을 처리합니다.
		ProcessThePacket(bufOfPacket);

		idxOfCur += sizeOfPacket;
	}
}

void CClient::ProcessThePacket(const char* const BufOfPacket)
{
	if (!BufOfPacket)
	{
		CONSOLE_LOG_NETWORK("[Error] <CClient::ProcessThePacket(...)> if (!BufOfPacket) \n");
		return;
	}
	/****************************************/

	CONSOLE_LOG_NETWORK("<CClient::ProcessThePacket(...)> %s \n", BufOfPacket);

	stringstream recvStream;
	recvStream << BufOfPacket;

	// 패킷의 전체크기를 획득합니다.
	uint16_t sizeOfRecvStream = 0;
	recvStream >> sizeOfRecvStream;
	CONSOLE_LOG_NETWORK("\t sizeOfRecvStream: %d \n", (int)sizeOfRecvStream);

	// 전체크기 예외를 처리합니다.
	if (sizeOfRecvStream == 0)
	{
		CONSOLE_LOG_NETWORK("[ERROR] <CClient::RegisterHeaderAndStaticFunc(...)> if (sizeOfRecvStream == 0) \n");
		return;
	}

	// 패킷의 헤더를 획득합니다.
	uint16_t header = -1;
	recvStream >> header;
	CONSOLE_LOG_NETWORK("\t packetHeader: %d \n", (int)header);

	// 헤더 범위의 예외를 처리합니다.
	if (header >= MAX_HEADER || header < 0)
	{
		CONSOLE_LOG_NETWORK("[ERROR] <CClient::RegisterHeaderAndStaticFunc(...)> if (header >= MAX_HEADER || header < 0) \n");
		return;
	}

	ProcFuncs[header].ExecuteFunc(recvStream, NULL);
}

void CClient::SetSizeOfDataForSend(const uint32_t& IdxOfStart, uint32_t& IdxOfEnd, const uint16_t& MaxSizeOfData, const char* const C_StrOfData)
{
	// 먼저 IdxOfEnd를 데이터의 크기로 설정합니다.
	IdxOfEnd = (uint16_t)strlen(C_StrOfData);

	// 데이터가 최대크기를 초과하면 데이터를 자르기 위해 IdxOfEnd를 최대크기 이내로 조절합니다.
	if ((IdxOfEnd - IdxOfStart) > MaxSizeOfData)
	{
		for (uint32_t cur = (IdxOfStart + MaxSizeOfData - 1); cur > IdxOfStart; cur--)
		{
			// 데이터의 끝을 '\n'으로 잘 설정했다면, 찾지 못하는 상황이 올 수 없으므로 오류가 발생한 것이기 때문에 송신하지 않고 종료합니다.
			if (cur <= IdxOfStart)
			{
				CONSOLE_LOG_NETWORK("\n\n\n\n\n[ERROR] <CClient::Send(...)> if (cur <= idxOfStart) \n\n\n\n\n\n");
				return;
			}

			// 탐색하던 중 데이터 끝을 발견하면
			if (C_StrOfData[cur] == '\n')
			{
				IdxOfEnd = cur + 1; // 끝 다음을 가리키도록 합니다.
				return;
			}
		}
	}
}

COverlappedMsg* CClient::GetOverlappedMsgForSend(const string& StrOfLengthAndHeader, const size_t& LenOfLengthAndHeader, const char* const C_StrOfData, const uint32_t& IdxOfStart, const uint16_t& sizeOfData)
{
	COverlappedMsg* overlappedMsg = CExceptionHandler<COverlappedMsg>::MustDynamicAlloc();

	CopyMemory(overlappedMsg->MessageBuffer, StrOfLengthAndHeader.c_str(), LenOfLengthAndHeader); // 전체크기와 헤더
	CopyMemory(&overlappedMsg->MessageBuffer[LenOfLengthAndHeader], &C_StrOfData[IdxOfStart], sizeOfData); // 데이터
	stringstream endStream;
	endStream << (char)3;
	CopyMemory(&overlappedMsg->MessageBuffer[LenOfLengthAndHeader + sizeOfData], endStream.str().c_str(), 1); // 끝
	overlappedMsg->MessageBuffer[LenOfLengthAndHeader + sizeOfData + 1] = '\0';
	overlappedMsg->DataBuf.len = (ULONG)(LenOfLengthAndHeader + sizeOfData + 1);
	overlappedMsg->DataBuf.buf = overlappedMsg->MessageBuffer;
	overlappedMsg->SendBytes = overlappedMsg->DataBuf.len;

	return overlappedMsg;

}