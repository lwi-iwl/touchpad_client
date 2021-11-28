#include<winsock2.h>
#include "Client.h"
#include <bluetoothapis.h>
#include<ws2bth.h>
#include "Devices.h"


boolean Client::startClient(Devices devices, int number)
{
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);

    int wsOk = WSAStartup(ver, &wsData);

    //CREATE A BLUETOOTH SOCKET
    client = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);

    if (client == INVALID_SOCKET)
        WSACleanup();

    //AUTO ALLOCATION FOR SERVER CHANNEL 

    SOCKADDR_BTH hint;
    memset(&hint, 0, sizeof(hint));
    hint.addressFamily = AF_BTH;
    hint.port = 0;
    hint.btAddr = BTH_ADDR(devices.getDeviceMac(number));
    GUID nguiD = { 0x00001101, 0x0000, 0x1000,  {0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB } };
    hint.serviceClassId = nguiD;


    int connResult = connect(client, (sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR)
    {
        closesocket(client);
        WSACleanup();
    }

    const char* sendbuf = "..";
    int recvbuflen = DEFAULT_BUFLEN;
    char recvbuf[DEFAULT_BUFLEN] = "";

    int res = send(client, sendbuf, (int)strlen(sendbuf), MSG_OOB);

    if (res == SOCKET_ERROR)
    {
        closesocket(client);
        WSACleanup();
        return false;
    }
    res = recv(client, recvbuf, recvbuflen, 0);
    return true;
}


void Client::sendClose()
{
    send(client, "DISCO", 5, 0);
    closesocket(client);
}

void Client::close()
{
    closesocket(client);
}

SOCKET Client::getSock()
{
    return client;
}