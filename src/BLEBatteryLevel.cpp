#include <BLETools.h>

#define UUID_BATTERY_SERVICE (uint16_t)0x180F
#define UUID_BATTERY_LEVEL (uint16_t)0x2A19

void BleBatteryLevel::setDebugSerial(Print *debugSerial)
{
    _debugSerial = debugSerial;
}

void BleBatteryLevel::init(NimBLEServer* bleServer, uint8_t percentage)
{
    if (_debugSerial != nullptr)
    {
        _debugSerial->println("BleBatteryLevel - Init");
    }
    
    _bleServer = bleServer;
    _percentage = percentage;

    BLEService *pBatteryService = _bleServer->createService(NimBLEUUID(UUID_BATTERY_SERVICE));

    _batteryLevel = pBatteryService->createCharacteristic(UUID_BATTERY_LEVEL, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
    _batteryLevel->setValue(&percentage, 1);

    NimBLE2904 *pBatteryLevelDescriptor = (NimBLE2904 *)_batteryLevel->createDescriptor((uint16_t)0x2904);
    pBatteryLevelDescriptor->setFormat(NimBLE2904::FORMAT_UINT8);
    pBatteryLevelDescriptor->setNamespace(1);
    pBatteryLevelDescriptor->setUnit(0x27ad);

    pBatteryService->start();
}

uint8_t BleBatteryLevel::getPercentage()
{
    return _percentage;
}

void BleBatteryLevel::setPercentage(uint8_t percentage)
{
    _percentage = percentage;
}