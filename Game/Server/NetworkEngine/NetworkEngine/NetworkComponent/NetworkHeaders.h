/***********************************************************************
* ������(author) && ���۱���(Copyright holder): �輺��(Kim Seongmin)
* ���� �Ҽ�(20/07/23): ȫ�ʹ��б� ����ķ�۽� ���Ӽ���Ʈ���� ���� 4�г�
* E-mail: ksm950310@naver.com
* License: X, �����Ӱ� ����ϼŵ� �����ϴ�.
* ����: ���� ������ IOCP ����� TCP ��Ʈ��ũ �����Դϴ�.
* (�ڵ尡 ������ �����ø�, ���� ���� ���α׷��ӷ� ����� �� �ְ� ���� ��Ź�帳�ϴ�.)
* (�ϴ� Ŭ���̾�Ʈ ���α׷��Ӹ� �����ϰ� �ֽ��ϴ�.)
************************************************************************/

#pragma once

//// ��Ƽ����Ʈ ���� ���� define
//#define _WINSOCK_DEPRECATED_NO_WARNINGS

// winsock2 ����� ���� �Ʒ��� �ڸ�Ʈ�� �߰��մϴ�.
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <WS2tcpip.h> // For: inet_pton()

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>

#include <process.h>
#include <memory>

#include <deque>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <queue>

#include <algorithm>

#include <ctime>

using namespace std;