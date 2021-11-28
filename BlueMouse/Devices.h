#ifndef DEVICES_H
#define DEVICES_H
#include <windows.h>
#include <bluetoothapis.h>
class Devices
{
    private:
        HWND buttons[20];
        BLUETOOTH_DEVICE_INFO m_device_info_arr[20];
    public:
        ULONGLONG getDeviceMac(int number);
        void startSearch(HWND hWnd);
        void deleteDevices();
};
#endif