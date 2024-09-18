/*
Controle de volume sur Raspberry Pi Pico
Prérequis : Voir le fichier platformio.ini

Connexions :

+----------------+----------------+
| Rotary encoder  | Raspberry Pico |
+----------------+----------------+
| CLK            | GP20           |
+----------------+----------------+
| DT             | GP21           |
+----------------+----------------+
| SW             | GP22           |
+----------------+----------------+
| +              | 3V3            |
+----------------+----------------+
| GND            | GND            |
+----------------+----------------+

*/

// GPIO Pin Definitions
#define PIN_BUTTON2 9
#define LED_MODE 13
#define DT_PIN1 3
#define CLK_PIN1 4
#define SW_PIN1 2

// Includes
#include <Arduino.h>
#include <Adafruit_TinyUSB.h> // USB HID Library
#include "pico/stdlib.h"
#include <RotaryEncoder.h>
#include "OneButton.h"

// HID Report ID types
enum
{
  RID_KEYBOARD = 1,
  RID_MOUSE,
  RID_CONSUMER_CONTROL // Media, volume, etc.
};

// HID Report Descriptors
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD)),
  TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(RID_MOUSE)),
  TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(RID_CONSUMER_CONTROL))};

Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_NONE, 2, false);

// Rotary encoder and button setup
RotaryEncoder encoder(DT_PIN1, CLK_PIN1, RotaryEncoder::LatchMode::TWO03);
OneButton button1(SW_PIN1, false);
OneButton button2(PIN_BUTTON2, false);

// Modes for the encoder
enum Mode
{
  VOLUME_CONTROL = 1,
  KEYBOARD_CONTROL = 2
};
Mode currentMode = VOLUME_CONTROL;

// Function Declarations
void playPause();
void mediaNext();
void changeMode();
void moveRight();
void moveLeft();
void sendHIDCommand(uint16_t reportID, uint16_t command);

// Setup
void setup()
{
  Serial.end(); // Disable Serial for USB HID
  set_sys_clock_khz(48000, true);
  pinMode(LED_MODE, OUTPUT);
  pinMode(SW_PIN1, INPUT_PULLDOWN);
  pinMode(PIN_BUTTON2, INPUT_PULLDOWN);
  usb_hid.begin();

  // Attach button functions
  button2.attachClick(changeMode);
  button1.attachClick(playPause);
  button1.attachDoubleClick(mediaNext);

  digitalWrite(LED_MODE, LOW); // Start with LED off
}

// Main Loop
void loop()
{
  static int lastPosition = 0;
  encoder.tick();    // Update encoder state
  button1.tick();    // Update button state
  button2.tick();    // Update button2 state

  int newPosition = encoder.getPosition();
  if (lastPosition != newPosition)
  {
    int direction = (int)(encoder.getDirection());
    lastPosition = newPosition;

    if (currentMode == VOLUME_CONTROL)
    {
      if (direction == 1)
        sendHIDCommand(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_VOLUME_INCREMENT);
      else
        sendHIDCommand(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_VOLUME_DECREMENT);
    }
    else if (currentMode == KEYBOARD_CONTROL)
    {
      if (direction == 1)
        moveRight();
      else
        moveLeft();
    }
  }
}

// Send HID command with report ID and command
void sendHIDCommand(uint16_t reportID, uint16_t command)
{
  usb_hid.sendReport16(reportID, command);
  delay(5); // Debounce
  usb_hid.sendReport16(reportID, 0); // Clear command
}

// Play/pause media control
void playPause()
{
  sendHIDCommand(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_PLAY_PAUSE);
  Serial.println("playPause");
}

// Next media track control
void mediaNext()
{
  sendHIDCommand(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_SCAN_NEXT);
  Serial.println("mediaNext");
}

// Move right (arrow key)
void moveRight()
{
  Serial.println("Right Arrow");
  uint8_t keycode[6] = {HID_KEY_ARROW_RIGHT};
  usb_hid.keyboardReport(RID_KEYBOARD, 0, keycode);
  delay(50);
  usb_hid.keyboardReport(RID_KEYBOARD, 0, 0);
}

// Move left (arrow key)
void moveLeft()
{
  Serial.println("Left Arrow");
  uint8_t keycode[6] = {HID_KEY_ARROW_LEFT};
  usb_hid.keyboardReport(RID_KEYBOARD, 0, keycode);
  delay(50);
  usb_hid.keyboardReport(RID_KEYBOARD, 0, 0);
}

// Change mode between volume and keyboard control
void changeMode()
{
  if (currentMode == VOLUME_CONTROL)
  {
    currentMode = KEYBOARD_CONTROL;
    digitalWrite(LED_MODE, HIGH);
  }
  else
  {
    currentMode = VOLUME_CONTROL;
    digitalWrite(LED_MODE, LOW);
  }

  Serial.print("Change Mode: ");
  Serial.println(currentMode);
}
