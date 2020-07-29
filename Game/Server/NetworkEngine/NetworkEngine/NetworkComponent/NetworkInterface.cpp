
#include "Console.h"

#include "NetworkInterface.h"

void INetworkInterface::CloseSocketWithCheck(SOCKET& Socket)
{
	if (Socket != NULL && Socket != INVALID_SOCKET)
	{
		closesocket(Socket);
		Socket = NULL;
	}
}
void INetworkInterface::CloseSocketWithCheck(const SOCKET& Socket)
{
	if (Socket != NULL && Socket != INVALID_SOCKET)
	{
		closesocket(Socket);
	}
}

void INetworkInterface::CloseSocketAndWSACleanup(SOCKET& Socket)
{
	CloseSocketWithCheck(Socket);
	WSACleanup();
}

void INetworkInterface::CloseHandleWithCheck(HANDLE& Handle)
{
	if (Handle != NULL && Handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(Handle);
		Handle = NULL;
	}
}

void INetworkInterface::SetSockOpt(const SOCKET& Socket, const int& SizeOfSendBuf, const int& SizeOfRecvBuf)
{
	/* ����
	The maximum send buffer size is 1,048,576 bytes.
	The default value of the SO_SNDBUF option is 32,767.
	For a TCP socket, the maximum length that you can specify is 1 GB.
	For a UDP or RAW socket, the maximum length that you can specify is the smaller of the following values:
	65,535 bytes (for a UDP socket) or 32,767 bytes (for a RAW socket).
	The send buffer size defined by the SO_SNDBUF option.
	*/

	/* ����
	1048576B == 1024KB
	TCP���� send buffer�� recv buffer ��� 1048576 * 256���� �����մϴ�.
	*/
	CONSOLE_LOG_NETWORK("[Start] <SetSockOpt(...)> \n");

	int optval;
	int optlen = sizeof(optval);

	// ������ 0�� ���н� -1�� ��ȯ�մϴ�.
	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)&optval, &optlen) == 0)
	{
		CONSOLE_LOG_NETWORK("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)&optval, &optlen) == 0)
	{
		CONSOLE_LOG_NETWORK("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
	}

	optval = SizeOfSendBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)&optval, sizeof(optval)) == 0)
	{
		CONSOLE_LOG_NETWORK("\t Socket: %d, setsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
	}
	optval = SizeOfRecvBuf;
	if (setsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)&optval, sizeof(optval)) == 0)
	{
		CONSOLE_LOG_NETWORK("\t Socket: %d, setsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
	}

	if (getsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)&optval, &optlen) == 0)
	{
		CONSOLE_LOG_NETWORK("\t Socket: %d, getsockopt SO_SNDBUF: %d \n", (int)Socket, optval);
	}
	if (getsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)&optval, &optlen) == 0)
	{
		CONSOLE_LOG_NETWORK("\t Socket: %d, getsockopt SO_RCVBUF: %d \n", (int)Socket, optval);
	}

	CONSOLE_LOG_NETWORK("[End] <SetSockOpt(...)> \n");
}

void INetworkInterface::SetIPv4AndPort(char* IPv4, USHORT& Port)
{
	CONSOLE_LOG("\n /*********************************************/ \n");

	CONSOLE_LOG("IPv4�� �Է��ϼ���. (����: 58.125.236.74) \n");
	CONSOLE_LOG("IPv4: ");
	std::cin >> IPv4;
	CONSOLE_LOG("�Է¹��� IPv4: %s \n", IPv4);

	CONSOLE_LOG("Port�� �Է��ϼ���. (����: 8000) \n");
	CONSOLE_LOG("Port: ");
	std::cin >> Port;
	CONSOLE_LOG("�Է¹��� Port: %d \n", Port);

	CONSOLE_LOG("/*********************************************/ \n\n");
}

void INetworkInterface::GetKoreaStandardTime(stringstream& TimeStream)
{
	time_t rawTime;
	time(&rawTime);

	//tm gmt_tm;
	//gmtime_s(&gmt_tm, &rawTime);
	//str << "[GMT(����ǥ�ؽð�): ";
	//str << (gmt_tm.tm_year + 1900) << "�� ";
	//str << (gmt_tm.tm_mon + 1) << "�� ";
	//str << gmt_tm.tm_mday << "�� ";
	//str << gmt_tm.tm_hour << "�� ";
	//str << gmt_tm.tm_min << "�� ";
	//str << gmt_tm.tm_sec << "��]" << endl;

	tm kst_tm;
	localtime_s(&kst_tm, &rawTime);
	//TimeStream << "[KST(�ѱ��ð�): ";
	TimeStream << (kst_tm.tm_year + 1900) << "�⵵ ";
	TimeStream << (kst_tm.tm_mon + 1) << "�� ";
	TimeStream << kst_tm.tm_mday << "�� ";
	TimeStream << kst_tm.tm_hour << "�� ";
	TimeStream << kst_tm.tm_min << "�� ";
	TimeStream << kst_tm.tm_sec << "�� ";
}


CProcessingFuncPtr::CProcessingFuncPtr()
{
	Func = nullptr;
}

void CProcessingFuncPtr::SetFunc(void(*StaticFunc)(stringstream&, const SOCKET&))
{
	this->Func = StaticFunc;
}
void CProcessingFuncPtr::ExecuteFunc(stringstream& RecvStream, const SOCKET& Socket)
{
	if (!Func)
		return;

	Func(RecvStream, Socket);
}


CCallBackFuncPtr::CCallBackFuncPtr()
{
	Func = nullptr;
}

void CCallBackFuncPtr::SetFunc(void(*StaticCBF)(CCompletionKey))
{
	this->Func = StaticCBF;
}
void CCallBackFuncPtr::ExecuteFunc(CCompletionKey CompletionKey)
{
	if (!Func)
		return;

	Func(CompletionKey);
}
