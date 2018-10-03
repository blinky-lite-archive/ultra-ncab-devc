#include "device-controller.h"
int loopDelaymS = 100;

void setup()
{
  setupCommunications(true, 115200);
  delay(200);
}

void loop()
{
  if (readSerial())
  {
    printMessage(getInputTopic(), getInputPayload());
  }
  delay(loopDelaymS);
}

