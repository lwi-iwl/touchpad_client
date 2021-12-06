#ifndef CLIENT_H
#define CLIENT_H
#include <windows.h>
#include "Devices.h"
#define DEFAULT_BUFLEN 7
class Client
{
private:
    int recvbuflen = DEFAULT_BUFLEN;
    SOCKET client[20];
public:
    void close(int number);
    void sendClose(int number);
    SOCKET getSock(int number);
    boolean startClient(Devices devices, int number);
};
#endif