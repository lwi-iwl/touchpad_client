#ifndef DEVICES_H
#define DEVICES_H
#include <windows.h>
#include <bluetoothapis.h>
#include <iostream>
class Devices
{
    private:
        BLUETOOTH_DEVICE_INFO m_device_info_arr[80];
        boolean isexist[80];
        int maxindex = 0;
    public:
        BLUETOOTH_DEVICE_INFO getBluetoothDeviceInfo(int number);
        int maxIndex();
        boolean isExist(int number);
        void startSearch();
        void deleteDevices();
};
#endif