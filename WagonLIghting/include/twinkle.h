//+--------------------------------------------------------------------------
//
// NightDriver - (c) 2020 Dave Plummer.  All Rights Reserved.
//
// File:        
//
// Description:
//
//   
//
// History:     Sep-15-2020     davepl      Created
//
//---------------------------------------------------------------------------

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#include "ledgfx.h"

static const CRGB TwinkleColors [] = 
{
    CRGB::Red,
    CRGB::Blue,
    CRGB::Purple,
    CRGB::Green,
    CRGB::Yellow,
    CRGB::White
};

void DrawTwinkle(int nosparkles)
{
    static int passCount = 0;
    if (passCount++ == FastLED.size())///4)
    {
        passCount = 0;
        FastLED.clear(false);
    }
    for (size_t i = 0; i < nosparkles; i++)
    {
        /* code */
        FastLED.leds()[random(FastLED.size())] = TwinkleColors[random(0, ARRAYSIZE(TwinkleColors))];
    }
    
    
    //delay(2);       
}