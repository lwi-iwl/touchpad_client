#ifndef CLIENT_H
#define CLIENT_H
#include <windows.h>
#include "Devices.h"
#define DEFAULT_BUFLEN 7
class Client
{
private:
    int recvbuflen = DEFAULT_BUFLEN;
    SOCKET client;
public:
    void close();
    void sendClose();
    SOCKET getSock();
    boolean startClient(Devices devices, int number);
};
#endif