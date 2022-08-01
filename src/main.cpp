#include <Arduino.h>

#include <USB.h>
#include <USBCDC.h>

#include <USBHIDKeyboard.h>

#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>

#include <Keys.h>
#include <HIDDescriptor.h>

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

#define DEVICE_INFO_SERVICE_UUID "0000180a-0000-1000-8000-00805f9b34fb"
#define MODEL_NUMER_UUID "00002A24-0000-1000-8000-00805f9b34fb"
#define SERIAL_NUMBER_UUID "00002A25-0000-1000-8000-00805f9b34fb"
#define FW_REVISION_UUID "00002A26-0000-1000-8000-00805f9b34fb"
#define HW_REVISION_UUID "00002A27-0000-1000-8000-00805f9b34fb"
#define SW_REVISION_UUID "00002A28-0000-1000-8000-00805f9b34fb"
#define MANUFACTURER_UUID "00002A29-0000-1000-8000-00805f9b34fb"
#define PNP_UUID "00002A50-0000-1000-8000-00805f9b34fb"

#define BATTERY_SERVICE_UUID "0000180f-0000-1000-8000-00805f9b34fb"
#define BATTERY_LEVEL_UUID "00002A19-0000-1000-8000-00805f9b34fb"
#define BATTERY_LEVEL_DESCRIPTOR_UUID "00002904-0000-1000-8000-00805f9b34fb"

#define HID_SERVICE_UUID "00001812-0000-1000-8000-00805f9b34fb"
#define HID_BOOT_INPUT_UUID "00002a22-0000-1000-8000-00805f9b34fb"
#define HID_BOOT_OUTPUT_UUID "00002a32-0000-1000-8000-00805f9b34fb"
#define HID_PROTOCOL_MODE_UUID "00002a4e-0000-1000-8000-00805f9b34fb"
#define HID_INFORMATION_UUID "00002a4a-0000-1000-8000-00805f9b34fb"
#define HID_REPORT_MAP_UUID "00002a4b-0000-1000-8000-00805f9b34fb"
#define HID_CONTROL_POINT_UUID "00002a4c-0000-1000-8000-00805f9b34fb"

#define HID_REPORT_UUID (uint16_t)0x2A4D
#define HID_REPORT_DESCRIPTOR_UUID "00002908-0000-1000-8000-00805f9b34fb"

#define MODEL_NUMER "1234567"
#define SERIAL_NUMBER "1"
#define FW_REVISION "0.0.1"
#define HW_REVISION "0.0.1"
#define SW_REVISION "0.0.1"
#define MANUFACTURER "Parozzz"

USBCDC SerialUSB;
USBHIDKeyboard Keyboard;

static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{

  if (event_base == ARDUINO_USB_EVENTS)
  {
    arduino_usb_event_data_t *data = (arduino_usb_event_data_t *)event_data;
    switch (event_id)
    {
    case ARDUINO_USB_STARTED_EVENT:
      // HWSerial.println("USB PLUGGED");
      break;
    case ARDUINO_USB_STOPPED_EVENT:
      // HWSerial.println("USB UNPLUGGED");
      break;
    case ARDUINO_USB_SUSPEND_EVENT:
      // HWSerial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
      break;
    case ARDUINO_USB_RESUME_EVENT:
      // HWSerial.println("USB RESUMED");
      break;
    default:
      break;
    }
  }
  else if (event_base == ARDUINO_USB_CDC_EVENTS)
  {
    arduino_usb_cdc_event_data_t *data = (arduino_usb_cdc_event_data_t *)event_data;
    switch (event_id)
    {
    case ARDUINO_USB_CDC_CONNECTED_EVENT:
      // HWSerial.println("CDC CONNECTED");
      break;
    case ARDUINO_USB_CDC_DISCONNECTED_EVENT:
      // HWSerial.println("CDC DISCONNECTED");
      break;
    case ARDUINO_USB_CDC_LINE_STATE_EVENT:
      // HWSerial.printf("CDC LINE STATE: dtr: %u, rts: %u\n", data->line_state.dtr, data->line_state.rts);
      break;
    case ARDUINO_USB_CDC_LINE_CODING_EVENT:
      // HWSerial.printf("CDC LINE CODING: bit_rate: %u, data_bits: %u, stop_bits: %u, parity: %u\n", data->line_coding.bit_rate, data->line_coding.data_bits, data->line_coding.stop_bits, data->line_coding.parity);
      break;
    case ARDUINO_USB_CDC_RX_EVENT:
      // HWSerial.printf("CDC RX [%u]:", data->rx.len);
      //{
      //     uint8_t buf[data->rx.len];
      //     size_t len = USBSerial.read(buf, data->rx.len);
      //     HWSerial.write(buf, len);
      // }
      // HWSerial.println();
      break;
    case ARDUINO_USB_CDC_RX_OVERFLOW_EVENT:
      // HWSerial.printf("CDC RX Overflow of %d bytes", data->rx_overflow.dropped_bytes);
      break;
    default:
      break;
    }
  }
}

