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
// History:     Sep-28-2020     davepl      Created
//
//---------------------------------------------------------------------------

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

void DrawComet(uint8_t no_comets, uint8_t speed_comets,int cometSize,int spacing,int fadepercent)
{
    const uint8_t fadeAmt = 128;
    //const int cometSize = 1;
    const int deltaHue  = 4;

    static uint8_t hue = HUE_RED;
    static int iDirection = 1;
    static int iPos = 0;
    static int posn = 0;
    hue += deltaHue;
    //hue = deltaHue;

    iPos += iDirection;
    //Serial.print("count:"); Serial.println(FastLED.size());

    

    if (iPos == (FastLED.size() - cometSize))
    {   //iDirection *= -1;
        iPos = 0;
        iDirection = 1;
    }

    for(int c= 0; c < no_comets; c++)
    {
        
        for (int i = 0; i < cometSize; i++)
        {
            posn = (c*spacing);
            posn += i;
            posn +=iPos;

            
            if (posn < FastLED.size()) //break;//posn = FastLED.size() -1;
                FastLED.leds()[posn].setHue(hue);
        }
        
        // Randomly fade the LEDs
        
        for (int j = 0; j < FastLED.size(); j++)
        {
            if (random(100) > fadepercent)
                FastLED.leds()[j] = FastLED.leds()[j].fadeToBlackBy(fadeAmt);  
        }
        
              
    }
    delay(speed_comets);
    ESP.wdtFeed();
}

void DrawSparks(int firstposn, int spacing,int sparkSize, int fadepercent)
{
    const uint8_t fadeAmt = 128;
    //const int cometSize = 1;
    const int deltaHue  = 4;

    //static uint8_t hue = WHITE;
    static int iDirection = 1;
    static int iPos = 0;
    static int posn = 0;
    //hue += deltaHue;
    //hue = deltaHue;

    //iPos += iDirection;
    //Serial.print("count:"); Serial.println(FastLED.size());
    if (random(100) > fadepercent)
    {
    // 2 wheels only
        for(int c= 0; c < 2; c++)
        {
            
            for (int i = 0; i < sparkSize; i++)
            {
                posn = firstposn + (c*spacing) + random(-5,5);
                posn += i;
                        
                if (posn < FastLED.size() && (random(10) > 4)) 
                    FastLED.leds()[posn]=CRGB::White;//.setHue(hue);
                    if (random(10) > 5)
                    FastLED.leds()[posn] = FastLED.leds()[posn].fadeToBlackBy(fadeAmt); 
            }
            
            // Randomly fade the LEDs
            /*
            for (int j = 0; j < FastLED.size(); j++)
            {
                if (random(100) > fadepercent)
                    FastLED.leds()[j] = FastLED.leds()[j].fadeToBlackBy(fadeAmt);  
            }
            */
            
                
        }
    }
    //delay(speed_comets);
    //ESP.wdtFeed();
}