/*
  HEADER TO ADD LATER
*/

// FastLED
#include "FastLED.h"
#define LED_PINS    11 //using ATMEGA328p's fast SPI pin for maximum powaa
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
#define MSGEQ7_INTERVAL ReadsPerSecond(90)
#define MSGEQ7_SMOOTH true
CMSGEQ7<MSGEQ7_SMOOTH, pinReset, pinStrobe, pinAnalogLeft, pinAnalogRight> MSGEQ7;

// System
const char NUM_ANIMATIONS = 5;
#define BAND_HEIGHT 24
#define BANDS 13
//create avicii's hit 2011 tune 
uint8_t LEVELS[13];
bool newReading;

// global variable definitons:
int SAT = 255;
int HUE = 0;
int CURRENT_ANI = 1;

bool dropped;

void setup() {
  // FastLED setup
  FastLED.addLeds<CHIPSET, LED_PINS, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(CORRECTION);
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.setDither(LED_DITHER);
  FastLED.show(); // needed to reset leds to zero
  
  //set power limit
  FastLED.setMaxPowerInVoltsAndMilliamps(5,5000); 
  
  // This will set the IC ready for reading
  MSGEQ7.begin();

  Serial.begin(57600);
  Serial.println("setup complete.");
  CURRENT_ANI = 1;
  Serial.flush();
  //
}
void loop() {
  newReading = MSGEQ7.read(MSGEQ7_INTERVAL);
  //check if we need to switch animation via serial
  if (Serial.available() > 0) {
    //read the byte into a temp buffer
    int in_byte = Serial.read();
    in_byte = in_byte - 48;
    //post some gr8 memes
    if (! ( (in_byte <= NUM_ANIMATIONS) && (in_byte >= 1)))
    {
      Serial.print("OOR animation number. enter between 1 and ");
      Serial.println(NUM_ANIMATIONS, DEC);
    } else {
      CURRENT_ANI = in_byte;
      Serial.print("Switched to ");
      Serial.println(CURRENT_ANI);
    }
    Serial.flush();
  }

  switch(CURRENT_ANI) {
    case 1:
    //rainbow EQ
    rainbow_eq();
    break;
    case 2:
    //Strobe visualizer
    strobe();
    break;
    case 3:
    waves();
    break;
    case 4:
    rainbowstrobe();
    break;
    case 5:
    rainbowpanel();
    break;
    default:
    loop(); 
  }
}

void rainbow_eq () {
  //this is the "rainbow EQ" animation. the OG pimp daddy of the lightboard animations.
  
  //roll the hue wheel
  EVERY_N_MILLISECONDS( 20 ) { HUE++; } // slowly cycle the "base color" through the rainbow
    
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
        leds[band*BAND_HEIGHT+m].setHSV(HUE+(18*band),SAT,255);
        j = j-255;
        m++;
      }
      leds[band*BAND_HEIGHT+m].setHSV(HUE+(18*band),SAT,j);
    }
    FastLED.show();
  }
}

void strobe() {
  //deadmau5's ch00na
  
  if (newReading) {
  // visualize the average bass of both channels
    uint8_t val = max(MSGEQ7.get(0), MSGEQ7.get(1));
//  uint8_t val = MSGEQ7.get(0);

    // Reduce noise
    val = mapNoise(val);
    if (val <= 80) {
      val = 0;
    } else {
    val = map(val, 80, 255, 0, 255);
    }

    // Visualize leds to the beat
    CRGB color = CRGB::White;
    color.nscale8_video(val);
    fill_solid(leds, NUM_LEDS, color);

    // Update Leds
    FastLED.show();
  }
}

void waves() {
//  memset(LEVELS, 0, sizeof(LEVELS));

}

void rainbowstrobe() {
  if (newReading) {
  // visualize the average bass of both channels
    uint8_t val = max(MSGEQ7.get(0), MSGEQ7.get(1));
//  uint8_t val = MSGEQ7.get(0);

    // Reduce noise
    val = mapNoise(val);
    if (val <= 80) {
      val = 0;
    } else {
    val = map(val, 80, 255, 0, 255);
    }
    if (val == 0) {
      //we off, randomize that shit
      HUE = random8();
    }

    // Visualize leds to the beat
//    color.nscale8_video(val);
    fill_solid(leds, NUM_LEDS, CHSV( HUE, SAT, val));

    // Update Leds
    FastLED.show();
  }
}

void rainbowpanel() {
  //
  FastLED.clear();
  if (newReading) {
  // visualize the average bass of both channels
    uint8_t val = max(MSGEQ7.get(0), MSGEQ7.get(1));
//  uint8_t val = MSGEQ7.get(0);

    // Reduce noise
    val = mapNoise(val);
    if (val <= 80) {
      val = 0;
      dropped = false;
    } else {
      if (dropped) {
        //
      } else {
        HUE = random8();
        dropped = true;
      }
    }

  fill_solid(leds, NUM_LEDS, CHSV( HUE, SAT, 255));
  FastLED.show();
}
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
