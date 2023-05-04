#ifndef BLE_TOOLS_H
#define BLE_TOOLS_H

#include <BLECallbacks.h>
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#include <StreamUtils.h>

#define APPEARANCE_GENERIC 0x03C0
#define APPEARANCE_KEYBOARD 0x03C1
#define APPEARANCE_MOUSE 0x03C2
#define APPEARANCE_JOYSTICK 0x03C3
#define APPEARANCE_GAMEPAD 0x03C4
#define APPEARANCE_DIGITIZER_TABLET 0x03C5
#define APPEARANCE_CARD_READER 0x03C6
#define APPEARANCE_DIGITAL_PEN 0x03C7
#define APPEARANCE_BARCODE_SCANNER 0x03C8
#define APPEARANCE_TOUCHPAD 0x03C9

#define SIG 0x02       // The vendor ID source number.
#define VID 0xe502     // The vendor ID number.
#define PID 0xA111     // The product ID number.
#define VERSION 0x0210 // The produce version number.
static const uint8_t pnp[] = {SIG,
                              (uint8_t)(VID >> 8),
                              (uint8_t)VID,
                              (uint8_t)(PID >> 8),
                              (uint8_t)PID,
                              (uint8_t)(VERSION >> 8),
                              (uint8_t)VERSION};

#define UUID_DEVICE_INFO_SERVICE (uint16_t)0x180A
#define UUID_MODEL_NUMER (uint16_t)0x2A24
#define UUID_SERIAL_NUMBER (uint16_t)0x2A25
#define UUID_FW_REVISION (uint16_t)0x2A26
#define UUID_HW_REVISION (uint16_t)0x2A27
#define UUID_SW_REVISION (uint16_t)0x2A28
#define UUID_MANUFACTURER (uint16_t)0x2A29
#define UUID_PNP (uint16_t)0x2A50

//ATT Maximum Transmission Unit (MTU) is the maximum length of an ATT packet
#define BLE_MTU 512

class BleTools;

typedef struct keyboard_report
{
    uint8_t modifiersKeys = 0;
    uint8_t reserved = 0;
    uint8_t keyCodes[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
} keyboard_report;

class BleSerial
{
public:
    BleSerial() {}

    void setDebugSerial(Print* debugSerial = nullptr);

    void init(NimBLEServer *bleServer);

    void loop();

    StringStream* getReceive()
    {
        return &_rxStream;
    }

    StringStream* getSend()
    {
        return &_txStream;
    }

private:
    Print *_debugSerial;

    NimBLEServer *_bleServer;

    NimBLECharacteristic* _tx;
    NimBLECharacteristic* _rx;
    BleSerialCallbacks _rxCallbacks;

    StringStream _rxStream;

    uint32_t _lastTrasmission = 0;
    StringStream _txStream;
};

class BleBatteryLevel
{
public:
    BleBatteryLevel() {}

    void setDebugSerial(Print *debugSerial = nullptr);

    void init(NimBLEServer *bleServer, uint8_t percentage = 100);

    uint8_t getPercentage();
    void setPercentage(uint8_t status);

private:
    Print *_debugSerial;

    NimBLEServer *_bleServer;

    NimBLECharacteristic *_batteryLevel;
    uint8_t _percentage;
};

class BleKeyboard
{
public:
    BleKeyboard() {}

    void setDebugSerial(Print *debugSerial = nullptr);

    void init(NimBLEServer *bleServer);

    void keyPress(uint8_t key);
    void keyRelease(uint8_t key);
    void keyClick(uint8_t key)
    {
        keyPress(key);
        keyRelease(key);
    }

    void modifierPress(uint8_t modifier);
    void modifierRelease(uint8_t modifier);

    void releaseAll();

private:
    void _sendKeyReport();
    bool _keyIsPressed(uint8_t key);

    Print *_debugSerial;

    NimBLEServer *_bleServer;
    BleKeyboardReportCallbacks _outputReportCallbacks;

    NimBLECharacteristic *_inputReport;
    NimBLECharacteristic *_outputReport;
    NimBLECharacteristic *_inputMediaKeysReport;

    keyboard_report _keysReport;
    uint8_t _keyModifiersPressed;
    uint8_t _keyPressedArray[6];
};

class BleTools
{
public:
    BleTools(const char *deviceName = "S3 BLE")
    {
        _deviceName = deviceName;
    }

    void setDebugSerial(Print *debugSerial)
    {
        _debugSerial = debugSerial;
        _serverCallbacks.setDebugSerial(debugSerial);
        _securityCallbacks.setDebugSerial(debugSerial);
    }

    NimBLEServer *getServer()
    {
        return _bleServer;
    }

    const char *getDeviceName()
    {
        return _deviceName;
    }

    void init(const char *manufacturer)
    {
        if (_debugSerial != nullptr)
        {
            _debugSerial->println("BleTools - Init");
        }

        NimBLEDevice::init(_deviceName);
        NimBLEDevice::setPower(ESP_PWR_LVL_P9);
        NimBLEDevice::setMTU(BLE_MTU);

        //========== SETUP AUTH ==========
        NimBLEDevice::setSecurityAuth(ESP_LE_AUTH_REQ_SC_BOND);
        NimBLEDevice::setSecurityPasskey(1234);
        NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_YESNO);
        NimBLEDevice::setSecurityInitKey(BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID);
        NimBLEDevice::setSecurityCallbacks(&_securityCallbacks);
        //========== SETUP AUTH ==========

        _bleServer = BLEDevice::createServer();
        _bleServer->advertiseOnDisconnect(true);
        _bleServer->setCallbacks(&_serverCallbacks);

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

    BleKeyboard *initKeyboard()
    {
        _keyboard.setDebugSerial(_debugSerial);
        _keyboard.init(_bleServer);
        return &_keyboard;
    }

    BleBatteryLevel *initBatteryLevel(uint8_t percentage = 100)
    {
        _batteryLevel.setDebugSerial(_debugSerial);
        _batteryLevel.init(_bleServer, percentage);
        return &_batteryLevel;
    }

    BleSerial *initSerial()
    {
        _serial.setDebugSerial(_debugSerial);
        _serial.init(_bleServer);
        return &_serial;
    }

    void loop()
    {   
        _serial.loop();

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

    void startAdvertising(uint32_t uuid, uint16_t appearance = APPEARANCE_GENERIC)
    {
        if (_debugSerial != nullptr)
        {
            _debugSerial->println("BleTools - Start Advertising");
        }
        
        if (_advertising == nullptr)
        {
            _advertising = NimBLEDevice::getAdvertising();
        }

        _advertising->setName(_deviceName);
        _advertising->addServiceUUID(NimBLEUUID(uuid));
        _advertising->setAppearance(appearance);
        _advertising->start();
    }

    void stopAdvertising()
    {
        if (_debugSerial != nullptr)
        {
            _debugSerial->println("BleTools - Stop Advertising");
        }

        _advertising->stop();
    }

    bool connected = false;
    bool authed = false;
    bool ready = false;

private:
    const char *_deviceName;

    Print *_debugSerial;

    BleKeyboard _keyboard;
    BleBatteryLevel _batteryLevel;
    BleSerial _serial;

    NimBLEServer *_bleServer;
    NimBLEAdvertising *_advertising;

    BleServerCallbacks _serverCallbacks;
    BleSecurityCallbacks _securityCallbacks;
};

#endif