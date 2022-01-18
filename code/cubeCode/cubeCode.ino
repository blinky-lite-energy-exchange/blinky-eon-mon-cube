#include "BlinkyBus.h"
#define BAUD_RATE  19200
#define commLEDPin    3
#define eonLedPin    13
#define eonPin        9
#define batMonPin    A0

#define BLINKYBUSBUFSIZE  4
union BlinkyBusUnion
{
  struct
  {
    int16_t state;
    int16_t batMon;
    uint32_t eonCount;
  };
  int16_t buffer[BLINKYBUSBUFSIZE];
} bb;
BlinkyBus blinkyBus(bb.buffer, BLINKYBUSBUFSIZE, Serial1, commLEDPin);

volatile boolean eonLED = true;
volatile uint32_t eonCount = 0;
int batCount = 0;

void setup() 
{
  pinMode(eonLedPin,  OUTPUT);
  pinMode(eonPin,  INPUT);
  pinMode(batMonPin,  INPUT);
  attachInterrupt(eonPin, eonPulseHandler, RISING);
  digitalWrite(eonLedPin, eonLED);

  bb.state = 0; //don't ask for init
  bb.batMon = 0;
  bb.eonCount = 0;

  Serial1.begin(BAUD_RATE);
//  Serial.begin(9600);
  blinkyBus.start();

}

void loop() 
{
  delay(50);
  bb.eonCount = eonCount; 
  blinkyBus.poll();
  ++batCount;
  if (batCount >= 40)
  {
    batCount = 0;
    bb.batMon = (int16_t) analogRead(batMonPin);
/*
    Serial.print(bb.batMon);
    Serial.print(", ");
    Serial.println(bb.eonCount);
*/    
  }
}
void eonPulseHandler()
{
  eonLED = !eonLED;
  digitalWrite(eonLedPin, eonLED);
  if (eonCount < 0xFFFFFFFF)
  {
    eonCount = eonCount + 1;
  }
  else
  {
    eonCount = 0;
  }
}
