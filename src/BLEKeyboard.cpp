#include <BLEKeyboard.h>

BleKeyboard::BleKeyboard(const char *deviceName)
{
    _deviceName = deviceName;
}

void BleKeyboard::setDebugPrint(Print *debugPrint)
{
    _debugPrint = debugPrint;

    _serverCallbacks.setUSBSerial(debugPrint);
    _securityCallbacks.setUSBSerial(debugPrint);
    _outputReportCallbacks.setUSBSerial(debugPrint);
}

void BleKeyboard::initDEVICE()
{
    NimBLEDevice::init(_deviceName);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    _bleServer = BLEDevice::createServer();
    _bleServer->advertiseOnDisconnect(true);
    _bleServer->setCallbacks(&_serverCallbacks);

    NimBLEDevice::setSecurityAuth(ESP_LE_AUTH_REQ_SC_MITM_BOND);
    NimBLEDevice::setSecurityPasskey(1234);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);
    NimBLEDevice::setSecurityInitKey(BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID);
    NimBLEDevice::setSecurityCallbacks(&_securityCallbacks);
}

void BleKeyboard::initINFO(const char *manufacturer)
{
    BLEService *pDeviceInfoService = _bleServer->createService(NimBLEUUID(UUID_DEVICE_INFO_SERVICE));
    // pDeviceInfoService->createCharacteristic(MODEL_NUMER_UUID, NIMBLE_PROPERTY::READ)->setValue(MODEL_NUMER);
    // pDeviceInfoService->createCharacteristic(SERIAL_NUMBER_UUID, NIMBLE_PROPERTY::READ)->setValue(SERIAL_NUMBER);
    // pDeviceInfoService->createCharacteristic(FW_REVISION_UUID, NIMBLE_PROPERTY::READ)->setValue(FW_REVISION);
    // pDeviceInfoService->createCharacteristic(HW_REVISION_UUID, NIMBLE_PROPERTY::READ)->setValue(HW_REVISION);
    // pDeviceInfoService->createCharacteristic(SW_REVISION_UUID, NIMBLE_PROPERTY::READ)->setValue(SW_REVISION);
    pDeviceInfoService->createCharacteristic(UUID_MANUFACTURER, NIMBLE_PROPERTY::READ)->setValue(manufacturer);
    pDeviceInfoService->createCharacteristic(UUID_PNP, NIMBLE_PROPERTY::READ)->setValue(pnp, sizeof(pnp));

    pDeviceInfoService->start();
}

void BleKeyboard::initHID()
{
    BLEService *pHIDService = _bleServer->createService(NimBLEUUID(UUID_HID_SERVICE));
    pHIDService->createCharacteristic(UUID_HID_BOOT_INPUT, NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ, 8);
    pHIDService->createCharacteristic(UUID_HID_BOOT_OUTPUT, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR, 1);

    pHIDService->createCharacteristic(UUID_HID_INFORMATION, NIMBLE_PROPERTY::READ, 4)->setValue(info, sizeof(info));
    pHIDService->createCharacteristic(UUID_HID_REPORT_MAP, NIMBLE_PROPERTY::READ)->setValue(HIDReportDescriptor, sizeof(HIDReportDescriptor));
    pHIDService->createCharacteristic(UUID_HID_CONTROL_POINT, NIMBLE_PROPERTY::WRITE_NR, 1)->setValue(1);
    pHIDService->createCharacteristic(UUID_HID_PROTOCOL_MODE, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE_NR)->setValue(protocolMode, sizeof(protocolMode));

    _inputReport = pHIDService->createCharacteristic(UUID_HID_REPORT, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ_ENC);
    _inputReport->createDescriptor(UUID_HID_REPORT_DESCRIPTOR, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC)->setValue({REPORT_KEYBOARD_ID, 0x01});

    _outputReport = pHIDService->createCharacteristic(UUID_HID_REPORT, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_ENC);
    _outputReport->createDescriptor(UUID_HID_REPORT_DESCRIPTOR, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_ENC)->setValue({REPORT_KEYBOARD_ID, 0x02});
    _outputReport->setCallbacks(&_outputReportCallbacks);

    _inputMediaKeysReport = pHIDService->createCharacteristic(UUID_HID_REPORT, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ_ENC);
    _inputMediaKeysReport->createDescriptor(UUID_HID_REPORT_DESCRIPTOR, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC)->setValue({REPORT_MEDIA_KEYS_ID, 0x01});

    pHIDService->start();
}

void BleKeyboard::initBATTERY(uint8_t level)
{
    BLEService *pBatteryService = _bleServer->createService(NimBLEUUID(UUID_BATTERY_SERVICE));

    _batteryLevel = pBatteryService->createCharacteristic(UUID_BATTERY_LEVEL, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    _batteryLevel->setValue(&level, 1);

    NimBLE2904 *pBatteryLevelDescriptor = (NimBLE2904 *)_batteryLevel->createDescriptor((uint16_t)0x2904);
    pBatteryLevelDescriptor->setFormat(NimBLE2904::FORMAT_UINT8);
    pBatteryLevelDescriptor->setNamespace(1);
    pBatteryLevelDescriptor->setUnit(0x27ad);

    pBatteryService->start();
}

void BleKeyboard::initADVERTISING(uint32_t UUID)
{
    _advertising = NimBLEDevice::getAdvertising();
    _advertising->setName(_deviceName);
    _advertising->addServiceUUID(NimBLEUUID(UUID));
    _advertising->setAppearance(APPEARANCE_KEYBOARD);
}

void BleKeyboard::loop()
{
    connected = _serverCallbacks.connect;
    if (!connected)
    {
        authed = false;
    }

    if (_securityCallbacks.authDone)
    {
        _securityCallbacks.authDone = false;

        authed = true;
    }

    ready = connected && authed;
}

void BleKeyboard::stopAdvertising()
{
    _advertising->start();
}

void BleKeyboard::startAdvertising()
{
    _advertising->start();
}

void BleKeyboard::_sendKeyReport()
{
    _keysReport.modifiersKeys = _keyModifiersPressed;
    for (int c = 0; c < 6; c++)
    {
        _keysReport.keyCodes[c] = _keyPressedArray[c];
    }

    _inputReport->setValue(_keysReport);
    _inputReport->notify();
}

bool BleKeyboard::_keyIsPressed(uint8_t key)
{
    for (int x = 0; x < 6; x++)
    {
        if (_keyPressedArray[x] = key)
        {
            return true;
        }
    }

    return false;
}

void BleKeyboard::keyPress(uint8_t key)
{
    if (!_keyIsPressed(key))
    {
        for (int x = 0; x < 6; x++)
        {
            if (_keyPressedArray[x] = 0)
            {
                _keyPressedArray[x] = key;
            }
        }
    }

    _sendKeyReport();
}

void BleKeyboard::keyRelease(uint8_t key)
{
    if (_keyIsPressed(key))
    {
        for (int x = 0; x < 6; x++)
        {
            if (_keyPressedArray[x] = key)
            {
                _keyPressedArray[x] = 0;
            }
        }
    }

    _sendKeyReport();
}

void BleKeyboard::modifierPress(uint8_t modifier)
{
    _keyModifiersPressed |= modifier;
    _sendKeyReport();
}

void BleKeyboard::modifierRelease(uint8_t modifier)
{
    _keyModifiersPressed &= ~modifier;
    _sendKeyReport();
}

void BleKeyboard::releaseAll()
{
    _keyModifiersPressed = 0;

    _keyModifiersPressed = 0;
    for (int x = 0; x < 6; x++)
    {
        _keyPressedArray[x] = 0;
    }

    _sendKeyReport();
}
