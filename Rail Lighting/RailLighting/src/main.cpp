//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2018 Dave Plummer.  All Rights Reserved.
//
// File:        rail lighting FOR TRAIN 
//
// Description:
//
//   Draws sample effects on a an addressable strip using FastLED
//
// History:     Sep-15-2020     davepl      Created
//              Oct-05-2020     davepl      Revised for Episode 07
//              Oct-11-2020     davepl      Revised for Episode 08
//              Oct-16-2020     davepl      Revised for Episode 09
//              Oct-23-2020     davepl      Revised for Episode 10
//---------------------------------------------------------------------------
/* Mole Driver Motor control.  Functions: audio using spiffs and software i2S, accelerometer
   for detecting hits, stepper motor control, light module driver
   the d1 mini pinout is as follows: NOTE ARDUINO FOLLOWS GPIO NUMBERS
   PIN ID  GPIO_NUMBER   normal FUNCTION                               new function
   RX0        3          PWM AUDIO OUT I2S NORMALLY RX                 serial Rx usb
   D8        15          I2S CLK PULLED GND BOOT FAILS IF PULLED HIGH  Audio comms i/o
   D4        2           I2S WS  PULLED UP HIGH AT BOOT ONBOARD LED    led driver out + stepper steps
   D6        12                                                        opto endstop top
   D0        16          E /WAKE?  MUST BE HIGH AT BOOT TO WAKE        stepper enable out
   D5        14                                                        OPTO-ENDSTOP BOTTOM
   D7        13          IN OR OUT                                     stepper direction
   TX0        1          tz BOOT FAILS IF PULLED LOW                   serial Tx
   D3        0           FLASH/ PULLED UP BOOT FAILS IF PULLED LOW,    Control Comms i/o
   D1        5           I2C SCL -->ACCELL SCL CAN BE MOVED            scl  for accell if used
   D2        4           I2C SDA -->ACCELL SDA CAN BE MOVED            sda  for accell if used
   A0                    ADC0
   */

#include <Arduino.h>            // Arduino Framework
#define FASTLED_INTERNAL        // Suppress build banner
#include <FastLED.h>


#define FAN_SIZE       110 //16       // How many pixels per fan 3m @60/m =180
#define NUM_FANS       1//3        // Number of fans in the strans
#define LED_FAN_OFFSET 0        // How far from 12 o'clock first pixel is
#define NUM_LEDS       (FAN_SIZE*NUM_FANS)
#define LED_PIN        2        //D4


CRGB g_LEDs[NUM_LEDS] = {0};    // Frame buffer for FastLED

int g_Brightness = 255;         // 0-255 LED brightness scale
int g_PowerLimit = 5000;         // 900mW Power Limit
int speed=0;
//#include "ledgfx.h"
#include "comet.h"
//#include "marquee.h"
//#include "twinkle.h"
//#include "fire.h"
//#include "bounce.h"



void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.begin(115200);
  while (!Serial) { }
  Serial.println("ESP8266 rail lighting v1 Startup");


  FastLED.addLeds<WS2812B, LED_PIN, GRB>(g_LEDs, NUM_LEDS);               // Add our LED strip to the FastLED library
  FastLED.setBrightness(g_Brightness);
  set_max_power_indicator_LED(LED_BUILTIN);                               // Light the builtin LED if we power throttle
  FastLED.setMaxPowerInMilliWatts(g_PowerLimit);                          // Set the power limit, above which brightness will be throttled
  speed =2;
  
}

void loop() 
{
   
  while (true)
  {
    
    FastLED.clear();
    //no_comets, speed_comets,cometSize,spacing)
    switch (speed)
    {
      case 0:
      DrawComet(1, 30, 4,25,50);
      DrawSparks(30, 30, 4,60);
      break;
      case 1/* slowestconstant-expression */:
      DrawComet(3, 25, 5,25,50);
      DrawSparks(30, 30, 3,90);
      break;
      case 2:
      DrawComet(6, 15, 6,18,80);
      DrawSparks(30, 30, 5,80);
      break;
      case 3:
      DrawComet(24, 8, 8,14,90);
      DrawSparks(30, 30, 8,75);
      break;
      case 4:
      DrawComet(48, 4, 7,12,95);
      DrawSparks(30, 30, 10, 50);
      break;
    default:
      break;
    }
    
  
  if (Serial.available() > 0) 
  {
    // read the incoming byte:
    speed = int(Serial.read()) -48;
    Serial.println(speed);
  }

    
    FastLED.show(g_Brightness);                          //  Show and delay
    delay(1);
    yield();
  }
  ESP.wdtFeed();
}