#define MODIFIERS_LEFT_CTRL 0x01
#define MODIFIERS_LEFT_SHIFT 0x02
#define MODIFIERS_LEFT_ALT 0x04
#define MODIFIERS_LEFT_GUI 0x08
#define MODIFIERS_RIGHT_CTRL 0x10
#define MODIFIERS_RIGHT_SHIFT 0x20
#define MODIFIERS_RIGHT_ALT 0x40
#define MODIFIERS_RIGHT_GUI 0x80

typedef struct keyboard_report_t
{
  uint8_t modifiersKeys = 0;
  uint8_t reserved = 0;
  uint8_t key_codes[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
} keyboard_report_t;
keyboard_report_t kbd_report;

bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    SerialUSB.println("BLE - OnConnect");
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer)
  {
    SerialUSB.println("BLE - OnDisconnect");
    deviceConnected = false;
  }

  uint32_t onPassKeyRequest()
  {
    SerialUSB.println("Server PassKeyRequest");
    return 123456;
  }

  bool onConfirmPIN(uint32_t pass_key)
  {
    SerialUSB.print("The passkey YES/NO number: ");
    SerialUSB.println(pass_key);
    return true;
  }

  void onAuthenticationComplete(ble_gap_conn_desc desc)
  {
    SerialUSB.println("Starting BLE work!");
  }
};

class OutputCallbacks : public NimBLECharacteristicCallbacks
{
  void onWrite(NimBLECharacteristic *pCharacteristic)
  {
    SerialUSB.print("Output written. Value = ");
    SerialUSB.println(pCharacteristic->getValue().c_str());
  }
};

NimBLECharacteristic *inputReport;
NimBLECharacteristic *outputReport;
NimBLECharacteristic *inputMediaKeysReport;

NimBLEAdvertising *pAdvertising;

#define TEST_SERVICE_UUID "6a15b3c8-10e1-11ed-861d-0242ac120002"
#define TEST_UUID "6a15b3cc-10e1-11ed-861d-0242ac120002"

//generatePNP(0x02, 0xe502, 0xa111, 0x0210)
#define SIG     0x02     //The vendor ID source number.
#define VID     0xe502   //The vendor ID number.
#define PID     0xA111   //The product ID number.
#define VERSION 0x0210   //The produce version number.
const uint8_t pnp[] = {SIG,
                 (uint8_t)(VID >> 8),
                 (uint8_t)VID,
                 (uint8_t)(PID >> 8),
                 (uint8_t)PID,
                 (uint8_t)(VERSION >> 8),
                 (uint8_t)VERSION};

#define COUNTRY 0     //The country code for the device.
#define INFO_FLAGS 1  //The HID Class Specification release number to use.
  const uint8_t info[] = {0x11,
                    0x1,
                    COUNTRY,
                    INFO_FLAGS};

#define PROTOCOL_MODE_BOOT 0x0
#define PROTOCOL_MODE_REPORT 0x1
const uint8_t protocolMode[] = {PROTOCOL_MODE_REPORT};

uint8_t battery = 75;

