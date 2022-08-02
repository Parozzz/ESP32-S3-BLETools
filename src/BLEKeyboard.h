#ifndef BLEKeyboard
#define BLEKeyboard

#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>

class BleKeyboard
{
public:
    void initINFO(const char* manufacturer);
    void initHID(const char* deviceName = "ESP32-S3 Keyboard");
    void initBATTERY(uint8_t level);

    

    
    void startServices();

private:
    NimBLECharacteristic *_inputReport;
    NimBLECharacteristic *_outputReport;
    NimBLECharacteristic *_inputMediaKeysReport;
    NimBLEAdvertising *pAdvertising;

};

#endif