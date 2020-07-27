
#include "MainClient.h"

#include "NetworkComponent/Console.h"
#include "NetworkComponent/NetworkComponent.h"

unique_ptr<class CNetworkComponent> CMainClient::Client;

CPlayerPacket CMainClient::MyInfoOfPlayer;
CRITICAL_SECTION CMainClient::csMyInfoOfPlayer;

CGamePacket CMainClient::MyInfoOfGame;
CRITICAL_SECTION CMainClient::csMyInfoOfGame;

CThreadSafetyQueue<CGamePacket> CMainClient::tsqFindGames;
CThreadSafetyQueue<CGamePacket> CMainClient::tsqWaitingGame;
CThreadSafetyQueue<bool> CMainClient::tsqDestroyWaitingGame;
CThreadSafetyQueue<CGamePacket> CMainClient::tsqModifyWaitingGame;
CThreadSafetyQueue<bool> CMainClient::tsqStartWaitingGame;
CThreadSafetyQueue<CPlayerPacket> CMainClient::tsqRequestInfoOfGameServer;

CMainClient::CMainClient()
{
	CONSOLE_LOG("[START] <CMainClient::CMainClient()>\n");
	
	// 크리티컬 섹션에 스핀락을 걸고 초기화에 성공할때까지 시도합니다.
	while (InitializeCriticalSectionAndSpinCount(&csMyInfoOfPlayer, SPIN_COUNT) == false);
	while (InitializeCriticalSectionAndSpinCount(&csMyInfoOfGame, SPIN_COUNT) == false);

	Client = make_unique<CNetworkComponent>(ENetworkComponentType::NCT_Client);
	if (Client)
	{
		Client->RegisterConCBF(ConnectCBF);
		Client->RegisterDisconCBF(DisconnectCBF);

		Client->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::LOGIN, RecvLogin);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::FIND_GAMES, RecvFindGames);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::WAITING_GAME, RecvWaitingGame);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::DESTROY_WAITING_GAME, RecvDestroyWaitingGame);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::MODIFY_WAITING_GAME, RecvModifyWaitingGame);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::START_WAITING_GAME, RecvStartWaitingGame);
		Client->RegisterHeaderAndStaticFunc((uint16_t)EMainPacketHeader::REQUEST_INFO_OF_GAME_SERVER, RecvRequestInfoOfGameServer);
	}

	CONSOLE_LOG("[END] <CMainClient::CMainClient()>\n");
}
CMainClient::~CMainClient()
{
	DeleteCriticalSection(&csMyInfoOfPlayer);
	DeleteCriticalSection(&csMyInfoOfGame);
}

CMainClient* CMainClient::GetSingleton()
{
	static CMainClient mainClient;
	return &mainClient;
}

bool CMainClient::Initialize(const char* const IPv4, const USHORT& Port)
{
	if (!Client)
	{
		CONSOLE_LOG("[Error] <CMainClient::Initialize(...)> if (!Client) \n");
		return false;
	}
	/****************************************/

	return Client->Initialize(IPv4, Port);
}
bool CMainClient::IsNetworkOn()
{
	CONSOLE_LOG("[START] <CMainClient::IsNetworkOn()>\n");

	if (!Client)
	{
		CONSOLE_LOG("[Error] <CMainClient::IsNetworkOn())> if (!Client) \n");
		return false;
	}
	/****************************************/

	return Client->IsNetworkOn();
}
void CMainClient::Close()
{
	CONSOLE_LOG("[START] <CMainClient::Close()>\n");
	
	if (!Client)
	{
		CONSOLE_LOG("[Error] <CMainClient::Close())> if (!Client) \n");
		return;
	}
	/****************************************/

	Client->Close();

	///////////
	// 초기화
	///////////
	InitMyInfoOfPlayer();
	InitMyInfoOfGame();

	tsqFindGames.clear();
	tsqWaitingGame.clear();
	tsqDestroyWaitingGame.clear();
	tsqModifyWaitingGame.clear();
	tsqStartWaitingGame.clear();
	tsqRequestInfoOfGameServer.clear();

	CONSOLE_LOG("[END] <CMainClient::Close()>\n");
}

