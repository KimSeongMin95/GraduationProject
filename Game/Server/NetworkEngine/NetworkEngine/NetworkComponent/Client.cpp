
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

	// ũ��Ƽ�� ���ǿ� ���ɶ��� �ɰ� �ʱ�ȭ�� �����Ҷ����� �õ��մϴ�.
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
	// �̹� Ŭ���̾�Ʈ�� �������̶�� ���� ������ �����մϴ�.
	if (IsNetworkOn())
	{
		CONSOLE_LOG_NETWORK("[Info] <CClient::Initialize()> if (IsNetworkOn()) \n");
		Close();
	}
	CONSOLE_LOG_NETWORK("\n\n/********** CClient **********/ \n");
	CONSOLE_LOG_NETWORK("[Start] <CClient::Initialize()> \n");

	WSADATA wsaData;

	// winsock ���̺귯���� 2.2 �������� �ʱ�ȭ�մϴ�.
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		CONSOLE_LOG_NETWORK("[Fail]  WSAStartup(...); \n");
		return false;
	}
	CONSOLE_LOG_NETWORK("\t [Success] WSAStartup(...) \n");

	// TCP ������ �����մϴ�.
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET)
	{
		CONSOLE_LOG_NETWORK("[Error] WSASocket(...); \n");
		WSACleanup();
		return false;
	}
	CONSOLE_LOG_NETWORK("\t [Success] WSASocket(...)\n");

	SetSockOpt(ServerSocket, 1048576, 1048576);

	// ������ �ּ� ������ �����մϴ�.
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	if (inet_pton(AF_INET, IPv4, &serverAddr.sin_addr.s_addr) != 1)
	{
		CONSOLE_LOG_NETWORK("[Fail] inet_pton(...) \n");
		CloseSocketAndWSACleanup(ServerSocket);
		return false;
	}
	serverAddr.sin_port = htons(Port);

	// ������ �ּ� ������ ����մϴ�.
	char bufOfIPv4Addr[32] = { 0, };
	CONSOLE_LOG_NETWORK("\t IPv4: %s \n", inet_ntop(AF_INET, &serverAddr.sin_addr, bufOfIPv4Addr, sizeof(bufOfIPv4Addr)));
	CONSOLE_LOG_NETWORK("\t Port: %d \n", ntohs(serverAddr.sin_port));

	// ������ ������ �õ��մϴ�.
	if (connect(ServerSocket, (sockaddr*)&serverAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		CONSOLE_LOG_NETWORK("[Fail] connect(...) \n");
		CloseSocketAndWSACleanup(ServerSocket);
		return false;
	}
	CONSOLE_LOG_NETWORK("\t [Success] connect(...) \n");

	// Ŭ���̾�Ʈ �����带 �����մϴ�.
	if (CreateClientThread() == false)
	{
		CONSOLE_LOG_NETWORK("[Fail] CreateClientThread()\n");
		CloseSocketAndWSACleanup(ServerSocket);
		return false;
	}
	CONSOLE_LOG_NETWORK("\t [Success] CreateClientThread()\n");

	// ���� Ŭ���̾�Ʈ ������ �����մϴ�.
	EnterCriticalSection(&csAccept);
	bAccept = true;
	LeaveCriticalSection(&csAccept);

	// �̸� ������ �����带 �簳�մϴ�.
	ResumeThread(hClientThreadHandle);

	// ������ ������ ����
	EnterCriticalSection(&csServer);
	Server.Socket = ServerSocket;
	Server.IPv4Addr = string(IPv4);
	Server.Port = (unsigned int)Port;
	ConCBF.ExecuteFunc(Server); // ������ �����ϸ� ������ �ݹ��Լ� �����մϴ�.
	LeaveCriticalSection(&csServer);

	CONSOLE_LOG_NETWORK("[End] <CClient::Initialize()> \n");
	return true;
}

