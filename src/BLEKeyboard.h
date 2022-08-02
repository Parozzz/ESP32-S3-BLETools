#ifndef BLE_KEYBOARD_H
#define BLE_KEYBOARD_H

#include <BLEKeyboarcCallbacks.h>
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>
#include <USBCDC.h>

#define UUID_DEVICE_INFO_SERVICE (uint16_t)0x180A
#define UUID_MODEL_NUMER (uint16_t)0x2A24
#define UUID_SERIAL_NUMBER (uint16_t)0x2A25
#define UUID_FW_REVISION (uint16_t)0x2A26
#define UUID_HW_REVISION (uint16_t)0x2A27
#define UUID_SW_REVISION (uint16_t)0x2A28
#define UUID_MANUFACTURER (uint16_t)0x2A29
#define UUID_PNP (uint16_t)0x2A50

#define UUID_HID_SERVICE (uint16_t)0x1812
#define UUID_HID_BOOT_INPUT (uint16_t)0x2A22
#define UUID_HID_BOOT_OUTPUT (uint16_t)0x2A32
#define UUID_HID_PROTOCOL_MODE (uint16_t)0x2A4E
#define UUID_HID_INFORMATION (uint16_t)0x2A4A
#define UUID_HID_REPORT_MAP (uint16_t)0x2A4B
#define UUID_HID_CONTROL_POINT (uint16_t)0x2A4C
#define UUID_HID_REPORT (uint16_t)0x2A4D
#define UUID_HID_REPORT_DESCRIPTOR (uint16_t)0x2908

#define UUID_BATTERY_SERVICE (uint16_t)0x180F
#define UUID_BATTERY_LEVEL (uint16_t)0x2A19

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

typedef struct keyboard_report
{
    uint8_t modifiersKeys = 0;
    uint8_t reserved = 0;
    uint8_t keyCodes[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
} keyboard_report;

class BleKeyboard
{
public:
    BleKeyboard(const char *deviceName = "ESP32-S3 Keyboard");

    void setDebugPrint(Print *debugPrint = nullptr);

    void initDEVICE();
    void initINFO(const char *manufacturer);
    void initHID();
    void initBATTERY(uint8_t level);
    void initADVERTISING(uint32_t UUID);

    void stopAdvertising();
    void startAdvertising();

    void loop();

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

    const char* getDeviceName()
    {
        return _deviceName;
    }

    bool connected = false;
    bool authed = false;
    bool ready = false;
private:
    void _sendKeyReport();
    bool _keyIsPressed(uint8_t key);

    const char * _deviceName;

    Print *_debugPrint;
    
    NimBLEServer *_bleServer;
    BleKeyboardServerCallbacks _serverCallbacks;
    BleKeyboardSecurityCallbacks _securityCallbacks;
    BleKeyboardReportCallbacks _outputReportCallbacks;

    NimBLECharacteristic *_inputReport;
    NimBLECharacteristic *_outputReport;
    NimBLECharacteristic *_inputMediaKeysReport;
    NimBLECharacteristic *_batteryLevel;

    NimBLEAdvertising *_advertising;

    keyboard_report _keysReport;
    uint8_t _keyModifiersPressed;
    uint8_t _keyPressedArray[6];
};

#endif