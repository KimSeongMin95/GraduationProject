#pragma once

#include <iostream>
#include <map>
#include <list>

using namespace std;

enum EPacketType
{
	ACCEPT_PLAYER,		  // �÷��̾ ������ ������ ��: �ش� �÷��̾�� ���� ���� ID�� �̰Ŷ�� �����ؾ� ��.
	CREATE_WAITING_ROOM,  // OnlineWidget���� CreateWaitingRoom ��ư�� ���� ������ ������ ��: 
	FIND_GAMES,			  // MainScreenWidget���� Online ��ư�� ���� ������ ã�� ��: �ش� �÷��̾�� ��� ���ӵ��� ����
	MODIFY_WAITING_ROOM,  // ������ ���濡�� Title�̳� Stage�� MaxOfNum�� ������ ��: FIND_GAMES�� ������ ������ ������ �÷��̾�鿡�� ��ε�ĳ���� �ؾ� ��.
	JOIN_WAITING_ROOM,	  // � �÷��̾ ���濡 ���� ��: FIND_GAMES�� ������ �÷��̾�鿡�� ��ε�ĳ���� �ؾ� ��.
	JOIN_PLAYING_GAME,	  // � �÷��̾ �������� ���ӿ� ���� ��: �ش� ������ �÷��̾��� FIND_GAMES�� �÷��̾�鿡�� ��ε�ĳ���� �ؾ� ��.
	DESTROY_WAITING_ROOM, // ������ ������ ������ ��: FIND_GAMES�� ������ ������ ������ �÷��̾�鿡�� ��ε�ĳ���� �ؾ� ��.
	EXIT_WAITING_ROOM,	  // ������ �ƴ� ������ �÷��̾ ���濡�� ���� ��: FIND_GAMES�� ������ �÷��̾�鿡�� ��ε�ĳ���� �ؾ� ��.

	START_WAITING_ROOM,   // ������ ���濡�� ������ ������ ��: ������ ������ ������ �÷��̾�鿡�� ��ε�ĳ���� �ؾ� ��.
	EXIT_PLAYER,		  // �÷��̾ ������ ������ ��: 
};

struct stInfoOfGame
{
	string State;
	string Title;
	int Leader = 0; // ������ SocketID
	int Stage = 1;
	int MaxOfNum = 100;

	string IPv4OfLeader;
	std::list<int> SocketIDOfPlayers;
};