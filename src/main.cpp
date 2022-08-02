#include <Arduino.h>

#include <USB.h>
#include <USBCDC.h>
#include <USBHIDKeyboard.h>

#include <Keys.h>
#include <BLEKeyboard.h>

USBCDC usbSerial;
USBHIDKeyboard usbKeyboard;
BleKeyboard bleKeyboard("ESP32-S3 BLE_HID");

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

void setup()
{
  USB.usbClass(0);
  USB.usbSubClass(0);
  USB.usbProtocol(0);
  USB.manufacturerName("PAROZZZ");
  USB.productName("PAROZZZ_USB");

  USB.onEvent(usbEventCallback);
  usbSerial.onEvent(usbEventCallback);

  usbSerial.begin(115200);
  usbKeyboard.begin();
  USB.begin();

  while (!usbSerial)
  {
    delay(100);
  }
  usbSerial.println("BEGIN");

  bleKeyboard.setDebugPrint(&usbSerial);

  usbSerial.println("BLE _ InitDevice");
  bleKeyboard.initDEVICE();

  usbSerial.println("BLE _ InitInfo");
  bleKeyboard.initINFO("PAROZZZ");

  usbSerial.println("BLE _ InitHID");
  bleKeyboard.initHID();

  usbSerial.println("BLE _ InitBattery");
  bleKeyboard.initBATTERY(100);

  usbSerial.println("BLE _ InitAdvertising");
  bleKeyboard.initADVERTISING(0xB00B);

  usbSerial.println("BLE _ StartAdvertising");
  bleKeyboard.startAdvertising();
}

void loop()
{
  bleKeyboard.loop();

  if (bleKeyboard.ready)
  {
    usbSerial.println("Send Z");

    bleKeyboard.keyClick(KEY_A);
    delay(2500);
  }
  else
  {
    usbSerial.println("Lap!");
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