void CMainClient::ConnectCBF(CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CMainClient::ConnectCBF(...)> \n");

	CONSOLE_LOG("[End] <CMainClient::ConnectCBF(...)> \n");
}
void CMainClient::DisconnectCBF(CCompletionKey CompletionKey)
{
	CONSOLE_LOG("[Start] <CMainClient::DisconnectCBF(...)> \n");

	CONSOLE_LOG("[End] <CMainClient::DisconnectCBF(...)> \n");
}

void CMainClient::SendLogin(const FText ID)
{
	CONSOLE_LOG("[Start] <CMainClient::SendLogin(...)>\n");
	
	CPlayerPacket playerPacket;

	// ID가 비어있지 않으면 대입합니다.
	if (!ID.IsEmpty())
		playerPacket.ID = TCHAR_TO_UTF8(*ID.ToString());

	playerPacket.PrintInfo();

	CPacket loginPacket((uint16_t)EMainPacketHeader::LOGIN);
	loginPacket.GetData() << playerPacket << endl;
	if (Client) Client->Send(loginPacket);

	CONSOLE_LOG("[End] <CMainClient::SendLogin(...)>\n");
}
void CMainClient::RecvLogin(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CMainClient::RecvLogin(...)>\n");
	
	CPlayerPacket playerPacket;
	RecvStream >> playerPacket;
	SetMyInfoOfPlayer(playerPacket);
	playerPacket.PrintInfo();

	CONSOLE_LOG("[End] <CMainClient::RecvLogin(...)>\n");
}
void CMainClient::SendCreateGame()
{
	CONSOLE_LOG("[Start] <CMainClient::SendCreateGame()>\n");
	
	CGamePacket gamePacket;
	gamePacket.Leader = CopyMyInfoOfPlayer();
	SetMyInfoOfGame(gamePacket);
	gamePacket.PrintInfo();

	CPacket createGamePacket((uint16_t)EMainPacketHeader::CREATE_GAME);
	createGamePacket.GetData() << gamePacket << endl;
	if (Client) Client->Send(createGamePacket);

	CONSOLE_LOG("[End] <CMainClient::SendCreateGame()>\n");
}
void CMainClient::SendFindGames()
{
	CONSOLE_LOG("[Start] <CMainClient::SendFindGames()>\n");
	
	CPacket findGamePacket((uint16_t)EMainPacketHeader::FIND_GAMES);
	if (Client) Client->Send(findGamePacket);

	CONSOLE_LOG("[End] <CMainClient::SendFindGames()>\n");
}
void CMainClient::RecvFindGames(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CMainClient::RecvFindGames(...)>\n");

	CGamePacket gamePacket;

	while (RecvStream >> gamePacket)
	{
		tsqFindGames.push(gamePacket);
		gamePacket.PrintInfo();
	}

	CONSOLE_LOG("[End] <CMainClient::RecvFindGames(...)>\n");
}
void CMainClient::SendJoinOnlineGame(int SocketIDOfLeader)
{
	CONSOLE_LOG("[Start] <CMainClient::SendJoinWaitingGame(...)>\n");
	CONSOLE_LOG("\t SocketIDOfLeader: %d\n", SocketIDOfLeader);

	CPlayerPacket playerPacket = CopyMyInfoOfPlayer();
	playerPacket.LeaderSocketByMainServer = SocketIDOfLeader;
	SetMyInfoOfPlayer(playerPacket);

	CPacket joinOinlineGamePacket((uint16_t)EMainPacketHeader::JOIN_ONLINE_GAME);
	joinOinlineGamePacket.GetData() << playerPacket << endl;
	if (Client) Client->Send(joinOinlineGamePacket);

	CONSOLE_LOG("[End] <CMainClient::SendJoinWaitingGame(...)>\n");
}
void CMainClient::RecvWaitingGame(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CMainClient::RecvJoinWaitingGame(...)>\n");

	CGamePacket gamePacket;
	RecvStream >> gamePacket;
	SetMyInfoOfGame(gamePacket);
	tsqWaitingGame.push(gamePacket);
	gamePacket.PrintInfo();

	CONSOLE_LOG("[End] <CMainClient::RecvJoinWaitingGame(...)>\n");
}
void CMainClient::SendDestroyWaitingGame()
{
	CONSOLE_LOG("[Start] <CMainClient::SendDestroyWaitingGame()>\n");

	// MyInfoOfPlayer의 특정 멤버변수들 초기화
	InitMyInfoOfPlayerSpecificPart();

	// MyInfoOfGame 초기화
	InitMyInfoOfGame();

	CPacket destroyWaitingGamePacket((uint16_t)EMainPacketHeader::DESTROY_WAITING_GAME);
	if (Client) Client->Send(destroyWaitingGamePacket);

	CONSOLE_LOG("[End] <CMainClient::SendDestroyWaitingGame()>\n");
}
void CMainClient::RecvDestroyWaitingGame(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CMainClient::RecvDestroyWaitingGame(...)>\n");

	// MyInfoOfPlayer의 특정 멤버변수들 초기화
	InitMyInfoOfPlayerSpecificPart();

	// MyInfoOfGame 초기화
	InitMyInfoOfGame();

	tsqDestroyWaitingGame.push(true);

	CONSOLE_LOG("[End] <CMainClient::RecvDestroyWaitingGame(...)>\n");
}
void CMainClient::SendExitWaitingGame()
{
	CONSOLE_LOG("[Start] <CMainClient::SendExitWaitingGame(...)\n");

	// MyInfoOfPlayer의 특정 멤버변수들 초기화
	InitMyInfoOfPlayerSpecificPart();

	// MyInfoOfGame 초기화
	InitMyInfoOfGame();

	CPacket exitWaitingGamePacket((uint16_t)EMainPacketHeader::EXIT_WAITING_GAME);
	if (Client) Client->Send(exitWaitingGamePacket);

	CONSOLE_LOG("[End] <CMainClient::SendExitWaitingGame(...)>\n");
}
void CMainClient::SendModifyWaitingGame()
{
	CONSOLE_LOG("[Start] <CMainClient::SendModifyWaitingGame()>\n");

	CGamePacket gamePacket = CopyMyInfoOfGame();
	gamePacket.PrintInfo();

	CPacket modifyWaitingGamePacket((uint16_t)EMainPacketHeader::MODIFY_WAITING_GAME);
	modifyWaitingGamePacket.GetData() << gamePacket << endl;
	if (Client) Client->Send(modifyWaitingGamePacket);

	CONSOLE_LOG("[End] <CMainClient::SendModifyWaitingGame()>\n");
}
void CMainClient::RecvModifyWaitingGame(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CMainClient::RecvModifyWaitingGame(...)>\n");

	CGamePacket gamePacket;
	RecvStream >> gamePacket;
	SetMyInfoOfGame(gamePacket);
	tsqModifyWaitingGame.push(gamePacket);
	gamePacket.PrintInfo();

	CONSOLE_LOG("[End] <CMainClient::RecvModifyWaitingGame(...)>\n");
}
void CMainClient::SendStartWaitingGame()
{
	CONSOLE_LOG("[Start] <CMainClient::SendStartWaitingGame()>\n");

	CPacket startWaitingGamePacket((uint16_t)EMainPacketHeader::START_WAITING_GAME);
	if (Client) Client->Send(startWaitingGamePacket);

	CONSOLE_LOG("[End] <CMainClient::SendStartWaitingGame()>\n");
}
void CMainClient::RecvStartWaitingGame(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CMainClient::RecvStartWaitingGame(...)>\n");

	tsqStartWaitingGame.push(true);

	CONSOLE_LOG("[End] <CMainClient::RecvStartWaitingGame(...)>\n");
}

