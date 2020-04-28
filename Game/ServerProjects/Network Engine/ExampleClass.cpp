#include "pch.h"

#include "ExampleClass.h"

#include "NetworkComponent.h"


/////////////////////
// �⺻
/////////////////////
CExampleClass::CExampleClass()
{
	// ���� ����
	Server = new CNetworkComponent(ENetworkComponentType::NCT_Server);
	if (Server)
	{	
		Server->RegisterCBF(ConnectCBF, DisconnectCBF);

		Server->RegisterTypeAndStaticFunc("Sample", SampleServer);
		Server->RegisterTypeAndStaticFunc("SampleHuge", SampleHugeServer);

		CONSOLE_LOG("Server->GetNumberOfType Sample %d \n", Server->GetNumberOfType("Sample"));
		CONSOLE_LOG("Server->GetNumberOfType SampleHuge %d \n", Server->GetNumberOfType("SampleHuge"));

		while (Server->Initialize("127.0.0.1", 8000) == false);
	}

	// Ŭ���̾�Ʈ ����
	for (int i = 0; i < TEST_MAX_CLIENT; i++)
	{
		Clients[i] = new CNetworkComponent(ENetworkComponentType::NCT_Client);
		if (Clients[i])
		{
			Clients[i]->RegisterTypeAndStaticFunc("Sample", SampleClient);
			Clients[i]->RegisterTypeAndStaticFunc("SampleHuge", SampleHugeClient);

			CONSOLE_LOG("Clients[i]->GetNumberOfType Sample %d \n", Clients[i]->GetNumberOfType("Sample"));
			CONSOLE_LOG("Clients[i]->GetNumberOfType SampleHuge %d \n", Clients[i]->GetNumberOfType("SampleHuge"));

			while (Clients[i]->Initialize("127.0.0.1", 8000) == false);
		}
	}
}
CExampleClass::~CExampleClass()
{
	if (Server)
	{
		delete Server;
		Server = nullptr;
	}
	for (int i = 0; i < TEST_MAX_CLIENT; i++)
	{
		if (Clients[i])
		{
			delete Clients[i];
			Clients[i] = nullptr;
		}
	}
}


////////////////////////
// NetworkComponent
////////////////////////
void CExampleClass::ConnectCBF(class CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CExampleClass::ConnectCBF(...)> \n");


	CompletionKey.PrintInfo();


	CONSOLE_LOG("[End] <CExampleClass::ConnectCBF(...)> \n");
}
void CExampleClass::DisconnectCBF(class CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CExampleClass::DisconnectCBF(...)> \n");


	CompletionKey.PrintInfo();


	CONSOLE_LOG("[End] <CExampleClass::DisconnectCBF(...)> \n");
}

void CExampleClass::SampleServer(CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <CExampleClass::SampleServer(...)>\n", (int)Socket);


	NC->GetCompletionKey(Socket).PrintInfo();


	/// ����
	int idx;
	RecvStream >> idx;
	CONSOLE_LOG("\t idx: %d \n", idx);
	idx++;


	/// �۽�
	stringstream sendStream;
	sendStream << NC->GetNumberOfType("Sample") << endl;
	sendStream << idx << endl;

	NC->SendHugeData(sendStream, Socket);


	CONSOLE_LOG("[Send to %d] <CExampleClass::SampleServer(...)>\n\n", (int)Socket);
}
void CExampleClass::SampleClient(CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket /*= 0*/)
{
	CONSOLE_LOG("[Start] <CExampleClass::SampleClient(...)>\n");

	
	/// ����
	int idx;
	RecvStream >> idx;
	CONSOLE_LOG("\t idx: %d \n", idx);
	idx++;


	CONSOLE_LOG("[End] <CExampleClass::SampleClient(...)>\n");
}
void CExampleClass::Send()
{
	/// �۽�
	for (int i = 0; i < TEST_MAX_CLIENT; i++)
	{
		stringstream sendStream;
		sendStream << Clients[i]->GetNumberOfType("Sample") << endl;
		sendStream << 111 << endl;

		Clients[i]->Send(sendStream);
	}
}


void CExampleClass::SampleHugeServer(CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <CExampleClass::SampleHugeServer(...)>\n", (int)Socket);


	/// ����


	/// �۽�
	stringstream sendStream;
	sendStream << NC->GetNumberOfType("SampleHuge") << endl;

	/// ��뷮 �׽�Ʈ
	const char* str[12] =
	{
		{ "1" },
		{ "22" },
		{ "333" },
		{ "4444" },
		{ "55555" },
		{ "666666" },
		{ "7777777" },
		{ "88888888" },
		{ "999999999" },
		{ "0000000000" },
		{ "Test Hi Hellow Nice to meet you!" },
		{ "Hi Every one." }
	};

	srand((unsigned int)time(NULL));
	for (int i = 0; i < 1000; i++)
	{
		sendStream << str[rand() % 12] << endl;
	}

	/// ū ������ �׽�Ʈ
	//char c = '0';
	//for (int i = 0; i < 4087; i++)
	//{
	//	sendStream << c;

	//	if (c == '9')
	//		c = '0';
	//	else
	//		c++;
	//}
	//sendStream << endl;

	//for (int i = 0; i < 4087; i++)
	//{
	//	sendStream << c;

	//	if (c == '9')
	//		c = '0';
	//	else
	//		c++;
	//}
	//sendStream << endl;

	//for (int i = 0; i < 4087; i++)
	//{
	//	sendStream << c;

	//	if (c == '9')
	//		c = '0';
	//	else
	//		c++;
	//}
	//sendStream << endl;

	NC->SendHugeData(sendStream, Socket);


	CONSOLE_LOG("[Send to %d] <CExampleClass::SampleHugeServer(...)>\n\n", (int)Socket);
}

void CExampleClass::SampleHugeClient(CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket /*= 0*/)
{
	CONSOLE_LOG("[Start] <CExampleClass::SampleHugeClient(...)>\n");


	/// ����
	//string str;
	//while (RecvStream >> str)
	//{
	//	CONSOLE_LOG("%s\n", str.c_str());
	//}
	CONSOLE_LOG("\t RecvStream.str().length(): %d \n", RecvStream.str().length());


	CONSOLE_LOG("[End] <CExampleClass::SampleHugeClient(...)>\n");
}

void CExampleClass::SendHuge()
{
	/// �۽�
	for (int i = 0; i < TEST_MAX_CLIENT; i++)
	{
		stringstream sendStream;
		sendStream << Clients[i]->GetNumberOfType("SampleHuge") << endl;

		Clients[i]->Send(sendStream);
	}
}