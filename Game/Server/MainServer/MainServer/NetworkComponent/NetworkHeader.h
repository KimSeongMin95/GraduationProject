/**************************************************************************
* 저작자(author) && 저작권자(Copyright holder): 김성민(Kim Seongmin)
* Current Affiliation(20/07/23): 홍익대학교 세종캠퍼스 게임소프트웨어 전공 4학년
* NetworkComponent: IOCP 모델을 사용하여 직접 개발한 TCP 네트워크 엔진입니다.
* Program: VisualStudio 2019
* E-mail: ksm950310@naver.com
* License: X (누구나 자유롭게 사용하셔도 좋습니다.)
* Github: https://github.com/KimSeongMin95/GraduationProject
* 사용법: NetworkComponent 객체를 생성하여 서버나 클라이언트로 사용할 수 있습니다.
* (코드가 마음에 들으시면, 제가 게임 프로그래머로 취업할 수 있게 연락 부탁드립니다.)
* (일단 클라이언트 프로그래머를 지망하고 있습니다. 감사합니다!)
***************************************************************************/

#pragma once

#define WIN32_LEAN_AND_MEAN

//// 멀티바이트 집합 사용시 define
//#define _WINSOCK_DEPRECATED_NO_WARNINGS

//////////////////////////////////////////////////////////////////
//// 언리얼엔진도 Windows.h를 사용하기 때문에 naming 충돌을 피하기 위해
//// AllowWindowsPlatformTypes.h과 prewindowsapi.h를 선언하고
//// 윈도우즈 헤더들을 선언한 뒤에 PostWindowsApi.h와 HideWindowsPlatformTypes.h를 선언해야 됩니다.
//////////////////////////////////////////////////////////////////
//
//// put this at the top of your .h file above #includes
//// UE4: allow Windows platform types to avoid naming collisions
//// must be undone at the bottom of this file!
//#include "AllowWindowsPlatformTypes.h"
//#include "prewindowsapi.h"

// winsock2 사용을 위해 아래의 코멘트를 추가합니다.
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