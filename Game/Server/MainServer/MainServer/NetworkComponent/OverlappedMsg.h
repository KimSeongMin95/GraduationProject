#pragma once

#include "NetworkHeaders.h"
#include "NetworkConfig.h"

class COverlappedMsg sealed
{
public:
	COverlappedMsg();

public:
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;

	char			messageBuffer[MAX_BUFFER];
	int				recvBytes;
	int				sendBytes;

public:
	void Initialize();
};