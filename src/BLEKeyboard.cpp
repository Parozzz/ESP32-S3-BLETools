#include <BLETools.h>

#define UUID_HID_SERVICE (uint16_t)0x1812
#define UUID_HID_BOOT_INPUT (uint16_t)0x2A22
#define UUID_HID_BOOT_OUTPUT (uint16_t)0x2A32
#define UUID_HID_PROTOCOL_MODE (uint16_t)0x2A4E
#define UUID_HID_INFORMATION (uint16_t)0x2A4A
#define UUID_HID_REPORT_MAP (uint16_t)0x2A4B
#define UUID_HID_CONTROL_POINT (uint16_t)0x2A4C
#define UUID_HID_REPORT (uint16_t)0x2A4D
#define UUID_HID_REPORT_DESCRIPTOR (uint16_t)0x2908

#define COUNTRY 0    // The country code for the device.
#define INFO_FLAGS 1 // The HID Class Specification release number to use.
static const uint8_t info[] = {0x11,
                               0x1,
                               COUNTRY,
                               INFO_FLAGS};

#define PROTOCOL_MODE_BOOT 0x0
#define PROTOCOL_MODE_REPORT 0x1
static const uint8_t protocolMode[] = {PROTOCOL_MODE_REPORT};

// Report IDs:
#define REPORT_KEYBOARD_ID 0x01
#define REPORT_MEDIA_KEYS_ID 0x02
static const uint8_t HIDReportDescriptor[] = {
    USAGE_PAGE(1), 0x01, // USAGE_PAGE (Generic Desktop Ctrls)
    USAGE(1), 0x06,      // USAGE (Keyboard)
    COLLECTION(1), 0x01, // COLLECTION (Application)
    // ------------------------------------------------- Keyboard
    REPORT_ID(1), REPORT_KEYBOARD_ID, //   REPORT_ID (1)
                                      //   KEYS MODIFIERS [1 BYTE]
    REPORT_SIZE(1), 0x01,             //   REPORT_SIZE (1)
    REPORT_COUNT(1), 0x08,            //   REPORT_COUNT (8)
    USAGE_PAGE(1), 0x07,              //   USAGE_PAGE (Kbrd/Keypad)
    USAGE_MINIMUM(1), 0xE0,           //   USAGE_MINIMUM (0xE0)
    USAGE_MAXIMUM(1), 0xE7,           //   USAGE_MAXIMUM (0xE7)
    LOGICAL_MINIMUM(1), 0x00,         //   LOGICAL_MINIMUM (0)
    LOGICAL_MAXIMUM(1), 0x01,         //   Logical Maximum (1)
    HIDINPUT(1), 0x02,                //   INPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
                                      //   RESERVED [1 BYTE]
    REPORT_COUNT(1), 0x01,            //   REPORT_COUNT (1) ; 1 byte (Reserved)
    REPORT_SIZE(1), 0x08,             //   REPORT_SIZE (8)
    HIDINPUT(1), 0x01,                //   INPUT (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
                                      //   KEYS [6 BYTES]
    REPORT_COUNT(1), 0x06,            //   REPORT_COUNT (6) ; 6 bytes (Keys)
    REPORT_SIZE(1), 0x08,             //   REPORT_SIZE(8)
    LOGICAL_MINIMUM(1), 0x00,         //   LOGICAL_MINIMUM(0)
    LOGICAL_MAXIMUM(1), 0x65,         //   LOGICAL_MAXIMUM(0x65) ; 101 keys
    USAGE_PAGE(1), 0x07,              //   USAGE_PAGE (Kbrd/Keypad)
    USAGE_MINIMUM(1), 0x00,           //   USAGE_MINIMUM (0)
    USAGE_MAXIMUM(1), 0x65,           //   USAGE_MAXIMUM (0x65)
    HIDINPUT(1), 0x00,                //   INPUT (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)

    REPORT_COUNT(1), 0x05,  //   REPORT_COUNT (5) ; 5 bits (Num lock, Caps lock, Scroll lock, Compose, Kana)
    REPORT_SIZE(1), 0x01,   //   REPORT_SIZE (1)
    USAGE_PAGE(1), 0x08,    //   USAGE_PAGE (LEDs)
    USAGE_MINIMUM(1), 0x01, //   USAGE_MINIMUM (0x01) ; Num Lock
    USAGE_MAXIMUM(1), 0x05, //   USAGE_MAXIMUM (0x05) ; Kana
    HIDOUTPUT(1), 0x02,     //   OUTPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)

    REPORT_COUNT(1), 0x01, //   REPORT_COUNT (1) ; 3 bits (Padding)
    REPORT_SIZE(1), 0x03,  //   REPORT_SIZE (3)
    HIDOUTPUT(1), 0x01,    //   OUTPUT (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)

    END_COLLECTION(0), // END_COLLECTION
    // -------------------------------------------------

    USAGE_PAGE(1), 0x0C, // USAGE_PAGE (Consumer)
    USAGE(1), 0x01,      // USAGE (Consumer Control)
    COLLECTION(1), 0x01, // COLLECTION (Application)
    // ------------------------------------------------- Media Keys
    REPORT_ID(1), REPORT_MEDIA_KEYS_ID, //   REPORT_ID (3)
    USAGE_PAGE(1), 0x0C,                //   USAGE_PAGE (Consumer)
    LOGICAL_MINIMUM(1), 0x00,           //   LOGICAL_MINIMUM (0)
    LOGICAL_MAXIMUM(1), 0x01,           //   LOGICAL_MAXIMUM (1)
    REPORT_SIZE(1), 0x01,               //   REPORT_SIZE (1)
    REPORT_COUNT(1), 0x10,              //   REPORT_COUNT (16)
    USAGE(1), 0xB5,                     //   USAGE (Scan Next Track)     ; bit 0: 1
    USAGE(1), 0xB6,                     //   USAGE (Scan Previous Track) ; bit 1: 2
    USAGE(1), 0xB7,                     //   USAGE (Stop)                ; bit 2: 4
    USAGE(1), 0xCD,                     //   USAGE (Play/Pause)          ; bit 3: 8
    USAGE(1), 0xE2,                     //   USAGE (Mute)                ; bit 4: 16
    USAGE(1), 0xE9,                     //   USAGE (Volume Increment)    ; bit 5: 32
    USAGE(1), 0xEA,                     //   USAGE (Volume Decrement)    ; bit 6: 64
    USAGE(2), 0x23, 0x02,               //   Usage (WWW Home)            ; bit 7: 128
    USAGE(2), 0x94, 0x01,               //   Usage (My Computer) ; bit 0: 1
    USAGE(2), 0x92, 0x01,               //   Usage (Calculator)  ; bit 1: 2
    USAGE(2), 0x2A, 0x02,               //   Usage (WWW fav)     ; bit 2: 4
    USAGE(2), 0x21, 0x02,               //   Usage (WWW search)  ; bit 3: 8
    USAGE(2), 0x26, 0x02,               //   Usage (WWW stop)    ; bit 4: 16
    USAGE(2), 0x24, 0x02,               //   Usage (WWW back)    ; bit 5: 32
    USAGE(2), 0x83, 0x01,               //   Usage (Media sel)   ; bit 6: 64
    USAGE(2), 0x8A, 0x01,               //   Usage (Mail)        ; bit 7: 128
    HIDINPUT(1), 0x02,                  //   INPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    END_COLLECTION(0)                   // END_COLLECTION */
};


void BleKeyboard::setDebugSerial(Print *debugSerial)
{
    _debugSerial = debugSerial;
    _outputReportCallbacks.setDebugSerial(debugSerial);
}

void BleKeyboard::init(NimBLEServer* bleServer)
{
    if(_debugSerial != nullptr)
    {
        _debugSerial->println("BleKeyboard - Init");
    }

    _bleServer = bleServer;

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
