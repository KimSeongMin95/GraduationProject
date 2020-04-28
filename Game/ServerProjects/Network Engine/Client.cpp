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
	// ��� ���� �ʱ�ȭ
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
	/// �������� �����ϱ� ���Ͽ�, �������� Ŭ���̾�Ʈ�� �ݾ��ݴϴ�.
	CloseClient();

	if (IsClientOn())
	{
		CONSOLE_LOG("[Info] <CClient::Initialize()> if (IsClientOn()) \n");

		return true;
	}
	CONSOLE_LOG("\n\n/********** CClient **********/ \n");
	CONSOLE_LOG("[Start] <CClient::Initialize()> \n");


	WSADATA wsaData;

	// ���� ������ 2.2�� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		CONSOLE_LOG("[Fail]  WSAStartup(...); \n");
		return false;
	}
	CONSOLE_LOG("\t [Success] WSAStartup(...) \n");


	// TCP ���� ����	
	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET)
	{
		CONSOLE_LOG("[Error] WSASocket(...); \n");
		WSACleanup();
		return false;
	}
	CONSOLE_LOG("\t [Success] WSASocket(...)\n");
	SetSockOpt(ServerSocket, 1048576, 1048576);


	// ������ ���� ������ ������ ����ü
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


	// ���� �õ�
	if (connect(ServerSocket, (sockaddr*)& serverAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		CONSOLE_LOG("[Fail] connect(...) \n");
		CloseServerSocketAndCleanupWSA();
		return false;
	}
	CONSOLE_LOG("\t [Success] connect(...) \n");


	// Ŭ���̾�Ʈ ������ ����
	if (CreateClientThread() == false)
	{
		CONSOLE_LOG("[Fail] CreateClientThread()\n");
		CloseServerSocketAndCleanupWSA();
		return false;
	}
	CONSOLE_LOG("\t [Success] CreateClientThread()\n");


	// ������ ������ ����
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

	// _beginthreadex()�� ::CloseHandle�� ���ο��� ȣ������ �ʱ� ������, ������ ����� ����ڰ� ���� CloseHandle()����� �մϴ�.
	// �����尡 ����Ǹ� _endthreadex()�� �ڵ�ȣ��˴ϴ�.
	hMainHandle = (HANDLE*)_beginthreadex(NULL, 0, &CallClientThread, this, CREATE_SUSPENDED, &threadId);
	if (hMainHandle == NULL)
	{
		CONSOLE_LOG("[Error] <CClient::CreateClientThread()> if (hMainHandle == NULL)\n");
		return false;
	}
	// Ŭ���̾�Ʈ ����
	EnterCriticalSection(&csAccept);
	bAccept = true;
	LeaveCriticalSection(&csAccept);

	// ������ �簳
	ResumeThread(hMainHandle);

	return true;
}