bool CClient::CreateClientThread()
{
	unsigned int threadId;

	// _beginthreadex()�� ::CloseHandle�� ���ο��� ȣ������ �ʱ� ������, ������ ����� ����ڰ� ���� CloseHandle()���־�� �մϴ�. �����尡 ����Ǹ� _endthreadex()�� �ڵ����� ȣ��˴ϴ�.
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
	int nRecvLen = 0; // ������ ����Ʈ ũ�⸦ �����մϴ�.
	char recvBuffer[MAX_BUFFER + 1]; // ������ �����͸� ������ �����Դϴ�.
	char bufOfPackets[2 * MAX_BUFFER + 1]; // RecvDeque���κ��� ȹ���ϴ� ��Ŷ���� �����մϴ�.

	// Ŭ���̾�Ʈ�� ���������� �����մϴ�.
	while (true)
	{
		nRecvLen = 0;

		// SendCompletionRoutine�� ���� ��� Alertable Wait ���·� ����ϴ�.
		SleepEx(1, true);

		// Ŭ���̾�Ʈ ������ ���� Ȯ��
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			LeaveCriticalSection(&csAccept);
			CONSOLE_LOG_NETWORK("[Info] <CClient::ClientThread()> if (!bAccept) \n");
			CONSOLE_LOG_NETWORK("[Info] <CClient::ClientThread()> Client thread is closed! \n");
			return;
		}
		LeaveCriticalSection(&csAccept);

		///* FIONREAD ����
		//FIONREAD�� ��Ʈ��ũ �Է� ���ۿ��� ��ٸ��� �ִ� ���� s�κ��� ���� �� �ִ� �������� ũ��(amount)�� ���µ� ���˴ϴ�.
		//ioctlsocket(...) �Լ��� argp �Ű������� ���� �� �ִ� �������� ũ�⸦ ȹ���� �� �ֽ��ϴ�.
		//���� ������(stream oriented) ����(��:SOCK_STREAM)�� ���, recv �Լ��� ȣ��� ���� �� �ִ� �������� ũ��(amount)�� ȹ���� �� �ֽ��ϴ�.
		//���� ������ �޽��� ������(message oriented) ����(��:SOCK_DGRAM)�� ���, ���Ͽ� ť�� ù ��° �����ͱ׷��� ũ�⸦ ��ȯ �մϴ�.
		//*/
		//u_long amount = 0;
		//if (ioctlsocket(ServerSocket, FIONREAD, &amount) == -1)
		//{
		//	CONSOLE_LOG_NETWORK("[Error] if (ioctlsocket(ServerSocket, FIONREAD, &amount) == -1) \n");
		//	continue;
		//}

		// ����
		nRecvLen = recv(ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0);
		if (nRecvLen == SOCKET_ERROR || nRecvLen == 0)
		{
			// ������ ������ ����Ǹ� ������ �ݹ��Լ��� �����մϴ�.
			DisconCBF.ExecuteFunc(GetCompletionKey());
			continue;
		}
		recvBuffer[nRecvLen] = '\0';

		// RecvDeque�� ����ְ� ������ �����Ͱ� ������ ��Ŷ�̶�� RecvDeque�� ��ġ�� �ʰ� �ٷ� ó���Ͽ� ������ ����ŵ�ϴ�.
		if (RecvDeque.empty() == true && recvBuffer[nRecvLen - 1] == (char)3)
		{
			// ������ ��Ŷ���� ��Ŷ���� �����ϰ� ���������� ��Ŷ�� ó���մϴ�.
			DividePacketsAndProcessThePacket(recvBuffer);
		}
		else // �׷��� �ʴٸ� ������ ���� ó���� �����մϴ�.
		{
			// RecvDeque�� �޺κп� ������ �����͸� �����մϴ�.
			LoadUpReceivedDataToRecvDeque(recvBuffer, nRecvLen);

			// RecvDeque���κ��� ��Ŷ���� ȹ���մϴ�.
			GetPacketsFromRecvDeque(bufOfPackets);

			// ȹ���� ��Ŷ���� ��Ŷ���� �����ϰ� ���������� ��Ŷ�� ó���մϴ�.
			DividePacketsAndProcessThePacket(bufOfPackets);
		}


		// �����ִ� WSASend(...)�� �� ������ ���� Alertable Wait ���·� ����ϴ�.
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

	// Ŭ���̾�Ʈ�� �����ϸ� �����ִ� WSASend(...)�� �� ������ ���� Alertable Wait ���·� ����ϴ�.
	SleepEx(1, true);

	// ������ �������϶��� �����ϵ��� �մϴ�.
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

	// ������ ������ ����Ǹ� ������ �ݹ��Լ��� �����մϴ�.
	DisconCBF.ExecuteFunc(GetCompletionKey());

	// ������ ������ �ʱ�ȭ�մϴ�.
	EnterCriticalSection(&csServer);
	Server = CCompletionKey();
	LeaveCriticalSection(&csServer);

	// ���� ���̻� ������� ���ϵ��� ServerSocket�� �ݽ��ϴ�.
	CloseSocketWithCheck(ServerSocket);

	// �� ����, Ŭ���̾�Ʈ �������� ���Ḧ Ȯ���մϴ�.
	if (hClientThreadHandle != NULL && hClientThreadHandle != INVALID_HANDLE_VALUE)
	{
		DWORD result = WaitForSingleObject(hClientThreadHandle, INFINITE);

		if (result == WAIT_OBJECT_0) // hClientThreadHandle�� signal�̸�
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

	// winsock ���̺귯���� �����մϴ�.
	WSACleanup();

	/****************************************/

	// ����� ������ ��� �����͸� �����մϴ�.
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
		if (om->SendBytes != cbTransferred) // ����� �ٸ��ٸ� ����� ������ ���� �������̹Ƿ� �ϴ� �ֿܼ� �˸��ϴ�.
		{
			CONSOLE_LOG_NETWORK("\n\n\n\n\n");
			CONSOLE_LOG_NETWORK("[Error] <CClient::SendCompletionRoutine()> if (overlappedMsg->sendBytes != cbTransferred) \n");
			CONSOLE_LOG_NETWORK("[Error] <CClient::SendCompletionRoutine()> overlappedMsg->SendBytes: %d \n", om->SendBytes);
			CONSOLE_LOG_NETWORK("[Error] <CClient::SendCompletionRoutine()> cbTransferred: %d \n", (int)cbTransferred);
			CONSOLE_LOG_NETWORK("\n\n\n\n\n");
		}

		// �Ҵ��� �����մϴ�.
		delete om;
		om = nullptr;
	}

	CONSOLE_LOG_NETWORK("[END] <CClient::SendCompletionRoutine()> \n");
}

