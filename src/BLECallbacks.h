#ifndef BLE_KEYBOARD_CALLBACKS_H
#define BLE_KEYBOARD_CALLBACKS_H

#include <NimBLEDevice.h>
#include <USBCDC.h>

// typedef void (*BooleanCallback)(bool);

class BleSerialCallbacks : public NimBLECharacteristicCallbacks
{
public:
    void setDebugSerial(Print *debugPrint = nullptr)
    {
        _debugPrint = debugPrint;
    }

    void onWrite(NimBLECharacteristic *pCharacteristic)
    {
        if (_debugPrint != nullptr)
        {
            _debugPrint->println("BLESerial - recv.");
            _debugPrint->println(pCharacteristic->getValue().c_str());
        }

        _available = true;
    }

    bool available()
    {
        bool ret = _available;
        _available = false;
        return ret;
    }

private:
    Print *_debugPrint;

    bool _available = false;
};

class BleServerCallbacks : public NimBLEServerCallbacks
{
public:
    void setDebugSerial(Print *debugPrint = nullptr)
    {
        _debugPrint = debugPrint;
    }

    void onConnect(BLEServer *pServer)
    {
        if (_debugPrint != nullptr)
        {
            _debugPrint->println("BLE - OnConnect");
        }

        connect = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        if (_debugPrint != nullptr)
        {
            _debugPrint->println("BLE - OnDisconnect");
        }

        connect = false;
    }

    bool connect = false;

private:
    Print *_debugPrint;
};

class BleKeyboardReportCallbacks : public NimBLECharacteristicCallbacks
{
public:
    void setDebugSerial(Print *debugPrint = nullptr)
    {
        _debugPrint = debugPrint;
    }

    void onWrite(NimBLECharacteristic *pCharacteristic)
    {
        if (_debugPrint != nullptr)
        {
            _debugPrint->println("Output written.");
        }
    }

private:
    Print *_debugPrint;
};

class BleSecurityCallbacks : public NimBLESecurityCallbacks
{
public:
    void setDebugSerial(Print *debugPrint = nullptr)
    {
        _debugPrint = debugPrint;
    }

    uint32_t onPassKeyRequest()
    {
        if (_debugPrint != nullptr)
        {
            _debugPrint->println("BLE - PassKeyRequested");
        }

        return 1234;
    };

    void onPassKeyNotify(uint32_t pass_key)
    {
        if (_debugPrint != nullptr)
        {
            _debugPrint->print("BLE - PassKeyNotify. Key=");
            _debugPrint->println(pass_key);
        }
    }

    bool onSecurityRequest()
    {
        if (_debugPrint != nullptr)
        {
            _debugPrint->println("BLE - SecurityRequest");
        }

        return true;
    }

    void onAuthenticationComplete(ble_gap_conn_desc *desc)
    {
        if (_debugPrint != nullptr)
        {
            _debugPrint->println("BLE - AuthenticationComplete");
        }

        authDone = true;
    }

    bool onConfirmPIN(uint32_t pin)
    {
        if (_debugPrint != nullptr)
        {
            _debugPrint->print("BLE - ConfirmPIN. Pin = ");
            _debugPrint->println(pin);
        }

        return true;
    }

    bool authDone = false;

private:
    Print *_debugPrint;
};

#endif