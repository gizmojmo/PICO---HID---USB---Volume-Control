#Pico Multimedia Controller Project

This program provides an HID interface that, with the help of two buttons, can control:

Volume

Play and pause of a media

The rotary button has 2 functions:

Volume control
Control of media position (this state is indicated by an LED): SW_PIN1
Switching between modes is done using the button named: PIN_BUTTON2

Here, I chose to use 2 independent buttons. I do not use the built-in button on the rotary encoder.

Below is the pinout:

| Rotary Encoder | Raspberry Pico |
|----------------|----------------|
| CLK            | GP20           |
| DT             | GP21           |
| SW             | not used       |
| +              | 3V3            |
| GND            | GND            |


Buttons:
| NAME        | GPIO |
|-------------|------|
| SW_PIN1     | 2    |
| PIN_BUTTON2 | 9    |

Libraries used:

RotaryEncoder 1.5.3
OneButton 2.0.3

For those using PlatFormIO, the platform.ini file is provided.

WARNING: If you need to reprogram the Raspberry Pi Pico, remember to use the BOOTSEL button. In the program, I have intentionally disabled Serial port management (using Serial.end in void setup) to prevent any accidental reprogramming, as the interface is meant to remain connected to your computer.