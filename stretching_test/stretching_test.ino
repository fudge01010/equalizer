#include "FastLED.h"
#include "ADC.h"

// LED definitions
#define NUM_LEDS 84
#define DATA_PIN 10
CRGB leds[NUM_LEDS];

//pin definitions
#define strobe = 4;         // strobe pins on digital 4
#define res = 5;            // reset pins on digital 5
#define leftPin = A0;
#define rightPin = A1;


int left[7];            // store band values in these arrays
int right[7];
int band;
int col;
int lowOverlap = 20;    // Level of overlap into lower bound from previous
int hightOverlap = 20;  // Level of overlap into higher bound from next
int cutOff = 90;        // Cut-off for noise

void setup()
{
 FastLED.addLeds<WS2811,DATA_PIN,GRB>(leds, NUM_LEDS);
 ADC *adc = new ADC(); // adc object
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
 left[band] = adc->analogRead(leftPin);
 //right[band] = analogRead(1); // ... and the right
 digitalWrite(strobe,HIGH); 
 }
}
void loop()
{
 readMSGEQ7();
 // display values of left channel on serial monitor
 for(int i = 0; i < 7; i++)
 {
    if(left[i] < cutOff){
        left[i] = 0;
    }
 }
 for (band = 0; band < 7; band++)
 {
   // col = map(left[band], 0, 1023, 0, 255);
   for (int x = 0; x < 12; x++)
   {
     if (x == 0)
     {
       col = map(left[band], 0, 85, 0, 255);
       //  col = map(left[band], 0, 85 + highOverlap, 0, 255);
     } else 
     {
        col = map(left[band], (x * 85) + 1), ((x + 1) * 85), 0, 255);
        // Overlap code
        //  col = map(left[band], (x * 85) - lowOverlap + 1), ((x + 1) * 85) + highOverlap, 0, 255);
        leds[((12 * band) + x)].setRGB(0, col, 0);
     }
   }
//   leds[band*12].setRGB( 0, col , 0);
 }
  FastLED.show();
 
 
// display values of right channel on serial monitor
  for (band = 0; band < 7; band++)
 {
   
 }
}
