#pragma once

#include "NetworkHeader.h"

class CCompletionKey sealed
{
public:
	CCompletionKey();

public:
	SOCKET	socket;

	string	IPv4Addr; // ���� Ŭ���̾�Ʈ�� IP �ּ�
	USHORT	Port;	  // ���� Ŭ���̾�Ʈ�� Port �ּ�

public:
	void PrintInfo(const char* c_str);
};