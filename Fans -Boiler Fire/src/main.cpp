//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2018 Dave Plummer.  All Rights Reserved.
//
// File:        boiler fire FOR TRAIN LED including servo movement
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
//#include <U8g2lib.h>            // For text on the little on-chip OLED
#define FASTLED_INTERNAL        // Suppress build banner
#include <FastLED.h>


#define FAN_SIZE       16 //16       // How many pixels per fan
#define NUM_FANS       8 //3        // Number of fans in the strans
#define LED_FAN_OFFSET 0        // How far from 12 o'clock first pixel is
#define NUM_LEDS       (FAN_SIZE*NUM_FANS)
#define LED_PIN        2        //D4
#define MAX_ANGLE      180       //MAX SERVO ANGLE
#define ANGLE_INCREMENT 45

CRGB g_LEDs[NUM_LEDS] = {0};    // Frame buffer for FastLED

//U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R2, OLED_RESET, OLED_CLOCK, OLED_DATA);
//int g_lineHeight = 0;
int g_Brightness = 255;         // 0-255 LED brightness scale
int g_PowerLimit = 5000;         // 900mW Power Limit
int Fireangle =0;  // for servo
bool FireangleDirection = true;
#include "ledgfx.h"
#include "comet.h"
#include "marquee.h"
#include "twinkle.h"
#include "fire.h"
#include "ServoControl.h"
//left to right == vertical direction

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  Fireangle =0;
  FireangleDirection = true;
  Fireservo.attach(servoPin);
  Serial.begin(115200);
  while (!Serial) { }
  Serial.println("ESP8266 Boiler Fire v2 Startup");

  //g_OLED.begin();
  //g_OLED.clear();
  //g_OLED.setFont(u8g2_font_profont15_tf);
  //g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent();        // Descent is a negative number so we add it to the total

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(g_LEDs, NUM_LEDS);               // Add our LED strip to the FastLED library
  FastLED.setBrightness(g_Brightness);
  set_max_power_indicator_LED(LED_BUILTIN);                               // Light the builtin LED if we power throttle
  FastLED.setMaxPowerInMilliWatts(g_PowerLimit);                          // Set the power limit, above which brightness will be throttled
  
}

