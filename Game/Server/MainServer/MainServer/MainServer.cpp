
#include "NetworkComponent/Console.h"
#include "NetworkComponent/NetworkComponent.h"

#include "MainServer.h"

unique_ptr<class CNetworkComponent> CMainServer::Server;

unordered_map<SOCKET, CPlayerPacket> CMainServer::InfoOfClients;
CRITICAL_SECTION CMainServer::csInfoOfClients;

unordered_map<SOCKET, CGamePacket> CMainServer::InfoOfGames;
CRITICAL_SECTION CMainServer::csInfoOfGames;

CMainServer::CMainServer()
{
	// ũ��Ƽ�� ���ǿ� ���ɶ��� �ɰ� �ʱ�ȭ�� �����Ҷ����� �õ��մϴ�.
	while (InitializeCriticalSectionAndSpinCount(&csInfoOfClients, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csInfoOfGames, SPIN_COUNT) == false);

	Server = make_unique<CNetworkComponent>(ENetworkComponentType::NCT_Server);
	if (Server)
	{
		Server->RegisterConCBF(ConnectCBF);
		Server->RegisterDisconCBF(DisconnectCBF);

		Server->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::LOGIN, Login);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::CREATE_GAME, CreateGame);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::FIND_GAMES, FindGames);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::JOIN_ONLINE_GAME, JoinOnlineGame);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::DESTROY_WAITING_GAME, DestroyWaitingGame);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::EXIT_WAITING_GAME, ExitWaitingGame);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::MODIFY_WAITING_GAME, ModifyWaitingGame);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::START_WAITING_GAME, StartWaitingGame);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::ACTIVATE_GAME_SERVER, ActivateGameServer);
		Server->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::REQUEST_INFO_OF_GAME_SERVER, RequestInfoOfGameServer);
	}
}
CMainServer::~CMainServer()
{
	DeleteCriticalSection(&csInfoOfClients);
	DeleteCriticalSection(&csInfoOfGames);
}

CMainServer* CMainServer::GetSingleton()
{
	static CMainServer mainServer;
	return &mainServer;
}

void CMainServer::SetIPv4AndPort(char* IPv4, USHORT& Port)
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

bool CMainServer::Initialize(const char* const IPv4, const USHORT& Port)
{
	if (!Server)
	{
		CONSOLE_LOG("[Error] <CMyServer::Initialize(...)> if (!Server) \n");
		return false;
	}
	/****************************************/

	return Server->Initialize(IPv4, Port);
}

void CMainServer::ConnectCBF(CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CMainServer::ConnectCBF(...)> \n");

	CompletionKey.PrintInfo("\t <CMainServer::ConnectCBF(...)>");

	CONSOLE_LOG("[End] <CMainServer::ConnectCBF(...)> \n");
}
void CMainServer::DisconnectCBF(CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CMainServer::DisconnectCBF(...)> \n");

	SOCKET disconSock = CompletionKey.Socket;

	// �ش� Ŭ���̾�Ʈ�� ��Ʈ��ũ ���� ���Ḧ �ٸ� Ŭ���̾�Ʈ�鿡�� �˷��ݴϴ�.
	stringstream temp;
	ExitWaitingGame(temp, disconSock);
	DestroyWaitingGame(temp, disconSock);

	// InfoOfClients���� ����
	SOCKET leaderSocket = 0;
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(disconSock) != InfoOfClients.end())
	{
		// InfoOfGames���� ���ſ��� ����� leaderSocketByMainServer�� ȹ���մϴ�.
		leaderSocket = (SOCKET)InfoOfClients.at(disconSock).LeaderSocketByMainServer;

		/// ��Ʈ��ũ ������ ������ Ŭ���̾�Ʈ�� ������ �����մϴ�.
		CONSOLE_LOG("\t InfoOfClients.size(): %d \n", (int)InfoOfClients.size());
		InfoOfClients.erase(disconSock);
		CONSOLE_LOG("\t InfoOfClients.size(): %d \n", (int)InfoOfClients.size());
	}
	LeaveCriticalSection(&csInfoOfClients);

	// InfoOfGames���� ����
	EnterCriticalSection(&csInfoOfGames);
	// ��Ʈ��ũ ������ ������ Ŭ���̾�Ʈ�� ������ ���ӹ��� �����մϴ�.
	if (InfoOfGames.find(disconSock) != InfoOfGames.end())
	{
		CONSOLE_LOG("\t InfoOfGames.size(): %d \n", (int)InfoOfGames.size());
		InfoOfGames.erase(disconSock);
		CONSOLE_LOG("\t InfoOfGames.size(): %d \n", (int)InfoOfGames.size());
	}
	// ��Ʈ��ũ ������ ������ Ŭ���̾�Ʈ�� �Ҽӵ� ���ӹ��� ã�Ƽ� Players���� �����մϴ�.
	if (InfoOfGames.find(leaderSocket) != InfoOfGames.end())
		InfoOfGames.at(leaderSocket).Players.Remove((int)disconSock);
	LeaveCriticalSection(&csInfoOfGames);

	CONSOLE_LOG("[End] <CMainServer::DisconnectCBF(...)> \n");
}