void setup()
{
  USB.usbClass(0);
  USB.usbSubClass(0);
  USB.usbProtocol(0);
  USB.manufacturerName("PAROZZZ");
  USB.productName("PAROZZZ_USB");

  USB.onEvent(usbEventCallback);
  SerialUSB.onEvent(usbEventCallback);

  SerialUSB.begin(115200);
  Keyboard.begin();

  USB.begin();

  while (!SerialUSB)
  {
  }
  SerialUSB.println("BEGIN");

  NimBLEDevice::init("ESP32-S3 HID");
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);

  BLEServer *pServer = BLEDevice::createServer();
  pServer->advertiseOnDisconnect(true);
  pServer->setCallbacks(new MyServerCallbacks());

  // ============================== INFO ==============================
  SerialUSB.println("DEVICE INFO SERVICE");

  BLEService *pDeviceInfoService = pServer->createService(NimBLEUUID((uint16_t) 0x180a));
  //pDeviceInfoService->createCharacteristic(MODEL_NUMER_UUID, NIMBLE_PROPERTY::READ)->setValue(MODEL_NUMER);
  //pDeviceInfoService->createCharacteristic(SERIAL_NUMBER_UUID, NIMBLE_PROPERTY::READ)->setValue(SERIAL_NUMBER);
  //pDeviceInfoService->createCharacteristic(FW_REVISION_UUID, NIMBLE_PROPERTY::READ)->setValue(FW_REVISION);
  //pDeviceInfoService->createCharacteristic(HW_REVISION_UUID, NIMBLE_PROPERTY::READ)->setValue(HW_REVISION);
  //pDeviceInfoService->createCharacteristic(SW_REVISION_UUID, NIMBLE_PROPERTY::READ)->setValue(SW_REVISION);
  pDeviceInfoService->createCharacteristic(MANUFACTURER_UUID, NIMBLE_PROPERTY::READ)->setValue(MANUFACTURER);
  pDeviceInfoService->createCharacteristic(PNP_UUID, NIMBLE_PROPERTY::READ)->setValue(pnp, sizeof(pnp));
  // ============================== INFO ==============================
  // ============================== HID ==============================
  SerialUSB.println("HID SERVICE");

  BLEService *pHIDService = pServer->createService(NimBLEUUID((uint16_t) 0x1812));
  //pHIDService->createCharacteristic(HID_BOOT_INPUT_UUID, NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ, 8);
  //pHIDService->createCharacteristic(HID_BOOT_OUTPUT_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR, 1);
  
  pHIDService->createCharacteristic(HID_INFORMATION_UUID, NIMBLE_PROPERTY::READ, 4)->setValue(info, sizeof(info));
  pHIDService->createCharacteristic(HID_REPORT_MAP_UUID, NIMBLE_PROPERTY::READ)->setValue(HIDReportDescriptor, sizeof(HIDReportDescriptor));
  pHIDService->createCharacteristic(HID_CONTROL_POINT_UUID, NIMBLE_PROPERTY::WRITE_NR, 1)->setValue(1);
  pHIDService->createCharacteristic(HID_PROTOCOL_MODE_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE_NR)->setValue(protocolMode, sizeof(protocolMode));

  
  inputReport = pHIDService->createCharacteristic((uint16_t)0x2A4D, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ_ENC);
  inputReport->createDescriptor((uint16_t)0x2908, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC)->setValue({REPORT_KEYBOARD_ID, 0x01});

  outputReport = pHIDService->createCharacteristic((uint16_t)0x2A4D, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_ENC);
  outputReport->createDescriptor((uint16_t)0x2908, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::WRITE_ENC)->setValue({REPORT_KEYBOARD_ID, 0x02});
  outputReport->setCallbacks(new OutputCallbacks());

  inputMediaKeysReport = pHIDService->createCharacteristic((uint16_t)0x2A4D, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ_ENC);
  inputMediaKeysReport->createDescriptor((uint16_t)0x2908, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC)->setValue({REPORT_MEDIA_KEYS_ID, 0x01});
  // ============================== HID ==============================

  // ============================== BATTERY ==============================
  SerialUSB.println("BATTERY SERVICE");

  BLEService *pBatteryService = pServer->createService(NimBLEUUID((uint16_t) 0x180f));

  NimBLECharacteristic* pBatteryLevel = pBatteryService->createCharacteristic((uint16_t) 0x2A19, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  pBatteryLevel->setValue(&battery, 1);
  
  NimBLE2904* pBatteryLevelDescriptor = (NimBLE2904*) pBatteryLevel->createDescriptor((uint16_t) 0x2904);
  pBatteryLevelDescriptor->setFormat(NimBLE2904::FORMAT_UINT8);
	pBatteryLevelDescriptor->setNamespace(1);
	pBatteryLevelDescriptor->setUnit(0x27ad);
  // ============================== BATTERY ==============================

  NimBLESecurity *pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

  pDeviceInfoService->start();
  pHIDService->start();
  pBatteryService->start();

  SerialUSB.println("ADVERTISING");

  pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->setName("ESP32-S3 HID");
  pAdvertising->addServiceUUID(pHIDService->getUUID());
  pAdvertising->setAppearance(APPEARANCE_KEYBOARD);
  pAdvertising->start();
}

void SendKeyReport(uint8_t key)
{
  kbd_report.modifiersKeys = 0;

  kbd_report.key_codes[0] = key;

  inputReport->setValue(kbd_report);
  inputReport->notify();
}

