/**************************************************************************
* 저작자(author) && 저작권자(Copyright holder): 김성민(Kim Seongmin)
* Current Affiliation(20/07/23): 홍익대학교 세종캠퍼스 게임소프트웨어 전공 4학년
* NetworkComponent: IOCP 모델을 사용하여 직접 개발한 TCP 네트워크 엔진입니다.
* E-mail: ksm950310@naver.com
* License: X (누구나 자유롭게 사용하셔도 좋습니다.)
* Github: https://github.com/KimSeongMin95/GraduationProject
* Caution: 코드에 버그가 존재할 수 있습니다. 이 점 유의하시기 바랍니다.
* (코드가 마음에 들으시면, 제가 게임 프로그래머로 취업할 수 있게 연락 부탁드립니다.)
* (일단 클라이언트 프로그래머를 지망하고 있습니다. 감사합니다!)
***************************************************************************/

#pragma once

//// 멀티바이트 집합 사용시 define
//#define _WINSOCK_DEPRECATED_NO_WARNINGS

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

using namespace std;