void CMainClient::SendActivateGameServer(int PortOfGameServer)
{
	CONSOLE_LOG("[Start] <CMainClient::SendActivateGameServer(...)\n");

	CPlayerPacket playerPacket = CopyMyInfoOfPlayer();
	playerPacket.PortOfGameServer = PortOfGameServer;
	SetMyInfoOfPlayer(playerPacket);
	playerPacket.PrintInfo();

	CGamePacket gamePacket = CopyMyInfoOfGame();
	gamePacket.State = "진행중";
	gamePacket.Leader = playerPacket;
	SetMyInfoOfGame(gamePacket);

	CPacket activateGameServerPacket((uint16_t)EMainPacketHeader::ACTIVATE_GAME_SERVER);
	activateGameServerPacket.GetData() << playerPacket << endl;
	activateGameServerPacket.GetData() << gamePacket << endl;
	if (Client) Client->Send(activateGameServerPacket);

	CONSOLE_LOG("[End] <CMainClient::SendActivateGameServer(...)>\n");
}
void CMainClient::SendRequestInfoOfGameServer()
{
	CONSOLE_LOG("[Start] <CMainClient::SendRequestInfoOfGameServer()>\n");

	CPacket requestInfoOfGameServerPacket((uint16_t)EMainPacketHeader::REQUEST_INFO_OF_GAME_SERVER);
	if (Client) Client->Send(requestInfoOfGameServerPacket);

	CONSOLE_LOG("[End] <CMainClient::SendRequestInfoOfGameServer()>\n");
}
void CMainClient::RecvRequestInfoOfGameServer(stringstream& RecvStream, const SOCKET& Socket /*= NULL*/)
{
	CONSOLE_LOG("[Start] <CMainClient::RecvRequestInfoOfGameServer(...)>\n");

	CPlayerPacket playerPacket;
	RecvStream >> playerPacket;
	playerPacket.PrintInfo();

	CGamePacket gamePacket = CopyMyInfoOfGame();
	gamePacket.State = "진행중";
	gamePacket.Leader = playerPacket;
	SetMyInfoOfGame(gamePacket);

	tsqRequestInfoOfGameServer.push(playerPacket);

	CONSOLE_LOG("[End] <CMainClient::RecvRequestInfoOfGameServer(...)>\n");
}

