
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
	// 크리티컬 섹션에 스핀락을 걸고 초기화에 성공할때까지 시도합니다.
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

	CONSOLE_LOG("IPv4를 입력하세요. (예시: 58.125.236.74) \n");
	CONSOLE_LOG("IPv4: ");
	std::cin >> IPv4;
	CONSOLE_LOG("입력받은 IPv4: %s \n", IPv4);

	CONSOLE_LOG("Port를 입력하세요. (예시: 8000) \n");
	CONSOLE_LOG("Port: ");
	std::cin >> Port;
	CONSOLE_LOG("입력받은 Port: %d \n", Port);

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

	// 해당 클라이언트의 네트워크 접속 종료를 다른 클라이언트들에게 알려줍니다.
	stringstream temp;
	ExitWaitingGame(temp, disconSock);
	DestroyWaitingGame(temp, disconSock);

	// InfoOfClients에서 제거
	SOCKET leaderSocket = 0;
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(disconSock) != InfoOfClients.end())
	{
		// InfoOfGames에서 제거에서 사용할 leaderSocketByMainServer를 획득합니다.
		leaderSocket = (SOCKET)InfoOfClients.at(disconSock).LeaderSocketByMainServer;

		/// 네트워크 연결을 종료한 클라이언트의 정보를 제거합니다.
		CONSOLE_LOG("\t InfoOfClients.size(): %d \n", (int)InfoOfClients.size());
		InfoOfClients.erase(disconSock);
		CONSOLE_LOG("\t InfoOfClients.size(): %d \n", (int)InfoOfClients.size());
	}
	LeaveCriticalSection(&csInfoOfClients);

	// InfoOfGames에서 제거
	EnterCriticalSection(&csInfoOfGames);
	// 네트워크 연결을 종료한 클라이언트가 생성한 게임방을 제거합니다.
	if (InfoOfGames.find(disconSock) != InfoOfGames.end())
	{
		CONSOLE_LOG("\t InfoOfGames.size(): %d \n", (int)InfoOfGames.size());
		InfoOfGames.erase(disconSock);
		CONSOLE_LOG("\t InfoOfGames.size(): %d \n", (int)InfoOfGames.size());
	}
	// 네트워크 연결을 종료한 클라이언트가 소속된 게임방을 찾아서 Players에서 제거합니다.
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

	/// 수신
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

	/// 송신
	CPacket loginPacket((uint16_t)EMainPacketHeader::LOGIN);
	loginPacket.GetData() << infoOfPlayer << endl;
	if (Server) Server->Send(loginPacket, Socket);

	CONSOLE_LOG("[Send to %d] <CMainServer::Login(...)> \n\n", (int)Socket);
}
void CMainServer::CreateGame(stringstream& RecvStream, const SOCKET& Socket)
{
	CONSOLE_LOG("[Recv by %d] <CMainServer::CreateGame(...)> \n", (int)Socket);

	/// 수신
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

	/// 송신
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

	/// 수신
	CPlayerPacket infoOfPlayer;
	RecvStream >> infoOfPlayer;

	SOCKET leaderSocket = (SOCKET)infoOfPlayer.LeaderSocketByMainServer;

	// 클라이언트 정보 적용
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

	// 게임방 정보 적용
	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(leaderSocket) == InfoOfGames.end()) // 게임방이 종료되었다면
	{
		LeaveCriticalSection(&csInfoOfGames);
		CONSOLE_LOG("[Error] <CMainServer::JoinOnlineGame(...)> if (InfoOfGames.find(leaderSocket) == InfoOfGames.end()) \n");

		CPacket destroyWaitingGamePacket((uint16_t)EMainPacketHeader::DESTROY_WAITING_GAME);
		if (Server) Server->Send(destroyWaitingGamePacket, Socket);

		return;
	}
	else if (InfoOfGames.at(leaderSocket).Players.Size() >= 29) // 최대 인원수 제한만큼 사람이 들어왔다면 더이상 들어오지 못합니다.
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

	/// 송신 to 방장
	CPacket waitingGamePacket((uint16_t)EMainPacketHeader::WAITING_GAME);
	waitingGamePacket.GetData() << infoOfGame << endl;
	if (Server) Server->Send(waitingGamePacket, leaderSocket);

	CONSOLE_LOG("[Send to %d] <CMainServer::JoinOnlineGame(...)> \n", (int)leaderSocket);

	/// 송신 to 대기방의 플레이어들 (해당 클라이언트 포함)
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

	/// 수신 by 방장
	CPlayersPacket players;

	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		LeaveCriticalSection(&csInfoOfClients);
		CONSOLE_LOG("[Error] <CMainServer::DestroyWaitingGame(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		return;
	}
	// 초기화
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
	// 게임방 플레이어들 초기화
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

	// 게임방 삭제
	CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	InfoOfGames.erase(Socket);
	CONSOLE_LOG("\t InfoOfGames.size(): %d\n", (int)InfoOfGames.size());
	LeaveCriticalSection(&csInfoOfGames);

	/// 송신 to 플레이어들(방장 제외)
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

	/// 수신
	EnterCriticalSection(&csInfoOfClients);
	if (InfoOfClients.find(Socket) == InfoOfClients.end())
	{
		LeaveCriticalSection(&csInfoOfClients);
		CONSOLE_LOG("[Error] <CMainServer::ExitWaitingGame(...)> if (InfoOfClients.find(Socket) == InfoOfClients.end()) \n");
		return;
	}
	SOCKET leaderSocket = (SOCKET)InfoOfClients.at(Socket).LeaderSocketByMainServer;
	// 초기화
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

	/// 송신 to 방장
	CPacket waitingGamePacket((uint16_t)EMainPacketHeader::WAITING_GAME);
	waitingGamePacket.GetData() << infoOfGame << endl;
	if (Server) Server->Send(waitingGamePacket, leaderSocket);

	CONSOLE_LOG("[Send to %d] <CMainServer::ExitWaitingGame(...)> \n", (int)leaderSocket);

	/// 송신 to 대기방의 플레이어들 (해당 클라이언트 포함)
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

	/// 수신
	CGamePacket infoOfGame;
	RecvStream >> infoOfGame;

	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		LeaveCriticalSection(&csInfoOfGames);
		CONSOLE_LOG("[Error] <CMainServer::ModifyWaitingGame(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
		return;
	}
	// 값 대입
	InfoOfGames.at(Socket).Title = infoOfGame.Title;
	InfoOfGames.at(Socket).Stage = infoOfGame.Stage;
	InfoOfGames.at(Socket).nMax = infoOfGame.nMax;

	// 송신을 위해 다시 복사
	infoOfGame = InfoOfGames.at(Socket);
	LeaveCriticalSection(&csInfoOfGames);

	infoOfGame.PrintInfo();

	/// 송신 to 플레이어들(방장 제외)
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

	/// 수신
	EnterCriticalSection(&csInfoOfGames);
	if (InfoOfGames.find(Socket) == InfoOfGames.end())
	{
		LeaveCriticalSection(&csInfoOfGames);
		CONSOLE_LOG("[Error] <CMainServer::StartWaitingGame(...)> if (InfoOfGames.find(Socket) == InfoOfGames.end()) \n");
		return;
	}
	// 송신을 위해 복사
	CGamePacket infoOfGame = InfoOfGames.at(Socket);
	LeaveCriticalSection(&csInfoOfGames);

	/// 송신 to 플레이어들(방장 제외)
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

	/// 수신
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

	/// 송신 to 방장
	CPacket waitingGamePacket((uint16_t)EMainPacketHeader::WAITING_GAME);
	waitingGamePacket.GetData() << infoOfGame << endl;
	if (Server) Server->Send(waitingGamePacket, Socket);

	CONSOLE_LOG("[Send to %d] <CMainServer::ActivateGameServer(...)> \n", (int)Socket);

	/// 송신 to 대기방의 플레이어들 (해당 클라이언트 포함)
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

	/// 수신
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

	// 아직 게임 서버가 구동되지 않았다면 송신하지 않습니다.
	if (infoOfPlayer.PortOfGameServer <= 0)
	{
		CONSOLE_LOG("[Error] <CMainServer::RequestInfoOfGameServer(...)> if (infoOfPlayer.PortOfGameServer <= 0) \n");
		return;
	}

	/// 송신
	CPacket requestInfoOfGameServerPacket((uint16_t)EMainPacketHeader::REQUEST_INFO_OF_GAME_SERVER);
	requestInfoOfGameServerPacket.GetData() << infoOfPlayer << endl;
	if (Server) Server->Send(requestInfoOfGameServerPacket, Socket);

	CONSOLE_LOG("[Send to %d] <CMainServer::RequestInfoOfGameServer(...)> \n", (int)Socket);
}