void CClient::ClientThread()
{
	// ���� ���� ��Ʈ�� (�ִ� MAX_BUFFER �������� �����͸� �����ϱ� ������, ������ '\0'�� ����� �ʿ�)
	char recvBuffer[MAX_BUFFER + 1];

	// recv while loop ����
	// StopTaskCounter Ŭ���� ������ ����� Thread Safety�ϰ� ����
	while (true)
	{
		// Alertable Wait ���¸� ����� ����
		SleepEx(1, true);

		// Ŭ���̾�Ʈ ������ ���� Ȯ��
		EnterCriticalSection(&csAccept);
		if (!bAccept)
		{
			CONSOLE_LOG("[Info] <CClient::ClientThread()> if (!bAccept) \n");
			CONSOLE_LOG("[Info] <CClient::ClientThread()> Client thread is closed! \n");

			LeaveCriticalSection(&csAccept);
			return;
		}
		LeaveCriticalSection(&csAccept);


		// ���� ����Ʈ ����
		int nRecvLen = 0;

		/*
		FIONREAD
		��Ʈ�� �Է� ���ۿ��� ��ٸ��� �ִ�, ���� s�κ��� ���� �� �ִ� �������� ũ��(amount)�� ���µ� ���˴ϴ�.
		argp �Ű������� �������� ũ�⸦ �ǹ��ϴ� unsigned long ���·� ����Ʈ �մϴ�.
		�ٽ� �����ڸ�, ���� s �Ű������� ����������(stream oriented) ����(��:SOCK_STREAM) �� ���,
		FIONREAD �ĸǵ忡 ���� ioctlsocket �Լ��� ȣ���� recv �Լ��� ȣ��� ���� �� �ִ�
		�������� ũ��(amount)�� ��ȯ�ϰ� �Ǵ°���. ���� ������ �޽��� ������(message oriented) ����(��:SOCK_DGRAM) �� ���
		FIONREAD �ĸǵ�� ���Ͽ� ť�� ù ��° �����ͱ׷��� ũ�⸦ ��ȯ �մϴ�.
		*/
		u_long amount = 0;
		if (ioctlsocket(ServerSocket, FIONREAD, &amount) == -1)
		{
			CONSOLE_LOG("[Error] if (ioctlsocket(ServerSocket, FIONREAD, &amount) == -1) \n");
			continue;
		}

		// recv ���ۿ� �����Ͱ� 4����Ʈ �̸����� �׿��ִ� ��Ȳ�̸� recv ���� �ʽ��ϴ�.
		if (amount < 4)
		{
			// recv ���ۿ� �����Ͱ� 0����Ʈ �����ϸ� ���� �ƹ� ��Ŷ�� ���� �������̹Ƿ� ȸ���մϴ�.
			if (amount == 0)
				continue;

			//CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n [Error] amount: %d \n\n\n\n\n\n\n\n\n\n\n", (int)amount);
			continue;
		}

		// ����
		nRecvLen = recv(ServerSocket, (CHAR*)& recvBuffer, MAX_BUFFER, 0);
		recvBuffer[nRecvLen] = '\0';


		///////////////////////////////////////////
		// recvDeque�� ������ �����͸� ����
		///////////////////////////////////////////
		if (LoadUpReceivedDataToRecvDeque(recvBuffer, nRecvLen) == false)
		{
			CONSOLE_LOG("[Error] <CClient::ClientThread()> if (LoadUpReceivedDataToRecvDeque(...) == false) \n");
			continue;
		}

		/**************************************************************************/

		char dataBuffer[MAX_BUFFER + 1];
		dataBuffer[0] = '\0'; // GetDataFromRecvDeque(...)�� �ص� ���� ����ִ� ���¸� ������ �� �� �����Ƿ� �ʱ�ȭ
		dataBuffer[MAX_BUFFER] = '\0';

		///////////////////////////////////////////
		// ������ �����͸� �����ϴ� ������ �����͸� ȹ��
		///////////////////////////////////////////
		GetDataFromRecvDeque(dataBuffer);


		///////////////////////////////////////////
		// ������ ȹ���� �����͸� ��Ŷ��� �����ϰ� ���������� ��Ŷ�� ó���մϴ�.
		///////////////////////////////////////////
		DivideDataToPacketAndProcessThePacket(dataBuffer);


		// ����Ŭ���̾�Ʈ�� �����ϸ� �����ִ� WSASend(...)�� �� ������ ���� Alertable Wait ���·� ����ϴ�.
		SleepEx(1, true);
	}
}

