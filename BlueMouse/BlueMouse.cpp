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
#include <sstream>
#include <iomanip>
#include "Devices.h"
#include "Client.h"
#pragma comment (lib, "ws2_32.lib")
#pragma comment(lib, "bthprops.lib")
#define DEFAULT_BUFLEN 7
#define COLUMN 20

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
Devices devices;
Client client;
SOCKET sock;
char recvx[DEFAULT_BUFLEN] = "";
HKL keyboard = LoadKeyboardLayoutA("00000409", KLF_ACTIVATE);
HWND button;
HWND buttons[80];
bool isRecieveArr[80] = { false };
int current = 81;
int maxindex = 0;
HBRUSH color = CreateSolidBrush(RGB(0, 0, 0));
int x = 10;

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
        L"BlueMouse",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        100, 50, 1280, 720,
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

void startGetCoordinates(WORD param, HWND hWnd) 
{
    InvalidateRect(hWnd, NULL, TRUE);
    isRecieveArr[param] = true;
    if (client.startClient(devices, param))
    {
        if (isRecieveArr[param])
        {
            sock = client.getSock(param);
            SetTimer(hWnd, 1, 20, NULL);
            while (isRecieveArr[param])
            {
                int res = 0;
                int recvbuflen = DEFAULT_BUFLEN;
                res = recv(sock, recvx, recvbuflen, 0);
            }
            KillTimer(hWnd, 1);
        }
        client.sendClose(param);
    }
    InvalidateRect(hWnd, NULL, TRUE);
    isRecieveArr[param] = false;
}