void loop() 
{
  bool bLED = 0;
  
  while (true)
  {
    FastLED.clear();
   
    /*
    // RGB Spinners
    float b = beat16(60) / 65535.0f * FAN_SIZE;
    DrawFanPixels(b, 1, CRGB::Red, Sequential, 0);
    DrawFanPixels(b, 1, CRGB::Green, Sequential, 1);
    DrawFanPixels(b, 1, CRGB::Blue, Sequential, 2);

    DrawFanPixels(b, 1, CRGB::Red, Sequential, 3);
    DrawFanPixels(b, 1, CRGB::Green, Sequential, 4);
    DrawFanPixels(b, 1, CRGB::Blue, Sequential, 5);

    DrawFanPixels(b, 1, CRGB::Red, Sequential, 6);
    DrawFanPixels(b, 1, CRGB::Green, Sequential, 7);
    DrawFanPixels(b, 1, CRGB::Blue, Sequential, 8);
    */
    /*
    // Left to Right Cyan Wipe
    float b = beatsin16(60) / 65535.0f * FAN_SIZE;
    for (int iFan = 0; iFan < NUM_FANS; iFan++)
        DrawFanPixels(0, b, CRGB::Cyan, LeftRight, iFan);
    */

    /*
    // Left to Right Cyan Wipe
    float b = beatsin16(60) / 65535.0f * FAN_SIZE;
    for (int iFan = 0; iFan < NUM_FANS; iFan++)
        DrawFanPixels(0, b, CRGB::Cyan, RightLeft, iFan);
    */

    /*
    // Bottom up Green Wipe
    float b = beatsin16(60) / 65535.0f * NUM_LEDS;
        DrawFanPixels(0, b, CRGB::Green, BottomUp);  //== left to right
    
    */
    /*
    // Bottom up Green Wipe
    float b = beatsin16(60) / 65535.0f * NUM_LEDS;
        DrawFanPixels(0, b, CRGB::Green, TopDown);
    */

    /*
    // Simple Color Cycle
    static byte hue = 0;
    for (int i = 0; i < NUM_LEDS; i++)
      DrawFanPixels(i, 1, CHSV(hue, 255, 255));
    hue += 4;
    */

    /*
    // Sequential Rainbows
    static byte basehue = 0;
    byte hue = basehue;
    for (int i = 0; i < NUM_LEDS; i++)
      DrawFanPixels(i, 1, CHSV(hue+=16, 255, 255),BottomUp);
    basehue += 4;
    */

    /*
    // Vertical Rainbow Wipe
    static byte basehue = 0;
    byte hue = basehue;
    for (int i = 0; i < NUM_LEDS; i++)
      DrawFanPixels(i, 1, CHSV(hue+=8, 255, 255), BottomUp);
    basehue += 4;
    */

    /*
    // Horizontal Rainbow Stripe
    static byte basehue = 0;
    byte hue = basehue;
    for (int i = 0; i < NUM_LEDS; i++)
      DrawFanPixels(i, 1, CHSV(hue+=16, 255, 255), LeftRight);
    basehue += 8;
    */

    /*
    // Rainbow Stripe Palette Effect
    static CRGBPalette256 pal(RainbowStripeColors_p);
    static byte baseColor = 0;
    byte hue = baseColor;
    for (int i = 0; i < NUM_LEDS; i++)
      DrawFanPixels(i, 1, ColorFromPalette(pal, hue += 4), BottomUp);
    baseColor += 1;
    */

    /*   
    // vu-Style Meter
    int b = beatsin16(30) * NUM_LEDS / 65535L;
    static const CRGBPalette256 vuPaletteGreen = vu_gpGreen;
    for (int i = 0; i < b; i++)
        DrawFanPixels(i, 1, ColorFromPalette(vuPaletteGreen, (int)(255 * i / NUM_LEDS)), BottomUp);
    */

   /*protected:
    int     Size;               // How many pixels the flame is total
    int     Cooling;            // Rate at which the pixels cool off
    int     Sparks;             // How many sparks will be attempted each frame
    int     SparkHeight;        // If created, max height for a spark
    int     Sparking;           // Probability of a spark each attempt
    bool    bReversed;          // If reversed we draw from 0 outwards
    bool    bMirrored;          // If mirrored we split and duplicate the drawing
    */
    // Sequential Fire Fans
    static FireEffect fire(NUM_LEDS, 15, 20, 8, NUM_LEDS/2, true, false);
    //FireEffect fire(NUM_LEDS, 10, NO_SPARKS_PER, 6, NUM_LEDS, true, false);
    fire.DrawFire(BottomUp);
   

   

    /*
    // Bottom Up Fire Effect with extra sparking on first fan only
    static FireEffect fire(NUM_LEDS, 20, 240, 16, FAN_SIZE, true, true);
    fire.DrawFire(BottomUp);
    */

    /*
    // LeftRight (Wide) Fire Effect with extra sparking on first fan only
    static FireEffect fire(NUM_LEDS, 20, 140, 3, FAN_SIZE, true, false);
    fire.DrawFire(LeftRight);
    */
   /*
    for (int i = 0; i < FAN_SIZE; i++)  // Copy end fan down onto others
    {
      g_LEDs[i] = g_LEDs[i + 2 * FAN_SIZE];             
      g_LEDs[i + FAN_SIZE] = g_LEDs[i + 2 * FAN_SIZE];
    }
    */
/*
    int b = beatsin16(30) * NUM_LEDS / 65535L;
    static const CRGBPalette256 seawhawksPalette = vu_gpSeahawks;
    for (int i = 0; i < NUM_LEDS; i++)
        DrawFanPixels(i, 1, ColorFromPalette(seawhawksPalette, beat8(64) + (int)(255 * i / NUM_LEDS)), BottomUp);
    
    */

    FastLED.show(g_Brightness);                          //  Show and delay

    EVERY_N_MILLISECONDS(800)
    {
      //g_OLED.clearBuffer();
      //g_OLED.setCursor(0, g_lineHeight);
      //g_OLED.printf("FPS  : %u", FastLED.getFPS());
      Serial.print("FPS  :"); Serial.println(FastLED.getFPS());
      //g_OLED.setCursor(0, g_lineHeight * 2);
      //g_OLED.printf("Power: %u mW", calculate_unscaled_power_mW(g_LEDs, 4));
      Serial.print("Power mW:");Serial.println(calculate_unscaled_power_mW(g_LEDs, NUM_LEDS));
      //g_OLED.setCursor(0, g_lineHeight * 3);
      //g_OLED.printf("Brite: %d", calculate_max_brightness_for_power_mW(g_Brightness, g_PowerLimit));
      Serial.print("Brite: ");Serial.println(calculate_max_brightness_for_power_mW(g_Brightness, g_PowerLimit));
      //g_OLED.sendBuffer();
      Fireangle=random(0,180);
      Fireservo.write(Fireangle);
      Serial.print("Posn: ");Serial.println(Fireangle);
      /*
      if (FireangleDirection)
      {
        Fireangle+= ANGLE_INCREMENT;
        if (Fireangle >= MAX_ANGLE) FireangleDirection = false;
      }
      else
      {
        Fireangle-= ANGLE_INCREMENT;
        if (Fireangle <= 0) FireangleDirection = true;
      
      }
      */

    }
    delay(33);
  }
}
