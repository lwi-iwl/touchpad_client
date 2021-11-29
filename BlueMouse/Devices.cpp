#include "Devices.h"
void Devices::deleteDevices()
{
	for (int i = 0; i < 20; i++)
	{
		if (buttons[i] != NULL)
			DestroyWindow(buttons[i]);
	}
}

void Devices::startSearch(HWND hWnd)
{
    BLUETOOTH_FIND_RADIO_PARAMS m_bt_find_radio = {
            sizeof(BLUETOOTH_FIND_RADIO_PARAMS)
    };

    BLUETOOTH_RADIO_INFO m_bt_info = {
        sizeof(BLUETOOTH_RADIO_INFO),
        0,
    };
    BLUETOOTH_DEVICE_SEARCH_PARAMS m_search_params =
    {
        sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS), 1, 0, 1, 1, 1, 15, NULL
    };

    BLUETOOTH_DEVICE_INFO m_device_info = {
        sizeof(BLUETOOTH_DEVICE_INFO),
        0,
    };

    HANDLE m_radio = NULL;
    HBLUETOOTH_RADIO_FIND m_bt = NULL;
    HBLUETOOTH_DEVICE_FIND m_bt_dev = NULL;
    m_bt = BluetoothFindFirstRadio(&m_bt_find_radio, &m_radio);
    m_search_params.hRadio = m_radio;
    m_search_params.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
    m_search_params.fReturnAuthenticated = false;
    m_search_params.fReturnRemembered = true;
    m_search_params.fReturnUnknown = true;
    m_search_params.fReturnConnected = false;
    m_search_params.fIssueInquiry = true;
    m_search_params.cTimeoutMultiplier = 1;
    ::ZeroMemory(&m_device_info, sizeof(BLUETOOTH_DEVICE_INFO));
    m_device_info.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);

    m_bt_dev = BluetoothFindFirstDevice(&m_search_params, &m_device_info);
    buttons[0] = CreateWindow(L"BUTTON", m_device_info.szName, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 60, 200, 30, hWnd, (HMENU)0, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
    m_device_info_arr[0] = m_device_info;
    int m_device_id = 0;
    while (BluetoothFindNextDevice(m_bt_dev, &m_device_info)) {
        m_device_id++;
        buttons[m_device_id] = CreateWindow(L"BUTTON", m_device_info.szName, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 30 + ((m_device_id + 1) * 30), 200, 30, hWnd, (HMENU)m_device_id, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        m_device_info_arr[m_device_id] = m_device_info;
    }
    isall = true;
    BluetoothFindDeviceClose(m_bt_dev);
}


boolean Devices::isAll()
{
    return isall;
}

BLUETOOTH_DEVICE_INFO Devices::getBluetoothDeviceInfo(int number)
{
    return m_device_info_arr[number];
}