/**************************************************************************
* ������(author) && ���۱���(Copyright holder): �輺��(Kim Seongmin)
* Current Affiliation(20/07/23): ȫ�ʹ��б� ����ķ�۽� ���Ӽ���Ʈ���� ���� 4�г�
* NetworkComponent: IOCP ���� ����Ͽ� ���� ������ TCP ��Ʈ��ũ �����Դϴ�.
* Program: VisualStudio 2019
* E-mail: ksm950310@naver.com
* License: X (������ �����Ӱ� ����ϼŵ� �����ϴ�.)
* Github: https://github.com/KimSeongMin95/GraduationProject
* ����: NetworkComponent ��ü�� �����Ͽ� ������ Ŭ���̾�Ʈ�� ����� �� �ֽ��ϴ�.
* (�ڵ尡 ������ �����ø�, ���� ���� ���α׷��ӷ� ����� �� �ְ� ���� ��Ź�帳�ϴ�.)
* (�ϴ� Ŭ���̾�Ʈ ���α׷��Ӹ� �����ϰ� �ֽ��ϴ�. �����մϴ�!)
***************************************************************************/

#pragma once

#define WIN32_LEAN_AND_MEAN

//// ��Ƽ����Ʈ ���� ���� define
//#define _WINSOCK_DEPRECATED_NO_WARNINGS

//////////////////////////////////////////////////////////////////
//// �𸮾����� Windows.h�� ����ϱ� ������ naming �浹�� ���ϱ� ����
//// AllowWindowsPlatformTypes.h�� prewindowsapi.h�� �����ϰ�
//// �������� ������� ������ �ڿ� PostWindowsApi.h�� HideWindowsPlatformTypes.h�� �����ؾ� �˴ϴ�.
//////////////////////////////////////////////////////////////////
//
//// put this at the top of your .h file above #includes
//// UE4: allow Windows platform types to avoid naming collisions
//// must be undone at the bottom of this file!
//#include "AllowWindowsPlatformTypes.h"
//#include "prewindowsapi.h"

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

//// put this at the bottom of the .h file
//// UE4: disallow windows platform types
//// this was enabled at the top of the file
//#include "PostWindowsApi.h"
//#include "HideWindowsPlatformTypes.h"

using namespace std;