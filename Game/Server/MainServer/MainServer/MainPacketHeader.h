#pragma once

#include "NetworkComponent/NetworkHeader.h"

enum class EMainPacketHeader : uint16_t
{
	/** ��Ŷ ����
	int PacketSize;
	int EPacketType;
	... Data;
	*/

	// �ּ� [1 1 ] ������� Ÿ�Ա��� 4����Ʈ?
	// �ִ� [4096(MAX_BUFFER) 1 ] ����� 4����Ʈ?

	/** ����
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

	/** �÷��̾ OnlineWidget���� LOGIN �ϸ�
	Main Client:
		Send [LOGIN]: �ӽ� ��ü�� cInfoOfPlayer infoOfPlayer�� ID�� ������ �۽�
		Recv [LOGIN]: ���� cInfoOfPlayer�� MyInfo�� ����
	Main Server:
		Recv [LOGIN]: ���� cInfoOfPlayer�� IPv4Addr, SocketByServer, PortByServer�� �����ϰ� InfoOfClients�� ����
		Send [LOGIN]: cInfoOfPlayer
	*/
	LOGIN,

	/** OnlineGameWidget���� (Create Game)��ư�� ���� ������ �����ϸ�
	Main Client:
		Send [CREATE_GAME]: MyInfoOfGame.Leader�� MyInfo�� �����ϰ� �۽�
		Recv []: X
	Main Server:
		Recv [CREATE_GAME]: ���� cInfoOfGame�� InfoOfGames�� ����
		Send []: X
	*/
	CREATE_GAME,

	/** �÷��̾ MainScreenWidget���� Online ��ư�� ���� �����ϸ�
	Main Client:
		Send [FIND_GAMES]: ���� ���� �䱸
		Recv [FIND_GAMES]: ��� ������� ����
	Main Server:
		Recv [FIND_GAMES]: O
		Send [FIND_GAMES]: InfoOfGames�� ���ԵǾ� �ִ� ��� cInfoOfGame�� ����
	*/
	FIND_GAMES,

	/** ���� ���� ����� */
	WAITING_GAME,

	/** �÷��̾ Join ��ư���� ���ӹ濡 ����
	Main Client:
		Send [JOIN_ONLINE_GAME]: Join�� ���ӹ� Leader�� SocketID�� ������ MyInfo
		Recv [WAITING_GAME]: ���� cInfoOfGame�� MyInfoOfGame�� ����
	Main Server:
		Recv [JOIN_ONLINE_GAME]: InfoOfClients�� LeaderSocketByMainServer�� �����ϰ� InfoOfGames�� Players�� �ش� Ŭ���̾�Ʈ ����
		Send [WAITING_GAME] to ����: �ش� ���ӹ��� cInfoOfGame
		Send [WAITING_GAME] to ���ӹ��� �ٸ� �÷��̾��: �ش� ���ӹ��� cInfoOfGame
	*/
	JOIN_ONLINE_GAME,

	/** ������ ���濡�� Back ��ư�� ���� ������ �����ϸ�
	Main Client:
		Send [DESTROY_WAITING_ROOM]: O, InitMyInfoOfGame();
		Recv [DESTROY_WAITING_ROOM]: ���� ���� �˸��� �ڷΰ��� ��ư Ȱ��ȭ, MyInfo�� Ư�� ��������� �ʱ�ȭ, InitMyInfoOfGame();
	Main Server:
		Recv [DESTROY_WAITING_ROOM] by ����: InfoOfClients�� �ش� Ŭ���̾�Ʈ�� Ư�� ��������� �ʱ�ȭ, InfoOfGames.erase(pSocketInfo->socket);
		Send [DESTROY_WAITING_ROOM] to �÷��̾��(���� ����): O
	*/
	DESTROY_WAITING_GAME,

	/** ������ �ƴ� ������ �÷��̾ ���濡�� ������
	Main Client:
		Send [EXIT_WAITING_ROOM]: O, MyInfo�� Ư�� ��������� �ʱ�ȭ, InitMyInfoOfGame();
		Recv [WAITING_GAME]: ���� cInfoOfGame
	Main Server:
		Recv [EXIT_WAITING_ROOM]: InfoOfClients�� �ش� Ŭ���̾�Ʈ�� Ư�� ��������� �ʱ�ȭ, InfoOfGames�� Players�� �ش� Ŭ���̾�Ʈ ����
		Send [WAITING_GAME] to ����: �ش� ������ cInfoOfGame
		Send [WAITING_GAME] to �÷��̾��(�ش� Ŭ���̾�Ʈ ������): �ش� ������ cInfoOfGame
	*/
	EXIT_WAITING_GAME,

	/** ������ ���濡�� Title�̳� Stage�� Maximum�� �����ϸ�
	Main Client:
		Send [MODIFY_WAITING_GAME]: cInfoOfGame(Title, Stage, Maximum)
		Recv [MODIFY_WAITING_GAME]: cInfoOfGame(Title, Stage, Maximum)
	Main Server:
		Recv [MODIFY_WAITING_GAME]: InfoOfGames�� cInfoOfGame(Title, Stage, Maximum) ����
		Send [MODIFY_WAITING_GAME] to �÷��̾��(���� ����): cInfoOfGame(Title, Stage, Maximum)
	*/
	MODIFY_WAITING_GAME,

	/** ������ ���濡�� ������ �����ϸ�
	Main Client:
		Send [START_WAITING_GAME]: O
		Recv [START_WAITING_GAME]: O
	Main Server:
		Recv [START_WAITING_GAME] by ����: O
		Send [START_WAITING_GAME] to �÷��̾��(���� ����): O
	*/
	START_WAITING_GAME,


	/** ������ ���濡�� ������ �����ϰ� CountStartedGame()���� ���� ���� �ʱ�ȭ�� �����ϸ�
	Main Client:
		Send [ACTIVATE_GAME_SERVER]: MyInfo�� PortOfGameServer�� �����ϰ� MyInfo�� ����, MyInfoOfGame.State = string("Playing");
		Recv [WAITING_GAME]: ���� cInfoOfGame
	Main Server:
		Recv [ACTIVATE_GAME_SERVER] by ����: ������ cInfoOfPlayer�� InfoOfClients�� InfoOfGames�� Leader�� ����
		Send [WAITING_GAME] to ����: �ش� ������ cInfoOfGame
		Send [WAITING_GAME] to �÷��̾��: �ش� ������ cInfoOfGame
	*/
	ACTIVATE_GAME_SERVER,

	/** �����ڰ� ���� Ŭ���̾�Ʈ�� ���� ������ �����Ű�� ���� ���� ���� ������ ��û
	Main Client:
		Send [REQUEST_INFO_OF_GAME_SERVER]: O
		Recv [REQUEST_INFO_OF_GAME_SERVER]: cInfoOfPlayer�� �ް� IPv4Addr�� PortOfGameServer�� ȹ��
	Main Server:
		Recv [REQUEST_INFO_OF_GAME_SERVER]: O
		Send [REQUEST_INFO_OF_GAME_SERVER]: cInfoOfPlayer infoOfPlayer = InfoOfClients.at(leaderSocket);�ϰ� infoOfPlayer ����
	*/
	REQUEST_INFO_OF_GAME_SERVER,
};
