#pragma once

#include "Packet.h"

using namespace std;

// ��Ŷ ó�� �Լ� ������
struct FuncProcess
{
	// RecvStream�� ������ ����, pSocket�� Overlapped I/O �۾��� �߻��� IOCP ���� ����ü ����
	void(*funcProcessPacket)(stringstream& RecvStream, stSOCKETINFO* pSocketInfo);
	FuncProcess()
	{
		funcProcessPacket = nullptr;
	}
};

class IocpServerBase
{
protected:
	stSOCKETINFO*	SocketInfo = nullptr;	 // ���� ����, �����Ҵ��ϰ� Clients�� �Ѱ��ִ� �뵵
	SOCKET			ListenSocket;			 // ���� ���� ����
	HANDLE			hIOCP;					 // IOCP ��ü �ڵ�

	bool			bAccept;				 // ��û ���� �÷���

	bool			bWorkerThread;			 // �۾� ������ ���� �÷���
	HANDLE*			hWorkerHandle = nullptr; // �۾� ������ �ڵ�		
	DWORD			nThreadCnt;				 // �۾� ������ ����

public:
	// WSAAccept�� ��� Ŭ���̾�Ʈ�� new stSOCKETINFO()�� ����
	static std::map<SOCKET, stSOCKETINFO*> Clients;
	static CRITICAL_SECTION csClients;

public:
	IocpServerBase();
	virtual ~IocpServerBase();

	// ���� ��� �� ���� ���� ����
	bool Initialize();

	// ���� ����
	virtual void StartServer();

	// �۾� ������ ����
	virtual bool CreateWorkerThread();

	// �۾� ������
	virtual void WorkerThread();

	// Ŭ���̾�Ʈ ���� ����
	virtual void CloseSocket(stSOCKETINFO* pSocketInfo);

	// Ŭ���̾�Ʈ���� �۽�
	virtual void Send(stringstream& SendStream, stSOCKETINFO* pSocketInfo);

	// Ŭ���̾�Ʈ ���� ���
	virtual void Recv(stSOCKETINFO* pSocketInfo);

	///////////////////////////////////////////
	// stringstream�� �� �տ� size�� �߰�
	///////////////////////////////////////////
	static void AddSizeInStream(stringstream& DataStream, stringstream& FinalStream)
	{
		if (DataStream.str().length() == 0)
		{
			printf_s("[ERROR] <AddSizeInStream(...)> if (DataStream.str().length() == 0) \n");
			return;
		}
		printf_s("[START] <AddSizeInStream(...)> \n");

		// ex) DateStream�� ũ�� : 98
		printf_s("\t DataStream size: %d\n", (int)DataStream.str().length());
		printf_s("\t DataStream: %s\n", DataStream.str().c_str());

		// dataStreamLength�� ũ�� : 3 [98 ]
		stringstream dataStreamLength;
		dataStreamLength << DataStream.str().length() << endl;

		// lengthOfFinalStream�� ũ�� : 4 [101 ]
		stringstream lengthOfFinalStream;
		lengthOfFinalStream << (dataStreamLength.str().length() + DataStream.str().length()) << endl;

		// FinalStream�� ũ�� : 101 [101 DataStream]
		int sizeOfFinalStream = (int)(lengthOfFinalStream.str().length() + DataStream.str().length());
		FinalStream << sizeOfFinalStream << endl;
		FinalStream << DataStream.str(); // �̹� DataStream.str() �������� endl;�� ��������Ƿ� ���⼱ �ٽ� ������� �ʽ��ϴ�.

		printf_s("\t FinalStream size: %d\n", (int)FinalStream.str().length());
		printf_s("\t FinalStream: %s\n", FinalStream.str().c_str());


		printf_s("[END] <AddSizeInStream(...)> \n");
	}
};