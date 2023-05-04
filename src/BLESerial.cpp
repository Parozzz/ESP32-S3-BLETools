#include <BLETools.h>

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define DELAY_BETWEEN_TRASMISSION 25

void BleSerial::setDebugSerial(Print *debugSerial)
{
    _debugSerial = debugSerial;
    _rxCallbacks.setDebugSerial(debugSerial);
}

void BleSerial::init(NimBLEServer *bleServer)
{
    if (_debugSerial != nullptr)
    {
        _debugSerial->println("BleSerial - Init");
    }

    _bleServer = bleServer;

    NimBLEService *aurtService = _bleServer->createService(NimBLEUUID(SERVICE_UUID));

    _tx = aurtService->createCharacteristic(UUID_TX,  NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);

    _rx = aurtService->createCharacteristic(UUID_RX, NIMBLE_PROPERTY::WRITE_NR);
    _rx->setCallbacks(&_rxCallbacks);

    aurtService->start();
}

void BleSerial::loop()
{      
    int available = _txStream.available();
    if(available && (_lastTrasmission == 0 || (millis() - _lastTrasmission) > DELAY_BETWEEN_TRASMISSION ))
    {   
        _lastTrasmission = millis();

        available = min(available, BLE_MTU);
        
        char txBuffer[BLE_MTU];
        uint16_t txLen = _txStream.readBytes(txBuffer, available);

        if(_debugSerial != nullptr)
        {
            _debugSerial->printf("BLEUart - Available %d, Sending %d \n", available, txLen);
        }

        _tx->setValue((uint8_t*) txBuffer, txLen);
        _tx->notify();
    }

    if(_rxCallbacks.available())
    {
        _rxStream.print(_rx->getValue().c_str());
    }
}