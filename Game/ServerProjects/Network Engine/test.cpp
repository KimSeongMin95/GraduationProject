#include "pch.h"

#include "test.h"

#include "NetworkComponent.h"


test::test()
{
	Server = new CNetworkComponent(ENetworkComponentType::NCT_Server);
	Client = new CNetworkComponent(ENetworkComponentType::NCT_Client);
}
test::~test()
{
	if (Server)
		delete Server;
	if (Client)
		delete Client;
}


void test::SampleServer(CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <test::SampleServer(...)>\n", (int)Socket);

	CCompletionKey completionKey = NC->GetCompletionKey(Socket);


	/// 수신
	int idx;
	RecvStream >> idx;
	CONSOLE_LOG("\t idx: %d \n", idx);
	idx++;


	/// 송신
	stringstream sendStream;
	sendStream << CPacket::GetNumberOfType("Sample") << endl;
	sendStream << idx << endl;

	NC->SendHugePacket(sendStream, Socket);


	CONSOLE_LOG("[Send to %d] <test::SampleServer(...)>\n\n", (int)Socket);
}
void test::SampleClient(CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket /*= 0*/)
{
	CONSOLE_LOG("[Start] <test::SampleClient(...)>\n");

	
	/// 수신
	int idx;
	RecvStream >> idx;
	CONSOLE_LOG("\t idx: %d \n", idx);
	idx++;


	CONSOLE_LOG("[End] <test::SampleClient(...)>\n");
}
void test::Send()
{
	/// 송신
	stringstream sendStream;
	sendStream << CPacket::GetNumberOfType("Sample") << endl;
	sendStream << 111 << endl;

	Client->Send(sendStream);
}


void test::SampleHugeServer(CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket)
{
	CONSOLE_LOG("[Recv by %d] <test::SampleHugeServer(...)>\n", (int)Socket);


	/// 수신


	/// 송신
	stringstream sendStream;
	sendStream << CPacket::GetNumberOfType("SampleHuge") << endl;

	/// 대용량 테스트
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
	for (int i = 0; i < 10000; i++)
	{
		sendStream << str[rand() % 12] << endl;
	}

	/// 큰 데이터 테스트
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

	NC->SendHugePacket(sendStream, Socket);


	CONSOLE_LOG("[Send to %d] <test::SampleHugeServer(...)>\n\n", (int)Socket);
}

void test::SampleHugeClient(CNetworkComponent* NC, stringstream& RecvStream, SOCKET Socket /*= 0*/)
{
	CONSOLE_LOG("[Start] <test::SampleHugeClient(...)>\n");


	/// 수신
	//string str;
	//while (RecvStream >> str)
	//{
	//	CONSOLE_LOG("%s\n", str.c_str());
	//}
	CONSOLE_LOG("[Info] <test::SampleHugeClient(...)> RecvStream.str().length(): %d \n", RecvStream.str().length());


	CONSOLE_LOG("[End] <test::SampleHugeClient(...)>\n");
}

void test::SendHuge()
{
	/// 송신
	stringstream sendStream;
	sendStream << CPacket::GetNumberOfType("SampleHuge") << endl;

	Client->Send(sendStream);
}