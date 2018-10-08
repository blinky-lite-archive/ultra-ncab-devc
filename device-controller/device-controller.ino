#include "device-controller.h"

int loopCount = 0;
int loopCountMax = 1000;
float deltaMicros = 0;
unsigned long nowTime;
unsigned long loopStartTime;
unsigned long startTime1Hz;
boolean blinky = false;

void setup()
{
  setupCommunications(true, 9600);
//  Serial.begin(9600);
   
  nowTime = micros();
  loopStartTime = nowTime;
  startTime1Hz = nowTime;
  delay(1000);
}

void loop()
{
  if (dataOnSerial())
  {
    if (getInputTopic().equals("getLoopTime"))
    {
      printMessage("loopTime", floatToString(deltaMicros,2));
    }
  }
  ++loopCount;
  nowTime = micros();
  if (loopCount > loopCountMax)
  {     
      deltaMicros = (float)(nowTime - loopStartTime) / ((float)loopCountMax);
      loopCount = 0;
      loopStartTime = nowTime;
  }
  if ((nowTime - startTime1Hz) > 1000000)
  {
    startTime1Hz = nowTime;
    blinky = !blinky;
    printMessage("loopTime", floatToString(deltaMicros,2));
    printMessage("blinky", booleanToString(blinky));
  }
}