void CClient::CloseClient()
{
	CONSOLE_LOG("[Start] <CClient::CloseClient()>\n");


	// ����Ŭ���̾�Ʈ�� �����ϸ� �����ִ� WSASend(...)�� �� ������ ���� Alertable Wait ���·� ����ϴ�.
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


	// ������ ���� �ʱ�ȭ
	EnterCriticalSection(&csServer);
	Server = CCompletionKey();
	LeaveCriticalSection(&csServer);


	////////////////////
	// Ŭ���̾�Ʈ ������ ����
	////////////////////
	if (hMainHandle != NULL && hMainHandle != INVALID_HANDLE_VALUE)
	{
		DWORD result = WaitForSingleObject(hMainHandle, INFINITE);

		// hMainHandle�� signal�̸�
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
	// RecvDeque �ʱ�ȭ
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
	//// ��������� �ʱ�ȭ
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
		// ������ ������ ������� ���۵� ������ ����� �ٸ���
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
	// ���� ��ȿ�� ����
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

	overlappedMsg->overlapped.hEvent = &overlappedMsg; // CompletionRoutine ����̹Ƿ� overlapped.hEvent�� Ȱ��
	CopyMemory(overlappedMsg->messageBuffer, (CHAR*)finalStream.str().c_str(), finalStream.str().length());
	overlappedMsg->messageBuffer[finalStream.str().length()] = '\0';
	overlappedMsg->dataBuf.len = finalStream.str().length();
	overlappedMsg->dataBuf.buf = overlappedMsg->messageBuffer;
	overlappedMsg->sendBytes = overlappedMsg->dataBuf.len;

	//CONSOLE_LOG("[Info] <CClient::Send(...)> overlappedMsg->sendBytes: %d \n", overlappedMsg->sendBytes);


	////////////////////////////////////////////////
	// (������) ��Ŷ ������� ���� ���� ������ �Լ�
	////////////////////////////////////////////////
	VerifyPacket(overlappedMsg->messageBuffer, true);


	int nResult = WSASend(
		ServerSocket, // s: ���� ������ ����Ű�� ���� ���� ��ȣ
		&(overlappedMsg->dataBuf), // lpBuffers: WSABUF(:4300)����ü �迭�� �����ͷ� ������ WSABUF ����ü�� ���ۿ� ������ ũ�⸦ ����Ų��.
		1, // dwBufferCount: lpBuffers�� �ִ� WSABUF(:4300)����ü�� ����
		NULL, // lpNumberOfBytesSent: �Լ��� ȣ��� ���۵� �������� ����Ʈ ũ�⸦ �Ѱ��ش�. ���� �Ű� ���� lpOverlapped�� NULL�� �ƴ϶��, �� �Ű� ������ ���� NULL�� �ؾ� �Ѵ�. �׷��� (��������)�߸��� ��ȯ�� ���� �� �ִ�.
		dwFlags,// dwFlags: WSASend �Լ��� � ������� ȣ�� �Ұ������� �����Ѵ�.
		&(overlappedMsg->overlapped), // lpOverlapped: WSAOVERLAPPED(:4300)����ü�� �����ʹ�. �� (overlapped)��ø ���Ͽ����� ���õȴ�.
		SendCompletionRoutine // lpCompletionRoutine: ������ ������ �Ϸ� �Ǿ��� �� ȣ���� �Ϸ� ��ƾ (completion routine)�� ������. �� ��ø ���Ͽ����� ���� �ȴ�.
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

			/// ���������� �ݾƵ� �Ǵ��� ���� Ȯ���� �ȵǾ����ϴ�.
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
	����: Send�� �� sendStream << ������ << endl;�� ���� �� �����ʹ� '\n'�� ������ �˴ϴ�.
	�������� ����ü�� Ŭ������ �����ʹ� operator<<(...) �Լ����� << endl; ��ſ� << ' ';�� ����Ͽ� �����Ͱ� ��� �̾�����
	���������� '\n'�� ������ �ǵ��� �մϴ�.

	�۽��Ϸ��� �������� ũ�Ⱑ
	[MAX_BUFFER -6(5����Ʈ�� ��Ŷ����� ���� ���� + �������� '\0'�� �־���� �ϴ� ����)]
	���� ũ�ٸ� �����͸� �����Ͽ� �����մϴ�.

	����: ��뷮 �����ʹ� ������ Ÿ������ �����͵�θ� �����Ǿ�� �����ؼ� ������ �� �����ϴ� Ŭ���̾�Ʈ���� ���װ� �߻����� �ʽ��ϴ�.
	*/
	//CONSOLE_LOG("[Info] <CClient::SendHugePacket(...)> SendStream.str().length(): %d \n", (int)SendStream.str().length());
	if (SendStream.str().length() <= (MAX_BUFFER - 6))
	{
		// �����Ͱ� ũ�� �����Ƿ� �ٷ� �۽��մϴ�.
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
			// �̷� ��Ȳ�� �� �� �����Ƿ� �߻��ϸ� �۽����� �ʰ� �����մϴ�.
			if (i == idxOfStart)
			{
				CONSOLE_LOG("[Error] <CClient::SendHugePacket(...)> if (i == idxOfStart) \n");
				return;
			}

			// ������ ������ �߰��ϸ�
			if (SendStream.str().c_str()[i] == '\n')
			{
				idxOfEnd = i + 1;
				break;
			}
		}

		char dividedBuffer[MAX_BUFFER];
		CopyMemory(dividedBuffer, &SendStream.str().c_str()[idxOfStart], idxOfEnd - idxOfStart);
		dividedBuffer[idxOfEnd - idxOfStart] = '\0';

		// �ٽ� ������ �ʱ�ȭ
		sizeOfPacketType = typeStream.str().length();
		idxOfStart = idxOfEnd;
		idxOfEnd = idxOfStart + (MAX_BUFFER - 6 - (unsigned int)sizeOfPacketType);

		// ��Ŷ ������� �ڸ� ������ ����� ���ٸ� ����: [��ŶŸ�� ������], �߸���: [��ŶŸ�� ] �̰�
		// �����Ͱ� �� �̾��� ���̶� �ڸ� �� ������ ��Ȳ�̶� �̷� ��� �������� �ʰ� �ٷ� �����մϴ�. 
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

		// �ڸ� �����͸� �۽��մϴ�.
		Send(sendStream);
		sendCount++;
	}

	//////////////////////////////////////////////////
	// ������ �۾��� ��Ŷ�� ó��
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

		// �ڸ� �����͸� �۽��մϴ�.
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

	/* ����
	1048576B == 1024KB
	TCP���� send buffer�� recv buffer ��� 1048576 * 256���� ����.
	*/
	CONSOLE_LOG("[Start] <CClient::SetSockOpt(...)> \n");


	int optval;
	int optlen = sizeof(optval);

	// ������ 0, ���н� -1 ��ȯ
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

	// �����Ͱ� MAX_BUFFER �״�� 4096�� �� ä���� ���� ��찡 �ֱ� ������, ����ϱ� ���Ͽ� +1�� '\0' ������ ������ݴϴ�.
	char* newBuffer = new char[MAX_BUFFER + 1];
	//ZeroMemory(newBuffer, MAX_BUFFER);
	CopyMemory(newBuffer, RecvBuffer, RecvLen);
	newBuffer[RecvLen] = '\0';

	RecvDeque.push_back(newBuffer); // �ڿ� ���������� �����մϴ�.

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

	// ���� �� ������ ���� (buffer�� �� ���� �ݺ����� �������ɴϴ�.)
	while (RecvDeque.empty() == false)
	{
		// dataBuffer�� ä����� �ϴ� ����� �ִ�� MAX_BUFFER�� CopyMemory ����.
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
			// ���ۿ� ���� �ڸ� ��ŭ �� ä��ϴ�.
			idxOfStartInNextQueue = MAX_BUFFER - idxOfStartInQueue;
			CopyMemory(&DataBuffer[idxOfStartInQueue], RecvDeque.front(), idxOfStartInNextQueue);
			DataBuffer[MAX_BUFFER] = '\0';

			// dateBuffer�� �����ϰ� ���� �����͵��� �ӽ� ���ۿ� �����մϴ�. 
			int lenOfRestInNextQueue = (int)strlen(&RecvDeque.front()[idxOfStartInNextQueue]);
			char tempBuffer[MAX_BUFFER + 1];
			CopyMemory(tempBuffer, &RecvDeque.front()[idxOfStartInNextQueue], lenOfRestInNextQueue);
			tempBuffer[lenOfRestInNextQueue] = '\0';

			// �ӽ� ���ۿ� �ִ� �����͵��� �ٽ� RecvDeque.front()�� �����մϴ�.
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
	// 1. ������ ���� ���̰� 0�̸�
	/////////////////////////////////////////////
	if (strlen(DataBuffer) == 0)
	{
		//CONSOLE_LOG("\t if (strlen(DataBuffer) == 0) \n");
	}
	/////////////////////////////////////////////
	// 2. ������ ���� ���̰� 4�̸��̸�
	/////////////////////////////////////////////
	if (strlen(DataBuffer) < 4)
	{
		//CONSOLE_LOG("\t if (strlen(DataBuffer) < 4): %d \n", (int)strlen(DataBuffer));

		// DataBuffer�� ���� �����͸� newBuffer�� �����մϴ�.
		char* newBuffer = new char[MAX_BUFFER + 1];
		CopyMemory(newBuffer, &DataBuffer, strlen(DataBuffer));
		newBuffer[strlen(DataBuffer)] = '\0';

		// �ٽ� �� �պκп� �����մϴ�.
		RecvDeque.push_front(newBuffer);
	}
	/////////////////////////////////////////////
	// 3. ������ ���� ���̰� 4�̻� MAX_BUFFER + 1 �̸��̸�
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

			// ���� ���̰� 4���ϸ� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
			if ((lenOfDataBuffer - idxOfStartInPacket) < 4)
			{
				//CONSOLE_LOG("\t if (lenOfDataBuffer - idxOfStartInPacket < 4): %d \n", (int)(lenOfDataBuffer - idxOfStartInPacket));

				// DataBuffer�� ���� �����͸� remainingBuffer�� �����մϴ�.
				char* newBuffer = new char[MAX_BUFFER + 1];
				CopyMemory(newBuffer, &DataBuffer[idxOfStartInPacket], strlen(&DataBuffer[idxOfStartInPacket]));
				newBuffer[strlen(&DataBuffer[idxOfStartInPacket])] = '\0';

				// �ٽ� �� �պκп� �����մϴ�.
				RecvDeque.push_front(newBuffer);

				// �ݺ����� �����մϴ�.
				break;
			}

			char sizeBuffer[5]; // [1234\0]
			CopyMemory(sizeBuffer, &DataBuffer[idxOfStartInPacket], 4); // �� 4�ڸ� �����͸� sizeBuffer�� �����մϴ�.
			sizeBuffer[4] = '\0';

			stringstream sizeStream;
			sizeStream << sizeBuffer;
			size_t sizeOfPacket = 0;
			sizeStream >> sizeOfPacket;

			//CONSOLE_LOG("\t sizeOfPacket: %d \n", (int)sizeOfPacket);
			//CONSOLE_LOG("\t strlen(&DataBuffer[idxOfStartInPacket]): %d \n", (int)strlen(&DataBuffer[idxOfStartInPacket]));

			// �ʿ��� ������ ����� ���ۿ� ���� ������ ������� ũ�� ���� ��Ŷ�� ���� ���ŵ��� �������̹Ƿ�
			if (sizeOfPacket > strlen(&DataBuffer[idxOfStartInPacket]))
			{
				//CONSOLE_LOG("\t if (sizeOfPacket > strlen(&DataBuffer[idxOfStartInPacket])) \n");

				// DataBuffer�� ���� �����͸� remainingBuffer�� �����մϴ�.
				char* newBuffer = new char[MAX_BUFFER + 1];
				CopyMemory(newBuffer, &DataBuffer[idxOfStartInPacket], strlen(&DataBuffer[idxOfStartInPacket]));
				newBuffer[strlen(&DataBuffer[idxOfStartInPacket])] = '\0';

				// �ٽ� �� �պκп� �����մϴ�.
				RecvDeque.push_front(newBuffer);

				// �ݺ����� �����մϴ�.
				break;;
			}

			/// ���� Ȯ��
			if (sizeOfPacket == 0)
			{
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
				CONSOLE_LOG("[Error] <MainServer::WorkerThread()> sizeOfPacket: %d \n", (int)sizeOfPacket);
				CONSOLE_LOG("\n\n\n\n\n\n\n\n\n\n");
				break;
			}

			// ��Ŷ�� �ڸ��鼭 �ӽ� ���ۿ� �����մϴ�.
			char cutBuffer[MAX_BUFFER + 1];
			CopyMemory(cutBuffer, &DataBuffer[idxOfStartInPacket], sizeOfPacket);
			cutBuffer[sizeOfPacket] = '\0';


			///////////////////////////////////////////
			// (������) ��Ŷ ������� ���� ���� ������ �Լ�
			///////////////////////////////////////////
			VerifyPacket(cutBuffer, false);


			///////////////////////////////////////////
			// ��Ŷ�� ó���մϴ�.
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

	// ������ Ȯ��
	int sizeOfRecvStream = 0;
	recvStream >> sizeOfRecvStream;
	//CONSOLE_LOG("\t sizeOfRecvStream: %d \n", sizeOfRecvStream);

	// ��Ŷ ���� Ȯ��
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

	//// ex) DateStream�� ũ�� : 98
	//CONSOLE_LOG("\t DataStream size: %d\n", (int)DataStream.str().length());
	//CONSOLE_LOG("\t DataStream: %s\n", DataStream.str().c_str());


	// dataStreamLength�� ũ�� : 3 [98 ]
	stringstream dataStreamLength;
	dataStreamLength << DataStream.str().length() << endl;

	// lengthOfFinalStream�� ũ�� : 4 [101 ]
	stringstream lengthOfFinalStream;
	lengthOfFinalStream << (dataStreamLength.str().length() + DataStream.str().length()) << endl;

	// FinalStream�� ũ�� : 102 [101 DataStream]
	int sizeOfFinalStream = (int)(lengthOfFinalStream.str().length() + DataStream.str().length());
	FinalStream << sizeOfFinalStream << endl;
	FinalStream << DataStream.str(); // �̹� DataStream.str() �������� endl;�� ��������Ƿ� ���⼱ �ٽ� ������� �ʽ��ϴ�.

	//CONSOLE_LOG("\t FinalStream size: %d\n", (int)FinalStream.str().length());
	//CONSOLE_LOG("\t FinalStream: %s\n", FinalStream.str().c_str());
	

	// ������ �����Ͱ� �ִ� ���� ũ�⺸�� ũ�ų� ������ ���� �Ұ����� �˸��ϴ�.
	// messageBuffer[MAX_BUFFER];���� �������� '\0'�� �־���� �Ǳ� ������ MAX_BUFFER�� �������� �����մϴ�.
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
	CopyMemory(sizeBuffer, buffer, 4); // �� 4�ڸ� �����͸� sizeBuffer�� �����մϴ�.
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