void CMainServer::Login(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Recv by %d] <CMainServer::Login(...)> \n", (int)Socket);

	CCompletionKey completionKey;
	if (Server)
	{
		completionKey = Server->GetCompletionKey(Socket);
	}

	/// ����
	CPlayerPacket infoOfPlayer;
	RecvStream >> infoOfPlayer;

	infoOfPlayer.IPv4Addr = completionKey.IPv4Addr;
	infoOfPlayer.SocketByMainServer = (int)completionKey.Socket;
	infoOfPlayer.PortOfMainClient = completionKey.Port;
	
	EnterCriticalSection(&csInfoOfClients);
	CONSOLE_LOG("\t InfoOfClients.size(): %d \n", (int)InfoOfClients.size());
	InfoOfClients[Socket] = infoOfPlayer;
	CONSOLE_LOG("\t InfoOfClients.size(): %d \n", (int)InfoOfClients.size());
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();

	/// �۽�
	CPacket loginPacket((uint16_t)EMainPacketHeader::LOGIN);
	loginPacket.GetData() << infoOfPlayer << endl;
	if (Server) Server->Send(loginPacket, Socket);

	CONSOLE_LOG("[Send to %d] <CMainServer::Login(...)> \n\n", (int)Socket);
}
void CMainServer::CreateGame(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Recv by %d] <CMainServer::CreateGame(...)> \n", (int)Socket);

	/// ����
	CGamePacket infoOfGame;
	RecvStream >> infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	CONSOLE_LOG("\t InfoOfGames.size(): %d \n", (int)InfoOfGames.size());
	InfoOfGames[Socket] = infoOfGame;
	CONSOLE_LOG("\t InfoOfGames.size(): %d \n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();

	CONSOLE_LOG("[End] <CMainServer::CreateGame(...)> \n\n");
}
void CMainServer::FindGames(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Recv by %d] <CMainServer::FindGames(...)> \n", (int)Socket);

	/// �۽�
	CPacket findGamesPacket((uint16_t)EMainPacketHeader::FIND_GAMES);
	
	EnterCriticalSection(&csInfoOfGames);
	CONSOLE_LOG("\t InfoOfGames.size(): %d \n", (int)InfoOfGames.size());
	for (auto& kvp : InfoOfGames)
	{
		findGamesPacket.GetData() << kvp.second << endl;
		kvp.second.PrintInfo();
	}
	LeaveCriticalSection(&csInfoOfGames);

	if (Server) Server->Send(findGamesPacket, Socket);

	CONSOLE_LOG("[Send to %d] <CMainServer::FindGames(...)> \n\n", (int)Socket);
}
void CMainServer::JoinOnlineGame(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Recv by %d] <CMainServer::JoinOnlineGame(...)> \n", (int)Socket);

	/// ����
	CPlayerPacket infoOfPlayer;
	RecvStream >> infoOfPlayer;

	SOCKET leaderSocket = (SOCKET)infoOfPlayer.LeaderSocketByMainServer;

	// Ŭ���̾�Ʈ ���� ����
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		LeaveCriticalSection(&csInfoOfClients);
		CONSOLE_LOG("[Error] <CMainServer::JoinOnlineGame(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		return;
	}
	InfoOfClients.at(Socket).LeaderSocketByMainServer = infoOfPlayer.LeaderSocketByMainServer;
	infoOfPlayer = InfoOfClients.at(Socket);
	LeaveCriticalSection(&csInfoOfClients);

	// ���ӹ� ���� ����
	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(leaderSocket) == InfoOfGames.end()) // ���ӹ��� ����Ǿ��ٸ�
	{
		LeaveCriticalSection(&csInfoOfGames);
		CONSOLE_LOG("[Error] <CMainServer::JoinOnlineGame(...)> if (InfoOfGames.find(leaderSocket) == InfoOfGames.end()) \n");

		CPacket destroyWaitingGamePacket((uint16_t)EMainPacketHeader::DESTROY_WAITING_GAME);
		if (Server) Server->Send(destroyWaitingGamePacket, Socket);

		return;
	}
	else if (InfoOfGames.at(leaderSocket).Players.Size() >= 29) // �ִ� �ο��� ���Ѹ�ŭ ����� ���Դٸ� ���̻� ������ ���մϴ�.
	{
		LeaveCriticalSection(&csInfoOfGames);
		CONSOLE_LOG("[Error] <CMainServer::JoinOnlineGame(...)> if (InfoOfGames.find(leaderSocket) == InfoOfGames.end()) \n");

		CPacket destroyWaitingGamePacket((uint16_t)EMainPacketHeader::DESTROY_WAITING_GAME);
		if (Server) Server->Send(destroyWaitingGamePacket, Socket);

		return;
	}
	InfoOfGames.at(leaderSocket).Players.Add((int)Socket, infoOfPlayer);
	CGamePacket infoOfGame = InfoOfGames.at(leaderSocket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();

	/// �۽� to ����
	CPacket waitingGamePacket((uint16_t)EMainPacketHeader::WAITING_GAME);
	waitingGamePacket.GetData() << infoOfGame << endl;
	if (Server) Server->Send(waitingGamePacket, leaderSocket);

	CONSOLE_LOG("[Send to %d] <CMainServer::JoinOnlineGame(...)> \n", (int)leaderSocket);

	/// �۽� to ������ �÷��̾�� (�ش� Ŭ���̾�Ʈ ����)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		if (Server) Server->Send(waitingGamePacket, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <CMainServer::JoinOnlineGame(...)> \n", (int)kvp.first);
	}

	CONSOLE_LOG("\n");
}
void CMainServer::DestroyWaitingGame(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Recv by %d] <CMainServer::DestroyWaitingGame(...)> \n", (int)Socket);

	/// ���� by ����
	CPlayersPacket players;

	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		LeaveCriticalSection(&csInfoOfClients);
		CONSOLE_LOG("[Error] <CMainServer::DestroyWaitingGame(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		return;
	}
	// �ʱ�ȭ
	InfoOfClients.at(Socket).SocketByGameServer = 0;
	InfoOfClients.at(Socket).PortOfGameServer = 0;
	InfoOfClients.at(Socket).PortOfGameClient = 0;
	InfoOfClients.at(Socket).LeaderSocketByMainServer = 0;
	LeaveCriticalSection(&csInfoOfClients);

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		LeaveCriticalSection(&csInfoOfGames);
		CONSOLE_LOG("[Error] <CMainServer::DestroyWaitingGame(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
		return;
	}
	// ���ӹ� �÷��̾�� �ʱ�ȭ
	players = InfoOfGames.at(Socket).Players;
	for (auto& kvp : players.Players)
	{
		SOCKET socketID = (SOCKET)kvp.first;
		EnterCriticalSection(&csInfoOfClients);
		if (InfoOfClients.find(socketID) != InfoOfClients.end())
		{
			InfoOfClients.at(socketID).SocketByGameServer = 0;
			InfoOfClients.at(socketID).PortOfGameServer = 0;
			InfoOfClients.at(socketID).PortOfGameClient = 0;
			InfoOfClients.at(socketID).LeaderSocketByMainServer = 0;
		}
		LeaveCriticalSection(&csInfoOfClients);
	}

	// ���ӹ� ����
	CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames.erase(Socket);
	CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);

	/// �۽� to �÷��̾��(���� ����)
	CPacket destroyWaitingGamePacket((uint16_t)EMainPacketHeader::DESTROY_WAITING_GAME);
	for (auto& kvp : players.Players)
	{
		if (Server) Server->Send(destroyWaitingGamePacket, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <CMainServer::DestroyWaitingGame(...)> \n", (int)kvp.first);
	}

	CONSOLE_LOG("\n");
}
void CMainServer::ExitWaitingGame(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Recv by %d] <CMainServer::ExitWaitingGame(...)> \n", (int)Socket);

	/// ����
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		LeaveCriticalSection(&csInfoOfClients);
		CONSOLE_LOG("[Error] <CMainServer::ExitWaitingGame(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		return;
	}
	SOCKET leaderSocket = (SOCKET)InfoOfClients.at(Socket).LeaderSocketByMainServer;
	// �ʱ�ȭ
	InfoOfClients.at(Socket).SocketByGameServer = 0;
	InfoOfClients.at(Socket).PortOfGameServer = 0;
	InfoOfClients.at(Socket).PortOfGameClient = 0;
	InfoOfClients.at(Socket).LeaderSocketByMainServer = 0;
	LeaveCriticalSection(&csInfoOfClients);

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(leaderSocket) == InfoOfGames.end())
	{
		LeaveCriticalSection(&csInfoOfGames);
		CONSOLE_LOG("[Error] <CMainServer::ExitWaitingGame(...)> if (InfoOfGames.find(leaderSocket) == InfoOfGames.end()) \n");
		return;
	}
	CONSOLE_LOG("\t Players.Size(): %d", (int)InfoOfGames.at(leaderSocket).Players.Size());
	InfoOfGames.at(leaderSocket).Players.Remove((int)Socket);
	CONSOLE_LOG("\t Players.Size(): %d", (int)InfoOfGames.at(leaderSocket).Players.Size());

	CGamePacket infoOfGame = InfoOfGames.at(leaderSocket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();

	/// �۽� to ����
	CPacket waitingGamePacket((uint16_t)EMainPacketHeader::WAITING_GAME);
	waitingGamePacket.GetData() << infoOfGame << endl;
	if (Server) Server->Send(waitingGamePacket, leaderSocket);

	CONSOLE_LOG("[Send to %d] <CMainServer::ExitWaitingGame(...)> \n", (int)leaderSocket);

	/// �۽� to ������ �÷��̾�� (�ش� Ŭ���̾�Ʈ ����)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		if (Server) Server->Send(waitingGamePacket, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <CMainServer::ExitWaitingGame(...)> \n", (int)kvp.first);
	}

	CONSOLE_LOG("\n");
}
void CMainServer::ModifyWaitingGame(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Recv by %d] <CMainServer::ModifyWaitingGame(...)> \n", (int)Socket);

	/// ����
	CGamePacket infoOfGame;
	RecvStream >> infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		LeaveCriticalSection(&csInfoOfGames);
		CONSOLE_LOG("[Error] <CMainServer::ModifyWaitingGame(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
		return;
	}
	// �� ����
	InfoOfGames.at(Socket).Title = infoOfGame.Title;
	InfoOfGames.at(Socket).Stage = infoOfGame.Stage;
	InfoOfGames.at(Socket).nMax = infoOfGame.nMax;

	// �۽��� ���� �ٽ� ����
	infoOfGame = InfoOfGames.at(Socket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();

	/// �۽� to �÷��̾��(���� ����)
	CPacket modifyWaitingGamePacket((uint16_t)EMainPacketHeader::MODIFY_WAITING_GAME);
	modifyWaitingGamePacket.GetData() << infoOfGame << endl;

	for (const auto& kvp : infoOfGame.Players.Players)
	{
		if (Server) Server->Send(modifyWaitingGamePacket, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <CMainServer::ModifyWaitingGame(...)> \n", (int)kvp.first);
	}

	CONSOLE_LOG("\n");
}
void CMainServer::StartWaitingGame(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Recv by %d] <CMainServer::StartWaitingGame(...)> \n", (int)Socket);

	/// ����
	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		LeaveCriticalSection(&csInfoOfGames);
		CONSOLE_LOG("[Error] <CMainServer::StartWaitingGame(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
		return;
	}
	// �۽��� ���� ����
	CGamePacket infoOfGame = InfoOfGames.at(Socket);
	LeaveCriticalSection(&csInfoOfGames);

	/// �۽� to �÷��̾��(���� ����)
	CPacket startWaitingGamePacket((uint16_t)EMainPacketHeader::START_WAITING_GAME);

	for (const auto& kvp : infoOfGame.Players.Players)
	{
		if (Server) Server->Send(startWaitingGamePacket, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <CMainServer::StartWaitingGame(...)> \n", (int)kvp.first);
	}

	CONSOLE_LOG("\n");
}

void CMainServer::ActivateGameServer(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Recv by %d] <CMainServer::ActivateGameServer(...)> \n", (int)Socket);

	/// ����
	CPlayerPacket infoOfPlayer;
	RecvStream >> infoOfPlayer;

	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		LeaveCriticalSection(&csInfoOfClients);
		CONSOLE_LOG("[Error] <CMainServer::ActivateGameServer(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		return;
	}
	InfoOfClients.at(Socket).PortOfGameServer = infoOfPlayer.PortOfGameServer;
	LeaveCriticalSection(&csInfoOfClients);

	CGamePacket infoOfGame;
	RecvStream >> infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		LeaveCriticalSection(&csInfoOfGames);
		CONSOLE_LOG("[Error] <CMainServer::ActivateGameServer(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
		return;
	}
	InfoOfGames.at(Socket).State = infoOfGame.State;
	InfoOfGames.at(Socket).Leader.PortOfGameServer = infoOfPlayer.PortOfGameServer;
	infoOfGame = InfoOfGames.at(Socket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfPlayer.PrintInfo();

	/// �۽� to ����
	CPacket waitingGamePacket((uint16_t)EMainPacketHeader::WAITING_GAME);
	waitingGamePacket.GetData() << infoOfGame << endl;
	if (Server) Server->Send(waitingGamePacket, Socket);

	CONSOLE_LOG("[Send to %d] <CMainServer::ActivateGameServer(...)> \n", (int)Socket);

	/// �۽� to ������ �÷��̾�� (�ش� Ŭ���̾�Ʈ ����)
	for (const auto& kvp : infoOfGame.Players.Players)
	{
		if (Server) Server->Send(waitingGamePacket, (SOCKET)kvp.first);

		CONSOLE_LOG("[Send to %d] <CMainServer::ActivateGameServer(...)> \n", (int)kvp.first);
	}

	CONSOLE_LOG("\n");
}
void CMainServer::RequestInfoOfGameServer(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Recv by %d] <CMainServer::RequestInfoOfGameServer(...)> \n", (int)Socket);

	/// ����
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		LeaveCriticalSection(&csInfoOfClients);
		CONSOLE_LOG("[Error] <CMainServer::RequestInfoOfGameServer(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		return;
	}
	SOCKET leaderSocket = (SOCKET)InfoOfClients.at(Socket).LeaderSocketByMainServer;
	CONSOLE_LOG("\t <CMainServer::RequestInfoOfGameServer(...)> leaderSocket: %d\n", (int)leaderSocket);

	if (InfoOfClients.find(leaderSocket) == InfoOfClients.end())
	{
		LeaveCriticalSection(&csInfoOfClients);
		CONSOLE_LOG("[Error] <CMainServer::RequestInfoOfGameServer(...)> if (InfoOfClients.find(leaderSocket) == InfoOfClients.end()) \n");
		return;
	}
	CPlayerPacket infoOfPlayer = InfoOfClients.at(leaderSocket);
	LeaveCriticalSection(&csInfoOfClients);

	infoOfPlayer.PrintInfo();

	// ���� ���� ������ �������� �ʾҴٸ� �۽����� �ʽ��ϴ�.
	if (infoOfPlayer.PortOfGameServer <= 0)
	{
		CONSOLE_LOG("[Error] <CMainServer::RequestInfoOfGameServer(...)> if (infoOfPlayer.PortOfGameServer <= 0) \n");
		return;
	}

	/// �۽�
	CPacket requestInfoOfGameServerPacket((uint16_t)EMainPacketHeader::REQUEST_INFO_OF_GAME_SERVER);
	requestInfoOfGameServerPacket.GetData() << infoOfPlayer << endl;
	if (Server) Server->Send(requestInfoOfGameServerPacket, Socket);

	CONSOLE_LOG("[Send to %d] <CMainServer::RequestInfoOfGameServer(...)> \n", (int)Socket);
}