void addButtons()
{
    //SetTimer(hWnd, 2, 20, NULL);
    devices.startSearch();
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case WM_CREATE:
    {
        std::fill_n(isRecieveArr, 80, false);
        button = CreateWindow(L"BUTTON", L"SearchBluetooth", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 200, 30, hWnd, (HMENU)81, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
    }
    break;

    case WM_COMMAND:
    {
        current = LOWORD(wParam);
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
        {
            if (LOWORD(wParam) == 81)
            {
                /*for (int i = 0; i < 20; i++)
                {
                    if (isRecieveArr[i])
                        client.sendClose(i);
                }
                std::fill_n(isRecieveArr, 20, false);
                addButtons(hWnd);*/
                x = 10;
                current = 82;
                SetTimer(hWnd, 2, 20, NULL);
            }
            else if ((LOWORD(wParam) >= 0)&&(LOWORD(wParam) < 80))
            {
                if (!isRecieveArr[LOWORD(wParam)])
                {
                    for (int i = 0; i < maxindex; i++)
                    {
                        if (isRecieveArr[i])
                            client.sendClose(i);
                    }
                    std::fill_n(isRecieveArr, maxindex, false);
                    std::thread thr(startGetCoordinates, LOWORD(wParam), hWnd);
                    thr.detach();
                }
            }
        }
        SetFocus(hWnd);
        }
        break;
    }
    break;

    case WM_TIMER:
        if (strlen(recvx) == 7) {
            if (recvx[0] == 'K')
            {
                INPUT Event = { 0 };
                if (recvx[1] == 'D')
                {
                    if (recvx[3] == 'm')
                    {
                        POINT pt;
                        GetCursorPos(&pt);
                        ScreenToClient(hWnd, &pt);
                        mouse_event(MOUSEEVENTF_RIGHTDOWN, pt.x, pt.y, 0, 0);
                    }
                    else {
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
                }
                else
                {
                    if (recvx[3] == 'm')
                    {
                        POINT pt;
                        GetCursorPos(&pt);
                        ScreenToClient(hWnd, &pt);
                        mouse_event(MOUSEEVENTF_RIGHTUP, pt.x, pt.y, 0, 0);
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
                /*if (recvx[4] == 'D')
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
                else*/
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
            INPUT Event = { 0 };
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
            for (int i = 0; i < maxindex; i++)
                if (isRecieveArr[i])
                    client.close(i);
                std::fill_n(isRecieveArr, maxindex, false);
                KillTimer(hWnd, 1);
                memset(recvx, 0, DEFAULT_BUFLEN);
            }
        }
        else if (wParam == 2)
        {
            if (current == 82)
            {
                current = 81;
                InvalidateRect(hWnd, NULL, TRUE);
                for (int i = 0; i < maxindex; i++)
                {
                    if (isRecieveArr[i])
                        client.sendClose(i);
                }
                for (int i = 0; i < maxindex; i++)
                {
                    DestroyWindow(buttons[i]);
                    buttons[i] = NULL;
                }
                maxindex = 0;
                devices.deleteDevices();
                std::fill_n(isRecieveArr, maxindex, false);
                std::thread thr(addButtons);
                thr.detach();
            }
            else if (devices.maxIndex()==0 || maxindex<=devices.maxIndex())
            {
                if (devices.isExist(maxindex))//while(?) для "сразу"
                {
                    int y = maxindex % COLUMN;
                    LPCWSTR lp = devices.getBluetoothDeviceInfo(maxindex).szName;
                    LPCWSTR p = lp;
                    if (devices.getBluetoothDeviceInfo(maxindex).szName[0] != L'\0')
                        buttons[maxindex] = CreateWindow(L"BUTTON", devices.getBluetoothDeviceInfo(maxindex).szName, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, x, 30 + ((y + 1) * 30), 200, 30, hWnd, (HMENU)maxindex, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
                    else
                    {
                       
                        std::string nameMac = "";
                        for (int i = 0; i < 6; i++)
                        {
                            std::stringstream ss;
                            ss << std::hex;
                            ss << std::setw(2) << std::setfill('0') << (int)devices.getBluetoothDeviceInfo(maxindex).Address.rgBytes[i];
                            nameMac = nameMac + ss.str();
                            if (i != 5)
                                nameMac = nameMac + ":";
                        }
                       
                        std::wstring wsTmp(nameMac.begin(), nameMac.end());
                        buttons[maxindex] = CreateWindow(L"BUTTON", (LPCWSTR)wsTmp.c_str(), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, x, 30 + ((y + 1) * 30), 200, 30, hWnd, (HMENU)maxindex, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
                        //buttons[maxindex] = CreateWindow(L"BUTTON", L"(Noname)", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, x, 30 + ((y + 1) * 30), 200, 30, hWnd, (HMENU)maxindex, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
                    }
                    maxindex++;
                    if ((maxindex % COLUMN) == 0)
                        x = x + 280;
                }
            }
            else
            {
                KillTimer(hWnd, 2);
            }
        }
        break;

        case WM_KEYDOWN:
            if (wParam == VK_RETURN)
            {
                if (current == 81)
                    SetFocus(button);
                SendMessage(hWnd, WM_COMMAND, current, 0);
            }
            else if (wParam == VK_RIGHT)
            {
                if (current != 81)
                {
                    if (current + COLUMN < maxindex)
                        current += COLUMN;
                    else
                        current = maxindex - 1;
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
            else if (wParam == VK_LEFT)
            {
                if (current != 81 && current >= COLUMN)
                {
                    current -= COLUMN;
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
            else if (wParam == VK_DOWN)
            {
                if (current == 81 && buttons[0] != NULL)
                {
                    current = 0;
                }
                else
                {
                    if (buttons[current + 1] != NULL)
                        current++;
                }
                InvalidateRect(hWnd, NULL, TRUE);
            }
            else if (wParam == VK_UP)
            {
                if (current!=81)
                    if (current == 0)
                    {
                        current = 81;
                    }
                    else
                    {
                        current--;
                    }
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;

        case WM_PAINT:
        {

            if (isRecieveArr[current] == true)
            {
                DeleteObject(color);
                color = CreateSolidBrush(RGB(255, 0, 0));
            }
            else
            {
                DeleteObject(color);
                color = CreateSolidBrush(RGB(0, 0, 0));
            }
            PAINTSTRUCT ps;
            RECT rc;
            if (current == 81)
                SetRect(&rc, 250, 20, 260, 30);
            else
            {
                RECT winr;
                RECT r;
                GetWindowRect(hWnd, &winr);
                GetWindowRect(buttons[current], &r);
                SetRect(&rc, r.right-winr.left+30, r.top-winr.top-20, r.right-winr.left+40, r.top-winr.top-10);
            }
            HDC hdc = BeginPaint(hWnd, &ps);
            FillRect(hdc, &rc, color);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_DESTROY:
    {
        //UnloadKeyboardLayout(keyboard);
        for (int i = 0; i<maxindex; i++)
            if (isRecieveArr[i])
                client.sendClose(i);
        PostQuitMessage(0);
    }
    break;

    default: {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    }
    return 0;
}
