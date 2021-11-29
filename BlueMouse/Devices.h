#ifndef DEVICES_H
#define DEVICES_H
#include <windows.h>
#include <bluetoothapis.h>
class Devices
{
    private:
        HWND buttons[20];
        BLUETOOTH_DEVICE_INFO m_device_info_arr[20];
        boolean isall = false;
    public:
        BLUETOOTH_DEVICE_INFO getBluetoothDeviceInfo(int number);
        boolean isAll();
        void startSearch(HWND hWnd);
        void deleteDevices();
};
#endif