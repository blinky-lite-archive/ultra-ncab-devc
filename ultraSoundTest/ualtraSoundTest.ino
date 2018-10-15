#include <ADC.h>
int pwmOutPin = 2;
int pwmInPin = 3;
int nsamplePin = 4;
int pwMValue = 2048;
int pwmFrequency = 40250;
int pwmResolution = 12;
const int detPin = A0; // ADC0
ADC *adc = new ADC(); // adc object
float detPinValue = 0;
float avgDetPinValue = 0.0;
float nsamples = 500.0;
unsigned long lastWriteTime = 0;
unsigned long nowTime;
float pwmInPinValue;
float overlap = 0;
float pos = 0;
int deltatT;
boolean nsamplePinValue = false;
int nsampleCount = 0;


void setup() 
{
  lastWriteTime = millis();
  analogWriteResolution(pwmResolution);   
  analogWriteFrequency(pwmOutPin, pwmFrequency);  
  pinMode(pwmOutPin, OUTPUT);  
  pinMode(pwmInPin, INPUT);  
  pinMode(nsamplePin, OUTPUT);  


  analogWrite(pwmOutPin, pwMValue);
  pinMode(detPin, INPUT);
  adc->setAveraging(1); // set number of averages
  adc->setResolution(12); // set bits of resolution
//  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED); // change the conversion speed
//  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED); // change the sampling speed
//  adc->enableInterrupts(ADC_0);
  adc->startContinuous(detPin, ADC_0);
  Serial.begin(9600);

}

void loop() 
{
//  delayMicroseconds(1);
  detPinValue = (float) adc->analogReadContinuous(ADC_0);
  avgDetPinValue = avgDetPinValue + (detPinValue - avgDetPinValue) / nsamples;
  
  pwmInPinValue = (float) digitalRead(pwmInPin);
  overlap = overlap + ((detPinValue * pwmInPinValue) - overlap) / nsamples;
  pos = 0.0;
  if (avgDetPinValue > 0.5)
  {
    pos = 10.0 * overlap / avgDetPinValue;
  }
  ++nsampleCount;
  if (nsampleCount >= (int) nsamples)
  {
    digitalWrite(nsamplePin, nsamplePinValue);
    nsamplePinValue = !nsamplePinValue;
    nsampleCount = 0;
  }

  nowTime = millis();
  deltatT = (int) (nowTime - lastWriteTime);
  if (deltatT > 1000)
  {
    lastWriteTime = nowTime;
    Serial.print(avgDetPinValue);
    Serial.print(" ");
    Serial.print(overlap);
    Serial.print(" ");
    Serial.println(pos);
  }
}
