
#include "OverlappedMsg.h"

COverlappedMsg::COverlappedMsg()
{
	ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	overlapped.hEvent = NULL; // IOCP������ overlapped.hEvent�� �� NULL�� ����� �Ѵٰ� �մϴ�.
	ZeroMemory(messageBuffer, MAX_BUFFER);
	dataBuf.len = MAX_BUFFER;
	dataBuf.buf = messageBuffer;
	recvBytes = 0;
	sendBytes = 0;
}

void COverlappedMsg::Initialize()
{
	ZeroMemory(&overlapped, sizeof(OVERLAPPED));
	overlapped.hEvent = NULL; // IOCP������ overlapped.hEvent�� �� NULL�� ����� �Ѵٰ� �մϴ�.
	ZeroMemory(messageBuffer, MAX_BUFFER);
	dataBuf.len = MAX_BUFFER;
	dataBuf.buf = messageBuffer;
	recvBytes = 0;
	sendBytes = 0;
}
