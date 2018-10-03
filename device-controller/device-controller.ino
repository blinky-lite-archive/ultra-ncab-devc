#include "device-controller.h"

int loopCount = 0;
int loopCountMax = 1000;
float deltaMicros = 0;
unsigned long nowTime;
unsigned long loopStartTime;


void setup()
{
  setupCommunications(true, 115200);
  delay(200);
  nowTime = micros();
  loopStartTime = nowTime;
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
  if (loopCount > loopCountMax)
  {
      nowTime = micros();
      deltaMicros = (float)(nowTime - loopStartTime) / ((float) loopCountMax);
      loopCount = 0;
      loopStartTime = nowTime;
  }
}

