#define _CRT_SECURE_NO_WARNINGS
#include<winsock2.h>
#include <windows.h>
#include <iostream>
#include <comdef.h>
#include <string>
#include <stdio.h>
#include <locale.h>
#include <TlHelp32.h>
#include <bluetoothapis.h>
#include<ws2bth.h>
#include <thread>
#include "Devices.h"
#include "Client.h"
#pragma comment (lib, "ws2_32.lib")
#pragma comment(lib, "bthprops.lib")
#define DEFAULT_BUFLEN 7

Devices devices;
Client client;
SOCKET sock;
boolean isRecieve = false;
char recvx[DEFAULT_BUFLEN] = "";
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INPUT Event = { 0 };
HKL keyboard = LoadKeyboardLayoutA("00000409", KLF_ACTIVATE);
int index = 0;
HWND buttons[20];

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int nCmdShow)
{
    WNDCLASS windowClass = { 0 };
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = L"HELLO_WORLD";
    windowClass.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
    RegisterClass(&windowClass);
    HWND hwnd = CreateWindow(
        windowClass.lpszClassName,
        L"CatXP",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        100, 50, 720, 1060,
        nullptr, nullptr,
        hInstance,
        nullptr);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}

void startGetCoordinates(SOCKET sock) 
{
    while (isRecieve)
    {
        int res = 0;
        int recvbuflen = DEFAULT_BUFLEN;
        res = recv(sock, recvx, recvbuflen, 0);
    }
}

