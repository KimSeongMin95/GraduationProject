
#include "OverlappedMsg.h"

COverlappedMsg::COverlappedMsg()
{
	ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	overlapped.hEvent = NULL; // IOCP에서는 overlapped.hEvent를 꼭 NULL로 해줘야 한다고 합니다.
	ZeroMemory(messageBuffer, MAX_BUFFER);
	dataBuf.len = MAX_BUFFER;
	dataBuf.buf = messageBuffer;
	recvBytes = 0;
	sendBytes = 0;
}

void COverlappedMsg::Initialize()
{
	ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	overlapped.hEvent = NULL; // IOCP에서는 overlapped.hEvent를 꼭 NULL로 해줘야 한다고 합니다.
	ZeroMemory(messageBuffer, MAX_BUFFER);
	dataBuf.len = MAX_BUFFER;
	dataBuf.buf = messageBuffer;
	recvBytes = 0;
	sendBytes = 0;
}
