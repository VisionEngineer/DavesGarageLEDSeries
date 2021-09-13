//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2018 Dave Plummer.  All Rights Reserved.
//
// File:        LED Episode 06
//
// Description:
//
//   Draws sample effects on a an addressable strip using FastLED
//
// History:     Sep-15-2020     davepl      Created
//
//---------------------------------------------------------------------------

#include <Arduino.h>            // Arduino Framework
#include <U8g2lib.h>            // For text on the little on-chip OLED
#define FASTLED_INTERNAL        // Suppress build banner
#include <FastLED.h>

#define OLED_CLOCK  15          // Pins for the OLED display
#define OLED_DATA   4
#define OLED_RESET  16

#define NUM_LEDS    45          // FastLED definitions
#define LED_PIN     5

CRGB g_LEDs[NUM_LEDS] = {0};    // Frame buffer for FastLED

U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R2, OLED_RESET, OLED_CLOCK, OLED_DATA);
int g_lineHeight = 0;
int g_Brightness = 255;           // 0-255 LED brightness scale



// FramesPerSecond
//
// Tracks a weighted average to smooth out the values that it calcs as the simple reciprocal
// of the amount of time taken specified by the caller.  So 1/3 of a second is 3 fps, and it
// will take up to 10 frames or so to stabilize on that value.

double FramesPerSecond(double seconds)
{
  static double framesPerSecond; 
  framesPerSecond = (framesPerSecond * .9) + (1.0 / seconds * .1);
  return framesPerSecond;
}

void DrawPixels(double fPos, double count, CRGB color)
{
    double availFirstPixel = 1.0 - (fPos - (long)(fPos));
    double amtFirstPixel = min(availFirstPixel, count);
    count = min(count, FastLED.size()-fPos);
    if (fPos >= 0 && fPos < FastLED.size())
    {
        CRGB frontColor = color;
        frontColor.fadeToBlackBy(255 * (1.0 - amtFirstPixel));
        FastLED.leds()[(uint)fPos] += frontColor;
    }

    fPos += amtFirstPixel;
    count -= amtFirstPixel;

    while (count >= 1.0)
    {
        if (fPos >= 0 && fPos < FastLED.size())
        {
            FastLED.leds()[(uint)fPos] += color;
            count -= 1.0;
        }
        fPos += 1.0;
    }

    if (count > 0.0)
    {
        if (fPos >= 0 && fPos < FastLED.size())
        {
            CRGB backColor = color;
            backColor.fadeToBlackBy(255 * (1.0 - count));
            FastLED.leds()[(uint)fPos] += backColor;
        }
    }
}

void set_max_power_indicator_LED(uint8_t);

#include "twinkle.h"
#include "marquee.h"
#include "comet.h"
#include "bounce.h"

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);
  while (!Serial) { }
  Serial.println("ESP32 Startup");

  g_OLED.begin();
  g_OLED.clear();
  g_OLED.setFont(u8g2_font_profont15_tf);
  g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent();        // Descent is a negative number so we add it to the total

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(g_LEDs, NUM_LEDS);               // Add our LED strip to the FastLED library
  FastLED.setBrightness(g_Brightness);
  FastLED.setMaxPowerInMilliWatts(900);
  set_max_power_indicator_LED(LED_BUILTIN);
}

void loop() 
{
  double fps = 0;

  BouncingBallEffect balls(NUM_LEDS, 3, 0, false, 8.0);

  while (true)
  {
    double dStart = millis() / 1000.0;                 // Display a frame and calc how long it takes

    // Handle LEDs

    balls.Draw();

    // Handle OLED drawing

    uint32_t milliwatts = calculate_unscaled_power_mW(g_LEDs, NUM_LEDS);

    static unsigned long msLastUpdate = millis();
    if (millis() - msLastUpdate > 500)
    {
      g_OLED.clearBuffer();
      g_OLED.setCursor(0, g_lineHeight);
      g_OLED.printf("FPS: %.1lf", fps);
      g_OLED.setCursor(1, g_lineHeight * 2);
      g_OLED.printf("Power: %u mW", milliwatts);
      g_OLED.sendBuffer();
      msLastUpdate = millis();
    }

    FastLED.show(g_Brightness);

    double dEnd = millis() / 1000.0;
    fps = FramesPerSecond(dEnd - dStart);
  }
}
