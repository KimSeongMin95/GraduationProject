#pragma once

#include "NetworkHeader.h"

class CCompletionKey final
{
public:
	CCompletionKey();

public:
	SOCKET	Socket;

	string	IPv4Addr; // ���� Ŭ���̾�Ʈ�� IP �ּ�
	USHORT	Port;	  // ���� Ŭ���̾�Ʈ�� Port �ּ�

public:
	void PrintInfo(const char* c_str);
};