void addButtons(HWND hWnd)
{
    devices.deleteDevices();
    //SetTimer(hWnd, 2, 20, NULL);
    devices.startSearch(hWnd);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case WM_CREATE:
    {
        HWND Button = CreateWindow(L"BUTTON", L"SearchBluetooth", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 200, 30, hWnd, (HMENU)21, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        HWND hScroll = CreateWindow(L"scrollbar", NULL, WS_CHILD | WS_VISIBLE | SBS_VERT, 685, 0, 20, 320, hWnd, (HMENU)22, (HINSTANCE)GetWindowLongA(hWnd, -6), NULL);
    }
    break;

    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
        {
            if (LOWORD(wParam) == 21)
            {
                addButtons(hWnd);
            }
            else if ((LOWORD(wParam) >= 0)&&(LOWORD(wParam) < 20))
            {
                if (isRecieve)
                {
                    isRecieve = false;
                    KillTimer(hWnd, 1);
                    client.sendClose();
                }
                if (client.startClient(devices, LOWORD(wParam)))
                {
                    isRecieve = true;
                    sock = client.getSock();
                    std::thread thr(startGetCoordinates, sock);
                    thr.detach();
                    SetTimer(hWnd, 1, 20, NULL);
                }

            }
        }
        }
        break;
    }
    break;

    case WM_TIMER:
        if (strlen(recvx) == 7) {
            if (recvx[0] == 'K')
            {
                if (recvx[1] == 'D')
                {
                    Event.type = INPUT_KEYBOARD;
                    Event.ki.dwFlags = KEYEVENTF_SCANCODE;
                    if (recvx[3] == 's')
                    {
                        Event.ki.wScan = MapVirtualKey(0x1B, 0);
                    }
                    else if (recvx[2] == '_')
                    {
                        Event.ki.wScan = MapVirtualKey(LOBYTE(VkKeyScanEx(' ', keyboard)), 0);
                    }
                    else
                    {
                        Event.ki.wScan = MapVirtualKey(LOBYTE(VkKeyScanEx(recvx[2], keyboard)), 0);
                    }
                    SendInput(1, &Event, sizeof(Event));
                }
                else
                {
                    Event.type = INPUT_KEYBOARD;
                    Event.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
                    if (recvx[3] == 's')
                    {
                        Event.ki.wScan = MapVirtualKey(0x1B, 0);
                    }
                    else if (recvx[2] == '_')
                    {
                        Event.ki.wScan = MapVirtualKey(LOBYTE(VkKeyScanEx(' ', keyboard)), 0);
                    }
                    else
                    {
                        Event.ki.wScan = MapVirtualKey(LOBYTE(VkKeyScanEx(recvx[2], keyboard)), 0);
                    }
                    SendInput(1, &Event, sizeof(Event));
                }
                memset(recvx, 0, DEFAULT_BUFLEN);
            }
            else if (recvx[0] == 'M')
            {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hWnd, &pt);
                std::string x = "";
                char xd[2] = { recvx[2], recvx[3] };
                x.append(xd);

                std::string y = "";
                char yd[2] = { recvx[5], recvx[6] };
                y.append(yd);
                int xi = 0;
                int yi = 0;
                if (recvx[1] == 'R')
                    xi = std::stoi(xd);
                else
                    xi = -std::stoi(xd);

                if (recvx[4] == 'U')
                    yi = -std::stoi(yd);
                else
                    yi = std::stoi(yd);

                pt.x = pt.x + xi * 3;
                pt.y = pt.y + yi * 3;

                ClientToScreen(hWnd, &pt);
                INPUT input;
                input.type = INPUT_MOUSE;
                input.mi.mouseData = 0;
                input.mi.time = 0;
                input.mi.dx = xi * 3;
                input.mi.dy = yi * 3;
                input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK;
                SendInput(1, &input, sizeof(input));
                //return true;
                //SetCursorPos(pt.x, pt.y);
                memset(recvx, 0, DEFAULT_BUFLEN);
            }
            else if (recvx[0] == 'L')
            {
                if (recvx[4] == 'D')
                {
                    Sleep(200);
                    if (recvx[4] != '2')
                    {
                        POINT pt;
                        GetCursorPos(&pt);
                        ScreenToClient(hWnd, &pt);
                        mouse_event(MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, 0);
                        memset(recvx, 0, DEFAULT_BUFLEN);
                    }
                    else
                    {
                        POINT pt;
                        GetCursorPos(&pt);
                        ScreenToClient(hWnd, &pt);
                        mouse_event(MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, 0);
                        mouse_event(MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, 0);
                        mouse_event(MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, 0);
                        mouse_event(MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, 0);
                        memset(recvx, 0, DEFAULT_BUFLEN);
                    }
                }
                else if (recvx[4] == 'U')
                {
                    POINT pt;
                    GetCursorPos(&pt);
                    ScreenToClient(hWnd, &pt);
                    mouse_event(MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, 0);
                    memset(recvx, 0, DEFAULT_BUFLEN);
                }
                else if (recvx[4] == 'C')
                {
                    POINT pt;
                    GetCursorPos(&pt);
                    ScreenToClient(hWnd, &pt);
                    mouse_event(MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, 0);
                    mouse_event(MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, 0);
                    memset(recvx, 0, DEFAULT_BUFLEN);
                }
            }
            else if (recvx[0] == 'R')
            {
                if (recvx[4] == 'D')
                {
                    POINT pt;
                    GetCursorPos(&pt);
                    ScreenToClient(hWnd, &pt);
                    mouse_event(MOUSEEVENTF_RIGHTDOWN, pt.x, pt.y, 0, 0);
                    memset(recvx, 0, DEFAULT_BUFLEN);
                }
                else if (recvx[4] == 'U')
                {
                    POINT pt;
                    GetCursorPos(&pt);
                    ScreenToClient(hWnd, &pt);
                    mouse_event(MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, 0);
                    memset(recvx, 0, DEFAULT_BUFLEN);
                }
                else
                {
                    POINT pt;
                    GetCursorPos(&pt);
                    ScreenToClient(hWnd, &pt);
                    mouse_event(MOUSEEVENTF_RIGHTDOWN, pt.x, pt.y, 0, 0);
                    mouse_event(MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, 0);
                    memset(recvx, 0, DEFAULT_BUFLEN);
                }
            }
            else if (recvx[0] == 'S')
            {
                std::string scroll = "";
                char scrolld[2] = { recvx[5], recvx[6] };
                scroll.append(scrolld);
                int scrolli;
                if (recvx[4] == 'U')
                    scrolli = -std::stoi(scrolld);
                else
                    scrolli = std::stoi(scrolld);
                SendInput;
                mouse_event(MOUSEEVENTF_WHEEL, 0, 0, scrolli, 0);
                SendInput;
                memset(recvx, 0, DEFAULT_BUFLEN);

            }
            else if (recvx[0] == 'C')
            {
                if (recvx[1] == 'D')
                {

                    Event.type = INPUT_KEYBOARD;
                    Event.ki.dwFlags = 0;
                    if (recvx[6] == 'r')
                        Event.ki.wVk = VK_RETURN;
                    else if (recvx[6] == 'e')
                        Event.ki.wVk = VK_SPACE;
                    else if (recvx[5] == 'p')
                        Event.ki.wVk = VK_SNAPSHOT;
                    else if (recvx[5] == 'k')
                        Event.ki.wVk = VK_BACK;
                    else if (recvx[5] == 's')
                        Event.ki.wVk = VK_CAPITAL;
                    else if (recvx[4] == 'b')
                        Event.ki.wVk = VK_TAB;
                    else if (recvx[4] == 'r')
                        Event.ki.wVk = VK_LCONTROL;
                    else if (recvx[4] == 'n')
                        Event.ki.wVk = VK_LWIN;
                    else if (recvx[4] == 't')
                        Event.ki.wVk = VK_LMENU;
                    else if (recvx[4] == 'с')
                        Event.ki.wVk = VK_ESCAPE;
                    else if (recvx[3] == 'h')
                        Event.ki.wVk = VK_RSHIFT;
                    else if (recvx[3] != 'a' && (recvx[2] == 'f'))
                    {
                        if (recvx[4] == '2')
                            Event.ki.wVk = VK_F12;
                        else if (recvx[4] == '1')
                            Event.ki.wVk = VK_F11;
                        else
                        {
                            int shift = (int)recvx[3] - 49;
                            Event.ki.wVk = VK_F1 + shift;
                        }
                    }
                    else
                    {
                        Event.ki.wVk = VkKeyScanEx(recvx[2], keyboard);
                    }
                    SendInput(1, &Event, sizeof(Event));
                }
                else
                {
                    Event.type = INPUT_KEYBOARD;
                    Event.ki.dwFlags = KEYEVENTF_KEYUP;
                    if (recvx[6] == 'r')
                        Event.ki.wVk = VK_RETURN;
                    else if (recvx[6] == 'e')
                        Event.ki.wVk = VK_SPACE;
                    else if (recvx[5] == 'p')
                        Event.ki.wVk = VK_SNAPSHOT;
                    else if (recvx[5] == 'k')
                        Event.ki.wVk = VK_BACK;
                    else if (recvx[5] == 's')
                        Event.ki.wVk = VK_CAPITAL;
                    else if (recvx[4] == 'b')
                        Event.ki.wVk = VK_TAB;
                    else if (recvx[4] == 'r')
                        Event.ki.wVk = VK_LCONTROL;
                    else if (recvx[4] == 'n')
                        Event.ki.wVk = VK_LWIN;
                    else if (recvx[4] == 't')
                        Event.ki.wVk = VK_LMENU;
                    else if (recvx[4] == 'с')
                        Event.ki.wVk = VK_ESCAPE;
                    else if (recvx[3] == 'h')
                        Event.ki.wVk = VK_RSHIFT;
                    else if (recvx[3] != 'a' && (recvx[2] == 'f'))
                    {
                        if (recvx[4] == '2')
                            Event.ki.wVk = VK_F12;
                        else if (recvx[4] == '1')
                            Event.ki.wVk = VK_F11;
                        else
                        {
                            int shift = (int)recvx[3] - 49;
                            Event.ki.wVk = VK_F1;
                        }
                    }
                    else
                    {
                        Event.ki.wVk = VkKeyScanEx(recvx[2], keyboard);
                    }
                    SendInput(1, &Event, sizeof(Event));
                }
                memset(recvx, 0, DEFAULT_BUFLEN);
            }
            else if (recvx[0] == 'D')
            {
                isRecieve = false;
                KillTimer(hWnd, 1);
                client.close();
                memset(recvx, 0, DEFAULT_BUFLEN);
            }

        }
        break;

    case WM_DESTROY:
    {
        client.sendClose();
        PostQuitMessage(0);
    }
    break;
    default: {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    }
    return 0;
}
