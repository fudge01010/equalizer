/*
  Copyright (c) 2014-2016 NicoHood
  See the readme for credit to other people.

  MSGEQ7 FastLED example
  Output via Led strip and FastLED library

  Reads MSGEQ7 IC with 7 different frequencies from range 0-255
  63Hz, 160Hz, 400Hz, 1kHz, 2.5kHz, 6.25KHz, 16kHz
*/

// FastLED
#include "FastLED.h"

#define LED_PINS    11 // DATA_PIN, or DATA_PIN, CLOCK_PIN
#define COLOR_ORDER GRB
#define CHIPSET     WS2811 // WS2811 LPD8806
#define NUM_LEDS    314

#define BRIGHTNESS  255  // reduce power consumption
#define LED_DITHER  1  // try 0 to disable flickering
#define CORRECTION  TypicalSMD5050

CRGB leds[NUM_LEDS]; // Define the array of leds

// MSGEQ7
#include "MSGEQ7.h"
#define pinAnalogLeft A0
#define pinAnalogRight A1
#define pinReset 12
#define pinStrobe 13
#define MSGEQ7_INTERVAL ReadsPerSecond(100)
#define MSGEQ7_SMOOTH true

CMSGEQ7<MSGEQ7_SMOOTH, pinReset, pinStrobe, pinAnalogLeft, pinAnalogRight> MSGEQ7;

//define avicii's hit 2011 tune 
int LEVELS[13];

//define some measurements for the size calculations:
#define BAND_HEIGHT 24
#define BANDS 13

int SAT = 200;
int HUE = 0;

void setup() {
  // FastLED setup
  FastLED.addLeds<CHIPSET, LED_PINS, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(CORRECTION);
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.setDither(LED_DITHER);
  FastLED.show(); // needed to reset leds to zero
//  Serial.begin(115200);

  //set power limit
  FastLED.setMaxPowerInVoltsAndMilliamps(5,2000); 

  // This will set the IC ready for reading
  MSGEQ7.begin();
}
void loop() {
  //roll the hue wheel
  EVERY_N_MILLISECONDS( 20 ) { HUE++; } // slowly cycle the "base color" through the rainbow

  // Analyze without delay
  bool newReading = MSGEQ7.read(MSGEQ7_INTERVAL);

  if (newReading)
  {
    //clear LED's, ready for new data
    FastLED.clear();
    
    //interpolate levels between channels
    interpolateChannels();
    
    for (int band=0; band<BANDS; band++)
    {
      //denoise the band
      LEVELS[band] = mapNoise(LEVELS[band]);
      //multiply by height to stretch it out
      int j = BAND_HEIGHT * LEVELS[band];
       
      int m = 0;
      while (j > 255)
      {
        leds[band*BAND_HEIGHT+m].setHSV(HUE+(15*band),SAT,255);
        j = j-255;
        m++;
      }
      leds[band*BAND_HEIGHT+m].setHSV(HUE+(15*band),SAT,j);
    }
    FastLED.show();
  }

  
//  // display values of left channel on serial monitor 
//  for (band = 0; band < 7; band++)
//  {
////   col = map(left[band], 0, 1023, 0, 255);
//     for (int x = 0; x < 12; x++)
//     {
//       leds[band * (12 * (x + 1))].setRGB(0, col, 0);
//     }
//  //   leds[band*12].setRGB( 0, col , 0);
//   }

//  Old "strobe", with single flash, de-noised bass channels
//  // Led strip output
//  if (newReading) {
//    // visualize the average bass of both channels
//    uint8_t val = MSGEQ7.get(MSGEQ7_BASS);
//
//    // Reduce noise
//    val = mapNoise(val);
//
//    // Visualize leds to the beat
//    CRGB color = CRGB::White;
//    color.nscale8_video(val);
//    fill_solid(leds, NUM_LEDS, color);
//
//    // Update Leds
//    FastLED.show();
//    Serial.println(val);
//  }
}

void interpolateChannels()
{
  LEVELS[0] = MSGEQ7.get(0);
  LEVELS[2] = MSGEQ7.get(1);
  LEVELS[4] = MSGEQ7.get(2);
  LEVELS[6] = MSGEQ7.get(3);
  LEVELS[8] = MSGEQ7.get(4);
  LEVELS[10]= MSGEQ7.get(5);
  LEVELS[12]= MSGEQ7.get(6);

  //fill the missing gaps using fastled fast maths
  LEVELS[1] = avg8(LEVELS[0], LEVELS[2]);
  LEVELS[3] = avg8(LEVELS[2], LEVELS[4]);
  LEVELS[5] = avg8(LEVELS[4], LEVELS[6]);
  LEVELS[7] = avg8(LEVELS[6], LEVELS[8]);
  LEVELS[9] = avg8(LEVELS[8], LEVELS[10]);
  LEVELS[11]= avg8(LEVELS[10], LEVELS[12]);
}


