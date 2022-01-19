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

volatile boolean eonLED = false;
volatile uint32_t eonCount = 0;
int batCount = 0;
boolean permitEonCountWrite = true;

void setup() 
{
  for (int ii = 2; ii <24; ++ii)
  {
    pinMode(ii,  OUTPUT);
    digitalWrite(ii, LOW);
  }
  pinMode(eonLedPin,  OUTPUT);
  pinMode(eonPin,  INPUT);
  pinMode(batMonPin,  INPUT);
  attachInterrupt(eonPin, eonPulseHandler, RISING);
  digitalWrite(eonLedPin, eonLED);

  bb.state = 1; //init
  bb.batMon = 0;
  bb.eonCount = 0;

  Serial1.begin(BAUD_RATE);
  blinkyBus.start();
}

void loop() 
{
  int ipoll = 0;
  delay(50);
  if (permitEonCountWrite) bb.eonCount = eonCount; 
  ipoll = blinkyBus.poll();
  if (ipoll == 2)
  {
    if (blinkyBus.getLastWriteAddress() == 2) permitEonCountWrite = false;
    if (blinkyBus.getLastWriteAddress() == 3)
    {
      eonCount = bb.eonCount;
      permitEonCountWrite = true;
    }
  }
  ++batCount;
  if (batCount >= 80)
  {
    batCount = 0;
    bb.batMon = (int16_t) analogRead(batMonPin);
  }
}
void eonPulseHandler()
{
//  eonLED = !eonLED;
//  digitalWrite(eonLedPin, eonLED);
  if (eonCount < 0xFFFFFFFF)
  {
    eonCount = eonCount + 1;
  }
  else
  {
    eonCount = 0;
  }
}
