#include <ADC.h>
#define PWMINBUFMAX 5
#define FFTPTS 256
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
float overlapI[FFTPTS];
float overlapQ[FFTPTS];
int ifftCounter = 0;
float isample = 0.0;

boolean counterPinValue = false;
int counter = 0;
int maxCounter = 10;
unsigned long deltaT;
unsigned long sampleInterval = 7000;
String outputData;

int pwmInBuf[PWMINBUFMAX];
int pwmBufPointerI = PWMINBUFMAX - 1;
int pwmBufPointerQ = 0;

void setup() 
{
  for (int ii = 0; ii < PWMINBUFMAX; ++ii) pwmInBuf[ii] = 0;
  for (int ii = 0; ii < FFTPTS; ++ii) 
  {
    overlapI[ii] = 0;
    overlapQ[ii] = 0;
  }
  ifftCounter = 0;
  lastWriteTime = micros();
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
  Serial1.begin(115200);

}

void loop() 
{
//  delayMicroseconds(1);
  detPinValue = (float) adc->analogReadContinuous(ADC_0);
  avgDetPinValue = avgDetPinValue + (detPinValue - avgDetPinValue) / nsamples;

  
  pwmInBuf[pwmBufPointerI] = digitalRead(pwmInPin);
  pwmInI = (float) pwmInBuf[pwmBufPointerI];
  pwmInQ = (float) pwmInBuf[pwmBufPointerQ];
     
  overlapI[ifftCounter] = overlapI[ifftCounter] + ((detPinValue * pwmInI) - overlapI[ifftCounter]) / nsamples;
  overlapQ[ifftCounter] = overlapQ[ifftCounter] + ((detPinValue * pwmInQ) - overlapQ[ifftCounter]) / nsamples;

  isample = isample + 1.0;

  ++pwmBufPointerI;
  ++pwmBufPointerQ;
  if (pwmBufPointerI == PWMINBUFMAX) pwmBufPointerI = 0;
  if (pwmBufPointerQ == PWMINBUFMAX) pwmBufPointerQ = 0;
 
  nowTime = micros();
  if ((nowTime - lastWriteTime) > sampleInterval)
  {
    lastWriteTime = nowTime;
    ++ifftCounter;
    nsamples = isample / 2.0;
    isample = 0.0;
  }
  if (ifftCounter == FFTPTS)
  {
    outputData = "*," + floatToString(nsamples * 2.0, 2);
    Serial1.println(outputData);
    for (int ii = 0; ii < FFTPTS; ++ii) 
    {
      outputData = floatToString(overlapI[ii], 2) + "," + floatToString(overlapQ[ii], 2);
      Serial1.println(outputData);
      overlapI[ii] = 0;
      overlapQ[ii] = 0;
    }
    ifftCounter = 0;
    lastWriteTime = micros();
    
  }
}
String floatToString( float val, unsigned int precision)
{
  char charBuf[32];
  dtostrf(val, 31, precision, charBuf);
  charBuf[31] = '\0';
  String output = String(charBuf);
  output.trim();
  return output;
}