void CClient::Send(CPacket& Packet, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG_NETWORK("[Start] <CClient::Send(...)>\n");

	// �����ʹ� (idxOfStart, idxOfEnd]�� ������ ������ ������ �����մϴ�.
	uint32_t idxOfStart = 0;
	uint32_t idxOfEnd = 0;

	const string& strOfData = Packet.GetData().str(); // stringstream.str()�� return by value�̱� ������ ��¿ �� ���� ������ ���� �ʽ��ϴ�.
	const char* c_StrOfData = strOfData.c_str(); // c_str()�� string�� ������ ���󰡱� ������ �� ���� �����Ͽ� ����ؾ� �մϴ�.

	// �ִ�ũ���� ��Ŷ: [("4095") ("256") (Data)(End)('\0')] �̹Ƿ� MAX_BUFFER - (4 + 1 + 3 + 1) - 1 - 1
	uint16_t maxSizeOfData = MAX_BUFFER - (uint16_t)(to_string(MAX_BUFFER).length() + 1 + to_string(MAX_HEADER).length() + 1) - 1 - 1;
	size_t totalSizeOfData = strlen(c_StrOfData);

	do
	{
		SetSizeOfDataForSend(idxOfStart, idxOfEnd, maxSizeOfData, c_StrOfData);

		const uint16_t sizeOfData = idxOfEnd - idxOfStart;

		// ���� �����Ϳ� '\n'�� ������ ���� ���� ����ó���� �����մϴ�.
		if (1 <= sizeOfData && sizeOfData <= 4)
		{
			// ��� ���ٸ� ������ �ʿ䰡 �����Ƿ� �۽����� �ʰ� �����մϴ�.
			if (strncmp(&c_StrOfData[idxOfStart], "\n\n\n\n", sizeOfData) == 0)
			{
				return;
			}
		}

		Packet.CalculateLen(sizeOfData);

		const string& strOfLengthAndHeader = Packet.GetStrOfLengthAndHeader();
		const size_t& lenOfLengthAndHeader = strOfLengthAndHeader.length();

		COverlappedMsg* overlappedMsg = GetOverlappedMsgForSend(strOfLengthAndHeader, lenOfLengthAndHeader, c_StrOfData, idxOfStart, sizeOfData);

		// ó���� ���� ��Ŷ�� �۽��մϴ�.
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
		ServerSocket,				  // s: ���� ������ ����Ű�� ���� ���� ��ȣ�Դϴ�.
		&(OverlappedMsg->DataBuf),	  // lpBuffers: WSABUF(:4300)����ü �迭�� �����ͷ� ������ WSABUF ����ü�� ���ۿ� ������ ũ�⸦ ����ŵ�ϴ�.
		1,							  // dwBufferCount: lpBuffers�� �ִ� WSABUF(:4300)����ü�� �����Դϴ�.
		NULL,						  // lpNumberOfBytesSent: �Լ��� ȣ��� ���۵� �������� ����Ʈ ũ�⸦ �Ѱ��ݴϴ�. ���� �Ű� ���� lpOverlapped�� NULL�� �ƴ϶��, �� �Ű� ������ ���� NULL�� �ؾ� (��������)�߸��� ��ȯ�� ���� �� �ֽ��ϴ�.
		dwFlags,					  // dwFlags: WSASend �Լ��� � ������� ȣ�� �Ұ������� �����մϴ�.
		&(OverlappedMsg->Overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)����ü�� �������Դϴ�. �� (overlapped)��ø ���Ͽ����� ���õ˴ϴ�.
		SendCompletionRoutine		  // lpCompletionRoutine: ������ ������ �Ϸ� �Ǿ��� �� ȣ���� �Ϸ� ��ƾ (completion routine)�� �������Դϴ�. �� ��ø ���Ͽ����� ���õ˴ϴ�.
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

			// �Ҵ��� �����մϴ�.
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

	// �����Ͱ� MAX_BUFFER �״�� 4096�� �� ä���� ���� ��쿡 ����ϱ� ���Ͽ� +1�� '\0' ������ ������ݴϴ�. (�׷��� �۽��Ҷ��� 4095�������� ä���� ���ϴ�.)
	RecvDeque.emplace_back(make_unique<char[]>(MAX_BUFFER + 1)); // �޺κп� ���������� �����մϴ�.
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

	// �ʱ�ȭ
	BufOfPackets[0] = '\0';
	BufOfPackets[MAX_BUFFER] = '\0';
	BufOfPackets[MAX_BUFFER * 2] = '\0';

	size_t idxOfCur = 0;
	size_t idxOfEnd = 0;

	while (RecvDeque.empty() == false)
	{
		// ������ �����ϴ�.
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

	// 3���� ����� ���� �����մϴ�.
	if (idxOfCur == 0) // ��Ŷ�� �������� �����Ƿ� �ٷ� �����մϴ�.
	{
		return;
	}
	else if (idxOfCur < MAX_BUFFER) // RecvDeque�� ����ִ� �����̹Ƿ� idxOfCur�� �������� ��Ŷ�� ���� ã���ϴ�.
	{
		idxOfEnd = idxOfCur - 1;
	}
	else if (idxOfCur >= MAX_BUFFER) // MAX_BUFFER ũ�⸦ ä�� �����̹Ƿ� MAX_BUFFER�� �������� ��Ŷ�� ���� ã���ϴ�.
	{
		idxOfEnd = MAX_BUFFER - 1;
	}

	// MAX_BUFFER �̳��� �����ϴ� ��Ŷ�� ���� üũ�մϴ�.
	for (; idxOfEnd > 0; idxOfEnd--)
	{
		if (BufOfPackets[idxOfEnd] == (char)3)
		{
			idxOfEnd++;
			break;
		}
	}

	// ���ٸ�, ������ ��Ŷ�鸸 �����ϹǷ� �������� ��� �ٽ� RecvDeque�� ������ �ʿ䰡 �����ϴ�.
	if (idxOfEnd == idxOfCur)
	{
		return;
	}

	// ������ �κ��� �ٽ� �����մϴ�.
	size_t remaining = idxOfCur - idxOfEnd;
	if (remaining <= MAX_BUFFER)
	{
		RecvDeque.emplace_front(make_unique<char[]>(MAX_BUFFER + 1));
		RecvDeque.front().get()[MAX_BUFFER] = '\0';
		CopyMemory(RecvDeque.front().get(), &BufOfPackets[idxOfEnd], remaining);
		RecvDeque.front().get()[remaining] = '\0';

		BufOfPackets[idxOfEnd] = '\0';
	}
	else // ������ �κ��� ũ�Ⱑ MAX_BUFFER�� �ʰ��ϸ� �����ؼ� �ٽ� �����ؾ� �մϴ�.
	{
		size_t halfOfRemaining = (size_t)(remaining * 0.5);

		// ���� ������ ���� �����մϴ�.
		RecvDeque.emplace_front(make_unique<char[]>(MAX_BUFFER + 1));
		RecvDeque.front().get()[MAX_BUFFER] = '\0';
		CopyMemory(RecvDeque.front().get(), &BufOfPackets[idxOfEnd + halfOfRemaining], remaining - halfOfRemaining);
		RecvDeque.front().get()[remaining - halfOfRemaining] = '\0';

		// �״��� ���� ������ �����մϴ�.
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
		CopyMemory(sizeBuffer, &BufOfPackets[idxOfCur], 4); // �� 4�ڸ� �����͸� sizeBuffer�� �����մϴ�.
		sizeBuffer[4] = '\0';

		stringstream sizeStream;
		sizeStream << sizeBuffer;
		uint16_t sizeOfPacket = 0;
		sizeStream >> sizeOfPacket;

		// ��Ŷ�� ��üũ�Ⱑ 0�̰ų� ���� ���� ũ�⺸�� ũ�ų� ���� ���� ���, ������ �߻��� ���̹Ƿ� ��Ŷ ó���� �ߴ��մϴ�.
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

		// ��Ŷ�� �ڸ��鼭 �ӽ� ���ۿ� �����մϴ�.(������ �� ��Ŷ�� ���� �����մϴ�.)
		CopyMemory(bufOfPacket, &BufOfPackets[idxOfCur], sizeOfPacket - 1);
		bufOfPacket[sizeOfPacket - 1] = '\0';

		// ��Ŷ�� ó���մϴ�.
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

	// ��Ŷ�� ��üũ�⸦ ȹ���մϴ�.
	uint16_t sizeOfRecvStream = 0;
	recvStream >> sizeOfRecvStream;
	CONSOLE_LOG_NETWORK("\t sizeOfRecvStream: %d \n", (int)sizeOfRecvStream);

	// ��üũ�� ���ܸ� ó���մϴ�.
	if (sizeOfRecvStream == 0)
	{
		CONSOLE_LOG_NETWORK("[ERROR] <CClient::RegisterHeaderAndStaticFunc(...)> if (sizeOfRecvStream == 0) \n");
		return;
	}

	// ��Ŷ�� ����� ȹ���մϴ�.
	uint16_t header = -1;
	recvStream >> header;
	CONSOLE_LOG_NETWORK("\t packetHeader: %d \n", (int)header);

	// ��� ������ ���ܸ� ó���մϴ�.
	if (header >= MAX_HEADER || header < 0)
	{
		CONSOLE_LOG_NETWORK("[ERROR] <CClient::RegisterHeaderAndStaticFunc(...)> if (header >= MAX_HEADER || header < 0) \n");
		return;
	}

	ProcFuncs[header].ExecuteFunc(recvStream, NULL);
}

void CClient::SetSizeOfDataForSend(const uint32_t& IdxOfStart, uint32_t& IdxOfEnd, const uint16_t& MaxSizeOfData, const char* const C_StrOfData)
{
	// ���� IdxOfEnd�� �������� ũ��� �����մϴ�.
	IdxOfEnd = (uint16_t)strlen(C_StrOfData);

	// �����Ͱ� �ִ�ũ�⸦ �ʰ��ϸ� �����͸� �ڸ��� ���� IdxOfEnd�� �ִ�ũ�� �̳��� �����մϴ�.
	if ((IdxOfEnd - IdxOfStart) > MaxSizeOfData)
	{
		for (uint32_t cur = (IdxOfStart + MaxSizeOfData - 1); cur > IdxOfStart; cur--)
		{
			// �������� ���� '\n'���� �� �����ߴٸ�, ã�� ���ϴ� ��Ȳ�� �� �� �����Ƿ� ������ �߻��� ���̱� ������ �۽����� �ʰ� �����մϴ�.
			if (cur <= IdxOfStart)
			{
				CONSOLE_LOG_NETWORK("\n\n\n\n\n[ERROR] <CClient::Send(...)> if (cur <= idxOfStart) \n\n\n\n\n\n");
				return;
			}

			// Ž���ϴ� �� ������ ���� �߰��ϸ�
			if (C_StrOfData[cur] == '\n')
			{
				IdxOfEnd = cur + 1; // �� ������ ����Ű���� �մϴ�.
				return;
			}
		}
	}
}

COverlappedMsg* CClient::GetOverlappedMsgForSend(const string& StrOfLengthAndHeader, const size_t& LenOfLengthAndHeader, const char* const C_StrOfData, const uint32_t& IdxOfStart, const uint16_t& sizeOfData)
{
	COverlappedMsg* overlappedMsg = CExceptionHandler<COverlappedMsg>::MustDynamicAlloc();

	CopyMemory(overlappedMsg->MessageBuffer, StrOfLengthAndHeader.c_str(), LenOfLengthAndHeader); // ��üũ��� ���
	CopyMemory(&overlappedMsg->MessageBuffer[LenOfLengthAndHeader], &C_StrOfData[IdxOfStart], sizeOfData); // ������
	stringstream endStream;
	endStream << (char)3;
	CopyMemory(&overlappedMsg->MessageBuffer[LenOfLengthAndHeader + sizeOfData], endStream.str().c_str(), 1); // ��
	overlappedMsg->MessageBuffer[LenOfLengthAndHeader + sizeOfData + 1] = '\0';
	overlappedMsg->DataBuf.len = (ULONG)(LenOfLengthAndHeader + sizeOfData + 1);
	overlappedMsg->DataBuf.buf = overlappedMsg->MessageBuffer;
	overlappedMsg->SendBytes = overlappedMsg->DataBuf.len;

	return overlappedMsg;

}