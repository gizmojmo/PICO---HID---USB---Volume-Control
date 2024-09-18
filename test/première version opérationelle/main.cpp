/*
Contole de volume sur raspberry Pico
Pré-requis :
Voir le fichier plateformio.ini

connexions :

+----------------+----------------+
| rotary encoder | Raspberry Pico |
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

//Additional button

#define PIN_BUTTON2 9


//LED MODE

#define LED_MODE 13

//------------------------------------------
#include <Arduino.h>
#include <Adafruit_TinyUSB.h> //Bibliotheque USB 
#include "pico/stdlib.h"



// ID du type de HID Report
enum
{
  RID_KEYBOARD = 1,
  RID_MOUSE,
  RID_CONSUMER_CONTROL, // Media, volume etc ..
};

// Descripteurs HID report utiliés par le modèle TinyUSB
uint8_t const desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(RID_KEYBOARD) ),
  TUD_HID_REPORT_DESC_MOUSE   ( HID_REPORT_ID(RID_MOUSE) ),
  TUD_HID_REPORT_DESC_CONSUMER( HID_REPORT_ID(RID_CONSUMER_CONTROL) )
};


Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_NONE, 2, false);

//-------------------Encodeur Rotatif-------------------------

#include <RotaryEncoder.h>

// Définition des GPIO pour l'encodeur rotatif
#define DT_PIN1 3
#define CLK_PIN1 4

int modeencoder=1;

RotaryEncoder encoder(DT_PIN1, CLK_PIN1, RotaryEncoder::LatchMode::TWO03);

//---------------------Bouton de l'encodeur rotatif-----------------------------------

#include "OneButton.h"

// Definition du GPIO pour le bouton de l'encodeur
#define SW_PIN1 2

OneButton button1(SW_PIN1, false);
OneButton button2(PIN_BUTTON2, false);

// déclaration de fonctions
void playpause();
void medianext();
void changemode();
void fdroite();
void fgauche();

void setup()
{
  
  Serial.end();
  set_sys_clock_khz(48000,true);
  pinMode(LED_MODE,OUTPUT);
  pinMode(SW_PIN1, INPUT_PULLDOWN);
  pinMode(PIN_BUTTON2, INPUT_PULLDOWN);
  usb_hid.begin();
  button2.attachClick(changemode);
  button1.attachClick(playpause);
  button1.attachDoubleClick(medianext);
  
} 


void loop()
{
  static int pos = 0;
  encoder.tick(); //lecture de l'encodeur
  button1.tick(); //lecture du bouton1
  button2.tick(); //lecture du bouton2

  int newPos = encoder.getPosition();
  if (pos != newPos) {
    int direction = (int)(encoder.getDirection());
    
    pos = newPos;
    if (modeencoder==1){
      
      if (direction == 1) {
          
          usb_hid.sendReport16(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_VOLUME_INCREMENT);
          delay(5);
          usb_hid.sendReport16(RID_CONSUMER_CONTROL, 0);
      }
      else {
        
        usb_hid.sendReport16(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_VOLUME_DECREMENT);
        delay(5);
        usb_hid.sendReport16(RID_CONSUMER_CONTROL, 0);
      }
    }
    
    if (modeencoder==2){
      digitalWrite(LED_MODE,HIGH);
      if (direction == 1) {
          
          fdroite();
      }
      else {
        
        fgauche();
      }
    

    }



  }
} 

void playpause() {
  usb_hid.sendReport16(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_PLAY_PAUSE);
  delay(10);
  usb_hid.sendReport16(RID_CONSUMER_CONTROL, 0);
  // Pour débogage
  Serial.println("playpause");
  //
  
}

void medianext() {
  usb_hid.sendReport16(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_SCAN_NEXT);
  delay(10);
  usb_hid.sendReport16(RID_CONSUMER_CONTROL, 0);
  delay(50);
  
}

void fdroite() {
  Serial.println("Droite");
  uint8_t keycode[6] = { 0 };
  keycode[0] = HID_KEY_ARROW_RIGHT;
  usb_hid.keyboardReport(RID_KEYBOARD, 0, keycode);
  
  delay(50);
  usb_hid.keyboardReport(RID_KEYBOARD, 0, 0);
  
}

void fgauche() {
  Serial.println("Gauche");
  uint8_t keycode[6] = { 0 };
  keycode[0] = HID_KEY_ARROW_LEFT;
  usb_hid.keyboardReport(RID_KEYBOARD, 0, keycode);
  
  delay(50);
  usb_hid.keyboardReport(RID_KEYBOARD, 0, 0);
  
}

void changemode() {

  if (modeencoder == 1) {
    modeencoder = 2;
    digitalWrite(LED_MODE,HIGH);
  }
  else 
  if (modeencoder ==2){
    modeencoder = 1;
    digitalWrite(LED_MODE,LOW);
  }

  Serial.print("Change Mode : ");
  Serial.println(modeencoder);
}