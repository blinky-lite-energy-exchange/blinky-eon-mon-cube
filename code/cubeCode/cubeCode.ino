#include "BlinkyBus.h"
#define BAUD_RATE  19200
#define commLEDPin   13
#define BLINKYBUSBUFSIZE  11
union BlinkyBusUnion
{
  struct
  {
    int16_t state;
    int16_t year;
    int16_t  month;
    int16_t  day;
    int16_t  hour;
    int16_t  min;
    int16_t  sec;
    int16_t  GWh;
    int16_t  MWh;
    int16_t  kWh;
    int16_t  Wh;
  };
  int16_t buffer[BLINKYBUSBUFSIZE];
} bb;
BlinkyBus blinkyBus(bb.buffer, BLINKYBUSBUFSIZE, Serial2, commLEDPin);

void setup() 
{

  bb.state = 1; //init
  bb.year = 0;
  bb.month = 0;
  bb.day = 0;
  bb.hour = 0;
  bb.min = 0;
  bb.sec = 0;
  bb.GWh = 0;
  bb.MWh = 0;
  bb.kWh = 0;
  bb.Wh  = 0;

  Serial1.begin(115200,SERIAL_8N1_RXINV);
  Serial2.begin(BAUD_RATE);
  blinkyBus.start();
}

void loop() 
{
  blinkyBus.poll();
  readTelegram();
}
void readTelegram()
{
  char telegram[64];
  String dumpy;
  String dataString;

  if (Serial1.available() )
  {
    while (Serial1.available()) 
    {
      memset(telegram, 0, sizeof(telegram));
      Serial1.readBytesUntil('\n',telegram, 64);
      dumpy = telegram;
      if (dumpy.indexOf("0-0:1.0.0") >= 0)
      {
        dataString = dumpy.substring( dumpy.indexOf("(") + 1, dumpy.indexOf(")") );
        bb.year  = (int16_t ) dataString.substring( 0, 2).toInt();
        bb.month = (int16_t ) dataString.substring( 2, 4).toInt();
        bb.day   = (int16_t ) dataString.substring( 4, 6).toInt();
        bb.hour  = (int16_t ) dataString.substring( 6, 8).toInt();
        bb.min   = (int16_t ) dataString.substring( 8,10).toInt();
        bb.sec   = (int16_t ) dataString.substring(10,12).toInt();
      }
      if (dumpy.indexOf("1-0:1.8.0") >= 0)
      {
        dataString = dumpy.substring( dumpy.indexOf("(") + 1, dumpy.indexOf(")") );
        bb.GWh  = (int16_t ) dataString.substring( 0, 2).toInt();
        bb.MWh  = (int16_t ) dataString.substring( 2, 5).toInt();
        bb.kWh  = (int16_t ) dataString.substring( 5, 8).toInt();
        bb.Wh   = (int16_t ) dataString.substring( 9,12).toInt();
      }
    }
  }
}
