#include "FastLED.h"
#include <ADC.h>

// How many leds in your strip?
#define NUM_LEDS 84


#define DATA_PIN 10

// Define the array of leds
CRGB leds[NUM_LEDS];

int strobe = 2; // strobe pins on digital 4
int res = 3; // reset pins on digital 5
int left[7]; // store band values in these arrays
int right[7];
int band;
int col;
const int leftPin = A0; // ADC0
const int rightPin = A3; // ADC1

void setup()
{
 FastLED.addLeds<WS2811,DATA_PIN,GRB>(leds, NUM_LEDS);
 pinMode(leftPin, INPUT);
 pinMode(rightPin, INPUT);
 pinMode(res, OUTPUT); // reset
 pinMode(strobe, OUTPUT); // strobe
 digitalWrite(res,LOW); // reset low
 digitalWrite(strobe,HIGH); //pin 5 is RESET on the shield
}
void readMSGEQ7()
// Function to read 7 band equalizers
{
 digitalWrite(res, HIGH);
 digitalWrite(res, LOW);
 for(band=0; band <7; band++)
 {
 digitalWrite(strobe,LOW); // strobe pin on the shield - kicks the IC up to the next band 
 delayMicroseconds(30); // 
 left[band] = analogRead(leftPin); // store left band reading
 //right[band] = analogRead(rightPin); // ... and the right
 digitalWrite(strobe,HIGH); 
 }
}
void loop()
{
 readMSGEQ7();
 // display values of left channel on serial monitor 
 for (band = 0; band < 7; band++)
 {
   col = map(left[band], 0, 1023, 0, 255);
   for (int x = 0; x < 12; x++)
   {
     leds[band * (12 * (x + 1))].setRGB(0, col, 0);
   }
//   leds[band*12].setRGB( 0, col , 0);
 }
 FastLED.show();
 
 
// display values of right channel on serial monitor
 for (band = 0; band < 7; band++)
 {
   
 }
}
