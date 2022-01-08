#include "ModbusRtu.h"
#define BAUD_RATE      9600
#define MODBUSBUFSIZE  4
#define irLedPin      13
#define irPin          9
#define batMonPin     A0
#define commLEDPin     3

/**
 *  Modbus object declaration
 *  u8id : node id = 0 for master, = 1..247 for slave
 *  port : serial port
 *  u8txenpin : 0 for RS-232 and USB-FTDI 
 *               or any pin number > 1 for RS-485
 */
Modbus slave(1,Serial1,0); // this is slave @1 and RS-232 or USB-FTDI

union ModbusUnion
{
  struct
  {
    uint16_t initCube;
    uint16_t batMon;
    uint32_t irCount;
  };
  uint16_t modbusBuffer[MODBUSBUFSIZE];
} mb;
boolean commLED = true;
uint16_t msgCnt = 0;

volatile boolean irLED = false;
volatile uint32_t irCount = 0;

void setup() 
{
  pinMode(irLedPin, OUTPUT);
  pinMode(commLEDPin, OUTPUT);
  pinMode(irPin,  INPUT);
  pinMode(batMonPin,  INPUT);
  attachInterrupt(irPin, irPulseHandler, RISING);

  mb.initCube               = 1;
  mb.batMon                 = 0;
  mb.irCount                = 0;

//  Serial.begin(9600);
  Serial1.begin(BAUD_RATE);
  slave.start();
  
  digitalWrite(irLedPin, irLED);
  digitalWrite(commLEDPin, commLED);
  delay(5000);
  commLED = !commLED;
  digitalWrite(commLEDPin, commLED);
}

void loop() 
{
  mb.batMon = (uint16_t) analogRead(batMonPin);
  mb.irCount = irCount;
  slave.poll( mb.modbusBuffer, MODBUSBUFSIZE );
  checkComm();
  delay(50);
//  blinkCommLED();
}

void irPulseHandler()
{
  irLED = !irLED;
  digitalWrite(irLedPin, irLED);
  if (irCount < 0xFFFFFFFF)
  {
    irCount = irCount + 1;
  }
  else
  {
    irCount = 0;
  }
}
void checkComm()
{
  uint16_t numMessages;
  numMessages = slave.getInCnt();
  if (numMessages != msgCnt)
  {
    msgCnt = numMessages;
    blinkCommLED();
  }
  
}
void blinkCommLED()
{
  commLED = true;
  digitalWrite(commLEDPin, commLED);
  delay(50);    
  commLED = false;
  digitalWrite(commLEDPin, commLED);
}
