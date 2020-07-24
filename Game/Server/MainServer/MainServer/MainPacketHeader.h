#pragma once

#include "NetworkComponent/NetworkHeader.h"

enum class EMainPacketHeader : uint16_t
{
	/** 패킷 구조
	int PacketSize;
	int EPacketType;
	... Data;
	*/

	// 최소 [1 1 ] 사이즈와 타입까지 4바이트?
	// 최대 [4096(MAX_BUFFER) 1 ] 사이즈만 4바이트?

	/** 설명
	Main Client:
		Send [EPacketType]:
		Recv [EPacketType]:
	Main Server:
		Recv [EPacketType]:
		Send [EPacketType]:
	*/


	///////////////////////////////////////////
	// Main Server / Main Clients
	///////////////////////////////////////////

	/** 플레이어가 OnlineWidget에서 LOGIN 하면
	Main Client:
		Send [LOGIN]: 임시 객체인 cInfoOfPlayer infoOfPlayer에 ID를 저장후 송신
		Recv [LOGIN]: 받은 cInfoOfPlayer를 MyInfo에 대입
	Main Server:
		Recv [LOGIN]: 받은 cInfoOfPlayer에 IPv4Addr, SocketByServer, PortByServer를 저장하고 InfoOfClients에 삽입
		Send [LOGIN]: cInfoOfPlayer
	*/
	LOGIN,

	/** OnlineGameWidget에서 (Create Game)버튼을 눌러 대기방을 생성하면
	Main Client:
		Send [CREATE_GAME]: MyInfoOfGame.Leader에 MyInfo를 저장하고 송신
		Recv []: X
	Main Server:
		Recv [CREATE_GAME]: 받은 cInfoOfGame를 InfoOfGames에 삽입
		Send []: X
	*/
	CREATE_GAME,

	/** 플레이어가 MainScreenWidget에서 Online 버튼을 눌러 진입하면
	Main Client:
		Send [FIND_GAMES]: 대기방 정보 요구
		Recv [FIND_GAMES]: 모든 대기방들의 정보
	Main Server:
		Recv [FIND_GAMES]: O
		Send [FIND_GAMES]: InfoOfGames에 삽입되어 있는 모든 cInfoOfGame의 정보
	*/
	FIND_GAMES,

	/** 대기방 정보 변경시 */
	WAITING_GAME,

	/** 플레이어가 Join 버튼으로 게임방에 들어가면
	Main Client:
		Send [JOIN_ONLINE_GAME]: Join한 게임방 Leader의 SocketID를 대입한 MyInfo
		Recv [WAITING_GAME]: 받은 cInfoOfGame을 MyInfoOfGame에 대입
	Main Server:
		Recv [JOIN_ONLINE_GAME]: InfoOfClients에 LeaderSocketByMainServer를 대입하고 InfoOfGames의 Players에 해당 클라이언트 삽입
		Send [WAITING_GAME] to 방장: 해당 게임방의 cInfoOfGame
		Send [WAITING_GAME] to 게임방의 다른 플레이어들: 해당 게임방의 cInfoOfGame
	*/
	JOIN_ONLINE_GAME,

	/** 방장이 대기방에서 Back 버튼을 눌러 대기방을 종료하면
	Main Client:
		Send [DESTROY_WAITING_ROOM]: O, InitMyInfoOfGame();
		Recv [DESTROY_WAITING_ROOM]: 대기방 종료 알림에 뒤로가기 버튼 활성화, MyInfo의 특정 멤버변수들 초기화, InitMyInfoOfGame();
	Main Server:
		Recv [DESTROY_WAITING_ROOM] by 방장: InfoOfClients의 해당 클라이언트의 특정 멤버변수들 초기화, InfoOfGames.erase(pSocketInfo->socket);
		Send [DESTROY_WAITING_ROOM] to 플레이어들(방장 제외): O
	*/
	DESTROY_WAITING_GAME,

	/** 방장이 아닌 대기방인 플레이어가 대기방에서 나가면
	Main Client:
		Send [EXIT_WAITING_ROOM]: O, MyInfo의 특정 멤버변수들 초기화, InitMyInfoOfGame();
		Recv [WAITING_GAME]: 대기방 cInfoOfGame
	Main Server:
		Recv [EXIT_WAITING_ROOM]: InfoOfClients의 해당 클라이언트의 특정 멤버변수들 초기화, InfoOfGames의 Players에 해당 클라이언트 제거
		Send [WAITING_GAME] to 방장: 해당 대기방의 cInfoOfGame
		Send [WAITING_GAME] to 플레이어들(해당 클라이언트 미포함): 해당 대기방의 cInfoOfGame
	*/
	EXIT_WAITING_GAME,

	/** 방장이 대기방에서 Title이나 Stage나 Maximum을 수정하면
	Main Client:
		Send [MODIFY_WAITING_GAME]: cInfoOfGame(Title, Stage, Maximum)
		Recv [MODIFY_WAITING_GAME]: cInfoOfGame(Title, Stage, Maximum)
	Main Server:
		Recv [MODIFY_WAITING_GAME]: InfoOfGames에 cInfoOfGame(Title, Stage, Maximum) 적용
		Send [MODIFY_WAITING_GAME] to 플레이어들(방장 제외): cInfoOfGame(Title, Stage, Maximum)
	*/
	MODIFY_WAITING_GAME,

	/** 방장이 대기방에서 게임을 시작하면
	Main Client:
		Send [START_WAITING_GAME]: O
		Recv [START_WAITING_GAME]: O
	Main Server:
		Recv [START_WAITING_GAME] by 방장: O
		Send [START_WAITING_GAME] to 플레이어들(방장 제외): O
	*/
	START_WAITING_GAME,


	/** 방장이 대기방에서 게임을 시작하고 CountStartedGame()에서 게임 서버 초기화에 성공하면
	Main Client:
		Send [ACTIVATE_GAME_SERVER]: MyInfo에 PortOfGameServer를 저장하고 MyInfo를 전송, MyInfoOfGame.State = string("Playing");
		Recv [WAITING_GAME]: 대기방 cInfoOfGame
	Main Server:
		Recv [ACTIVATE_GAME_SERVER] by 방장: 수신한 cInfoOfPlayer를 InfoOfClients와 InfoOfGames의 Leader에 적용
		Send [WAITING_GAME] to 방장: 해당 대기방의 cInfoOfGame
		Send [WAITING_GAME] to 플레이어들: 해당 대기방의 cInfoOfGame
	*/
	ACTIVATE_GAME_SERVER,

	/** 참가자가 게임 클라이언트를 게임 서버와 연결시키기 위해 게임 서버 정보를 요청
	Main Client:
		Send [REQUEST_INFO_OF_GAME_SERVER]: O
		Recv [REQUEST_INFO_OF_GAME_SERVER]: cInfoOfPlayer를 받고 IPv4Addr와 PortOfGameServer를 획득
	Main Server:
		Recv [REQUEST_INFO_OF_GAME_SERVER]: O
		Send [REQUEST_INFO_OF_GAME_SERVER]: cInfoOfPlayer infoOfPlayer = InfoOfClients.at(leaderSocket);하고 infoOfPlayer 전송
	*/
	REQUEST_INFO_OF_GAME_SERVER,
};