void CMainClient::SetMyInfoOfPlayer(const CPlayerPacket& PlayerPacket)
{
	EnterCriticalSection(&csMyInfoOfPlayer);
	MyInfoOfPlayer = PlayerPacket;
	LeaveCriticalSection(&csMyInfoOfPlayer);
}
CPlayerPacket CMainClient::CopyMyInfoOfPlayer()
{
	CPlayerPacket playerPacket;

	EnterCriticalSection(&csMyInfoOfPlayer);
	playerPacket = MyInfoOfPlayer;
	LeaveCriticalSection(&csMyInfoOfPlayer);

	return playerPacket;
}
void CMainClient::InitMyInfoOfPlayer()
{
	EnterCriticalSection(&csMyInfoOfPlayer);
	MyInfoOfPlayer = CPlayerPacket();
	LeaveCriticalSection(&csMyInfoOfPlayer);
}
void CMainClient::InitMyInfoOfPlayerSpecificPart()
{
	EnterCriticalSection(&csMyInfoOfPlayer);
	MyInfoOfPlayer.SocketByGameServer = 0;
	MyInfoOfPlayer.PortOfGameServer = 0;
	MyInfoOfPlayer.PortOfGameClient = 0;
	MyInfoOfPlayer.LeaderSocketByMainServer = 0;
	LeaveCriticalSection(&csMyInfoOfPlayer);
}

void CMainClient::SetMyInfoOfGame(const CGamePacket& GamePacket)
{
	EnterCriticalSection(&csMyInfoOfGame);
	MyInfoOfGame = GamePacket;
	LeaveCriticalSection(&csMyInfoOfGame);
}
CGamePacket CMainClient::CopyMyInfoOfGame()
{
	CGamePacket gamePacket;

	EnterCriticalSection(&csMyInfoOfGame);
	gamePacket = MyInfoOfGame;
	LeaveCriticalSection(&csMyInfoOfGame);

	return gamePacket;
}
void CMainClient::InitMyInfoOfGame()
{
	EnterCriticalSection(&csMyInfoOfGame);
	MyInfoOfGame = CGamePacket();
	LeaveCriticalSection(&csMyInfoOfGame);
}
