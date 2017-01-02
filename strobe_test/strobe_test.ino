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

// U8GLIB
//#include "U8glib.h"
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);  // Fast I2C / TWI (A4 and A5)

//define avicii's hit 2011 tune 
int LEVELS[13];

//define some measurements for the size calculations:
#define BAND_HEIGHT 24
#define BANDS 13

int SAT = 0;
int HUE = 0;

void setup() {
  // FastLED setup
  FastLED.addLeds<CHIPSET, LED_PINS, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(CORRECTION);
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.setDither(LED_DITHER);
  FastLED.show(); // needed to reset leds to zero
  
  //set power limit
//  FastLED.setMaxPowerInVoltsAndMilliamps(5,2000); 
  
  // This will set the IC ready for reading
  MSGEQ7.begin();

  //set params for OLED stuff
//  u8g_prepare();
}



void loop() {
  //roll the hue wheel
  EVERY_N_MILLISECONDS( 20 ) { HUE++; } // slowly cycle the "base color" through the rainbow

  // Analyze without delay
  bool newReading = MSGEQ7.read(MSGEQ7_INTERVAL);

//  if (newReading)
//  {
////    u8g.firstPage();
//    //clear LED's, ready for new data
//    FastLED.clear();
//    //interpolate levels between channels
////    interpolateChannels();
//
////    int maxbright = max(MSGEQ7.get(0), MSGEQ7.get(1));    //mapping for both kick and bass
//    int maxbright = MSGEQ7.get(0); 
//    maxbright = mapNoise(maxbright);
//    maxbright = map(maxbright, 0, 255, 0,250);
//    fill_solid(leds, NUM_LEDS, CHSV(HUE, SAT, maxbright));
//
//    FastLED.show();
//    
//  }

  
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
  // Led strip output
  if (newReading) {
    // visualize the average bass of both channels
//    uint8_t val = MSGEQ7.get(MSGEQ7_BASS);
    uint8_t val = max(MSGEQ7.get(0), MSGEQ7.get(1));
//    uint8_t val = MSGEQ7.get(0);

    // Reduce noise
    val = mapNoise(val);

    // Visualize leds to the beat
    CRGB color = CRGB::White;
    color.nscale8_video(val);
    fill_solid(leds, NUM_LEDS, color);

    // Update Leds
    FastLED.show();
    Serial.println(val);
  }
}


//void u8g_prepare(void) {
//  u8g.setFont(u8g_font_6x10);
//  u8g.setFontRefHeightExtendedText();
//  u8g.setDefaultForegroundColor();
//  u8g.setFontPosTop();
//}
