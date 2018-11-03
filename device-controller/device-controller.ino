#include <ADC.h>
#define PWMINBUFMAX 5
const int pwmOutPin = 38;
const int pwmInPin = 37;
const int counterPin = 36;
const int detPin = A0; // ADC0

int pwMValue = 2048;
int pwmFrequency = 40250;
int pwmResolution = 12;

ADC *adc = new ADC(); // adc object
float detPinValue = 0;
float avgDetPinValue = 0.0;
float nsamples = 500.0;
unsigned long lastWriteTime = 0;
unsigned long nowTime;
float pwmInI;
float pwmInQ;
float overlapI = 0;
float overlapQ = 0;

boolean counterPinValue = false;
int counter = 0;
int maxCounter = 10;
int deltaT;

int pwmInBuf[PWMINBUFMAX];
int pwmBufPointerI = PWMINBUFMAX - 1;
int pwmBufPointerQ = 0;

void setup() 
{
  for (int ii = 0; ii < PWMINBUFMAX; ++ii) pwmInBuf[ii] = 0;
  lastWriteTime = millis();
  analogWriteResolution(pwmResolution);   
  analogWriteFrequency(pwmOutPin, pwmFrequency);  
  pinMode(pwmOutPin, OUTPUT);  
  pinMode(pwmInPin, INPUT);  
  pinMode(counterPin, OUTPUT);  


  analogWrite(pwmOutPin, pwMValue);
  pinMode(detPin, INPUT);
  adc->setAveraging(1); // set number of averages
  adc->setResolution(12); // set bits of resolution
//  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
//  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED); // change the sampling speed
//  adc->enableInterrupts(ADC_0);
  adc->startContinuous(detPin, ADC_0);
  Serial.begin(115200);

}

void loop() 
{
//  delayMicroseconds(1);
  detPinValue = (float) adc->analogReadContinuous(ADC_0);
  avgDetPinValue = avgDetPinValue + (detPinValue - avgDetPinValue) / nsamples;

  
  pwmInBuf[pwmBufPointerI] = digitalRead(pwmInPin);
  pwmInI = (float) pwmInBuf[pwmBufPointerI];
  pwmInQ = (float) pwmInBuf[pwmBufPointerQ];
     
  overlapI = overlapI + ((detPinValue * pwmInI) - overlapI) / nsamples;
  overlapQ = overlapQ + ((detPinValue * pwmInQ) - overlapQ) / nsamples;

  ++pwmBufPointerI;
  ++pwmBufPointerQ;
  if (pwmBufPointerI == PWMINBUFMAX) pwmBufPointerI = 0;
  if (pwmBufPointerQ == PWMINBUFMAX) pwmBufPointerQ = 0;
 
/*
  ++counter;
  if (counter >  maxCounter)
  {
    digitalWrite(counterPin, counterPinValue);
    counterPinValue = !counterPinValue;
    counter = 0;
  }
*/
  nowTime = millis();
  deltaT = (int) (nowTime - lastWriteTime);
  if (deltaT > 1000)
  {
    lastWriteTime = nowTime;
    Serial.print(avgDetPinValue);
    Serial.print(" ");
    Serial.print(overlapI);
    Serial.print(" ");
    Serial.println(overlapQ);
  }
}
