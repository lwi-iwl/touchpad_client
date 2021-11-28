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
char recvx[DEFAULT_BUFLEN] = "";
boolean isRecieve = true;
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INPUT Event = { 0 };


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
                if (client.startClient(devices, LOWORD(wParam)))
                {
                    isRecieve = true;
                    sock = client.getSock();
                    std::thread thr(startGetCoordinates, sock);
                    thr.detach();
                    SetTimer(hWnd, 1, 20, NULL);
                }

            }
            else if (LOWORD(wParam) == 6)
            {
                
            }
            else if (LOWORD(wParam) == 7)
            {
                
            }
        }
        }
        break;
    }
    break;

    case WM_TIMER:
        if (strlen(recvx) == 7) {
            if (recvx[0] == 'M')
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
                input.mi.dx =xi*3;
                input.mi.dy = yi*3;
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
                    POINT pt;
                    GetCursorPos(&pt);
                    ScreenToClient(hWnd, &pt);
                    mouse_event(MOUSEEVENTF_LEFTDOWN, pt.x, pt.y, 0, 0);
                    memset(recvx, 0, DEFAULT_BUFLEN);
                }
                else if (recvx[4] == 'U')
                {
                    POINT pt;
                    GetCursorPos(&pt);
                    ScreenToClient(hWnd, &pt);
                    mouse_event(MOUSEEVENTF_LEFTUP, pt.x, pt.y, 0, 0);
                    memset(recvx, 0, DEFAULT_BUFLEN);
                }
                else
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
                    
                    if (recvx[6] == 'r')
                        keybd_event(VK_RETURN, 0, 0, 0);
                    else if (recvx[6] == 'e')
                        keybd_event(VK_SPACE, 0, 0, 0);
                    else if (recvx[5] == 'k')
                        keybd_event(VK_BACK, 0, 0, 0);
                    else if (recvx[5] == 's')
                        keybd_event(VK_CAPITAL, 0, 0, 0);
                    else if (recvx[4] == 'b')
                        keybd_event(VK_TAB, 0, 0, 0);
                    else if (recvx[4] == 'r')
                        keybd_event(VK_LCONTROL, 0, 0, 0);
                    else if (recvx[4] == 'n')
                        keybd_event(VK_LWIN, 0, 0, 0);
                    else if (recvx[4] == 't')
                        keybd_event(VK_MENU, 0, 0, 0);
                    else if (recvx[4] == 'l')
                        keybd_event(VK_DELETE, 0, 0, 0);
                    else if (recvx[3] == 'h')
                        keybd_event(VK_RSHIFT, 0, 0, 0);
                    else 
                    {
                        keybd_event(VkKeyScanEx(recvx[2], GetKeyboardLayout(0)), 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
                    }
                }
                else
                {
                    if (recvx[3] == 'h')
                        keybd_event(VK_RSHIFT, 0, KEYEVENTF_KEYUP, 0);
                    else if (recvx[4] == 'b')
                        keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
                    else if (recvx[4] == 'r')
                    {
                        keybd_event(VK_LCONTROL, 0, KEYEVENTF_KEYUP, 0);
                        keybd_event(VK_RCONTROL, 0, KEYEVENTF_KEYUP, 0);
                        keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
                    }
                    else if (recvx[4] == 'n')
                        keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
                    else if (recvx[4] == 't')
                        keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
                    else if (recvx[4] == 'l')
                        keybd_event(VK_DELETE, 0, KEYEVENTF_KEYUP, 0);
                    else if (recvx[5] == 'k')
                        keybd_event(VK_BACK, 0, KEYEVENTF_KEYUP, 0);
                    else if (recvx[5] == 's')
                        keybd_event(VK_CAPITAL, 0, KEYEVENTF_KEYUP, 0);
                    else if (recvx[6] == 'r')
                        keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
                    else if (recvx[6] == 'e')
                        keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
                    else
                    {
                        keybd_event(VkKeyScanEx(recvx[2], GetKeyboardLayout(0)), 0, KEYEVENTF_KEYUP, 0);
                    }
                }
                memset(recvx, 0, DEFAULT_BUFLEN);
            }
            else if (recvx[0] == 'K')
            {
                if (recvx[1] == 'D')
                {
                       
                    if (recvx[3] == 's')
                    {
                        Event.type = INPUT_KEYBOARD;
                        Event.ki.dwFlags = KEYEVENTF_SCANCODE;
                        Event.ki.wScan = MapVirtualKey(0x1B, 0);
                        SendInput(1, &Event, sizeof(Event));
                    }
                    else if (recvx[2] == '_')
                    {
                        Event.type = INPUT_KEYBOARD;
                        Event.ki.dwFlags = KEYEVENTF_SCANCODE;
                        Event.ki.wScan = MapVirtualKey(LOBYTE(VkKeyScan(' ')), 0);
                        SendInput(1, &Event, sizeof(Event));
                    }
                    else
                    {
                        Event.type = INPUT_KEYBOARD;
                        Event.ki.dwFlags = KEYEVENTF_SCANCODE;
                        char x = recvx[3];
                        Event.ki.wScan = MapVirtualKey(LOBYTE(VkKeyScan(recvx[2])), 0);
                        SendInput(1, &Event, sizeof(Event));
                    }
                }
                else
                {
                    if (recvx[3] == 's')
                    {
                        Event.type = INPUT_KEYBOARD;
                        Event.ki.dwFlags = KEYEVENTF_SCANCODE;
                        Event.ki.wScan = MapVirtualKey(0x1B, 0);
                        SendInput(1, &Event, sizeof(Event));
                    }
                    else if (recvx[2] == '_')
                    {
                        Event.type = INPUT_KEYBOARD;
                        Event.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
                        Event.ki.wScan = MapVirtualKey(LOBYTE(VkKeyScan(' ')), 0);
                        SendInput(1, &Event, sizeof(Event));
                    }
                    else
                    {
                        Event.type = INPUT_KEYBOARD;
                        Event.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
                        Event.ki.wScan = MapVirtualKey(LOBYTE(VkKeyScan(recvx[2])), 0);
                        SendInput(1, &Event, sizeof(Event));
                    }
                      
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
        //else if (recvx[1]=='')
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