void ReleaseAll()
{
  kbd_report.modifiersKeys = 0;

  for (int x = 0; x < 6; x++)
  {
    kbd_report.key_codes[x] = 0;
  }

  inputReport->setValue(kbd_report);
  inputReport->notify();
}

bool oldConnected = false;
void loop()
{
  if (deviceConnected)
  {
    SerialUSB.println("Sending Z!");

    SendKeyReport(KEY_Z);
    delay(10);
    ReleaseAll();

    delay(1000);
  }
  else
  {
    SerialUSB.println("Lap!");
    delay(500);
  }
}

/*
#include <BleKeyboard.h>

static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{

  if (event_base == ARDUINO_USB_EVENTS)
  {
    arduino_usb_event_data_t *data = (arduino_usb_event_data_t *)event_data;
    switch (event_id)
    {
    case ARDUINO_USB_STARTED_EVENT:
      // HWSerial.println("USB PLUGGED");
      break;
    case ARDUINO_USB_STOPPED_EVENT:
      // HWSerial.println("USB UNPLUGGED");
      break;
    case ARDUINO_USB_SUSPEND_EVENT:
      // HWSerial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
      break;
    case ARDUINO_USB_RESUME_EVENT:
      // HWSerial.println("USB RESUMED");
      break;

    default:
      break;
    }
  }
  else if (event_base == ARDUINO_USB_CDC_EVENTS)
  {
    arduino_usb_cdc_event_data_t *data = (arduino_usb_cdc_event_data_t *)event_data;
    switch (event_id)
    {
    case ARDUINO_USB_CDC_CONNECTED_EVENT:
      // HWSerial.println("CDC CONNECTED");
      break;
    case ARDUINO_USB_CDC_DISCONNECTED_EVENT:
      // HWSerial.println("CDC DISCONNECTED");
      break;
    case ARDUINO_USB_CDC_LINE_STATE_EVENT:
      // HWSerial.printf("CDC LINE STATE: dtr: %u, rts: %u\n", data->line_state.dtr, data->line_state.rts);
      break;
    case ARDUINO_USB_CDC_LINE_CODING_EVENT:
      // HWSerial.printf("CDC LINE CODING: bit_rate: %u, data_bits: %u, stop_bits: %u, parity: %u\n", data->line_coding.bit_rate, data->line_coding.data_bits, data->line_coding.stop_bits, data->line_coding.parity);
      break;
    case ARDUINO_USB_CDC_RX_EVENT:
      // HWSerial.printf("CDC RX [%u]:", data->rx.len);
      //{
      //     uint8_t buf[data->rx.len];
      //     size_t len = USBSerial.read(buf, data->rx.len);
      //     HWSerial.write(buf, len);
      // }
      // HWSerial.println();
      break;
    case ARDUINO_USB_CDC_RX_OVERFLOW_EVENT:
      // HWSerial.printf("CDC RX Overflow of %d bytes", data->rx_overflow.dropped_bytes);
      break;

    default:
      break;
    }
  }
}

/*
#include <BleKeyboard.h>

USBCDC SerialUSB;
BleKeyboard bleKeyboard;

void setup()
{
  USB.usbClass(0);
  USB.usbSubClass(0);
  USB.usbProtocol(0);
  USB.manufacturerName("PAROZZZ");
  USB.productName("PAROZZZ_USB");

  SerialUSB.begin(115200);

  USB.begin();

  while (!SerialUSB)
  {
  }

  SerialUSB.println("BEGIN");

  SerialUSB.println("Starting BLE work!");
  bleKeyboard.begin();

  bleKeyboard.setBatteryLevel(56);
}

void loop()
{
  if (bleKeyboard.isConnected())
  {
    SerialUSB.println("Sending 'Hello world'...");
    bleKeyboard.print("Hello world");

    delay(1000);

    //SerialUSB.println("Sending Enter key...");
    //bleKeyboard.write(KEY_RETURN);
    //delay(1000);

    //SerialUSB.println("Sending Play/Pause media key...");
    //bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);

    //delay(1000);

    //SerialUSB.println("Sending Ctrl+Alt+Delete...");
    //bleKeyboard.press(KEY_LEFT_CTRL);
    //bleKeyboard.press(KEY_LEFT_ALT);
    //bleKeyboard.press(KEY_DELETE);
    delay(100);
    bleKeyboard.releaseAll();
  }

  SerialUSB.println("Waiting 5 seconds...");
  delay(5000);
}
*/