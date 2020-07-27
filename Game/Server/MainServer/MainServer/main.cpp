// MainServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "NetworkComponent/NetworkComponent.h"

#include "MainServer.h"

int main()
{
    CMainServer* mainServer = CMainServer::GetSingleton();

    char IPv4[16];
    USHORT Port;

    do
    {
        INetworkInterface::SetIPv4AndPort(IPv4, Port);
    } while (mainServer->Initialize(IPv4, Port) == false);

    while (true);

    return 0;
}