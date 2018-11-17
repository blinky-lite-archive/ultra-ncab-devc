#define FFTPTS 512
#define FFTPTS2 1024
#define FFTPTS8 4096
#define SAMPLEINTERVAL 2000
#define DIGRATE 16 //uS per second

const int pwmPin = 29;
const int pwmRisePin = 28;
const int pwmFallPin = 27;
const int transPin = 39;
const int pwmIPin = 26;
const int pwmQPin = 25;
const int ledPin = 13;
const int inIPin = A0;
const int inQPin = A1;

int pwmValue = 128;
int pwmFrequency = 80850;
int pwmResolution = 8;
boolean ledPinValue = true;
volatile boolean pwmIValue = false;
volatile boolean pwmQValue = false;
float inIValue;
float inQValue;

int ifftCounterI = 0;
int ifftCounterQ = FFTPTS;
float nsamples = 30.0;
unsigned long lastWriteTime = 0;
unsigned long nowTime;
float isample = 0.0;
boolean readyToReceiveFlag = false;
unsigned long deltaT;
unsigned long dataStartTime = 0;

typedef union
{
 float number[FFTPTS2];
 uint8_t bytes[FFTPTS8];
} FLOATUNION_t;
FLOATUNION_t overlap;


void setup() 
{
  analogWriteResolution(pwmResolution);   
  analogWriteFrequency(pwmPin, pwmFrequency);  
  pinMode(pwmPin, OUTPUT);  
  pinMode(transPin, OUTPUT);  
  pinMode(pwmIPin, OUTPUT);  
  pinMode(transPin, OUTPUT);  
  pinMode(pwmQPin, OUTPUT);  
  pinMode(pwmRisePin, INPUT);  
  pinMode(pwmFallPin, INPUT);  
  pinMode(inIPin, INPUT);  
  pinMode(inQPin, INPUT);  
  digitalWrite(ledPin, ledPinValue);
  digitalWrite(transPin, pwmIValue);
  digitalWrite(pwmIPin, pwmIValue);
  digitalWrite(pwmQPin, pwmQValue);
  digitalWrite(ledPin, ledPinValue);
  attachInterrupt(pwmRisePin, pwmRisePinHandler, RISING);
  attachInterrupt(pwmFallPin, pwmFallPinHandler, FALLING);
  analogWrite(pwmPin, pwmValue);

  nsamples = (SAMPLEINTERVAL / DIGRATE) / 4.0;
  if (nsamples < 1.0) nsamples = 1.0;

  ifftCounterI = 0;
  ifftCounterQ = FFTPTS;
  lastWriteTime = micros();

  for (int ii = 0; ii < FFTPTS2; ++ii) 
  {
    overlap.number[ii] = 0.0;
  }

  Serial1.begin(115200);
//  Serial.begin(9600);

}

void loop() 
{
  if (readyToReceiveFlag)
  {
    if (ifftCounterI < 1) dataStartTime = micros();
    inIValue = (float) analogRead(inIPin);
    inQValue = (float) analogRead(inQPin);
     
    overlap.number[ifftCounterI] = overlap.number[ifftCounterI] + (inIValue - overlap.number[ifftCounterI]) / nsamples;
    overlap.number[ifftCounterQ] = overlap.number[ifftCounterQ] + (inQValue - overlap.number[ifftCounterQ]) / nsamples;
  
    isample = isample + 1.0;
    nowTime = micros();
    if ((nowTime - lastWriteTime) > SAMPLEINTERVAL)
    {
      lastWriteTime = nowTime;
      ++ifftCounterI;
      ++ifftCounterQ;
      isample = 0.0;
    }
    if (ifftCounterI == FFTPTS)
    {
/*
      Serial.print(inIValue);
      Serial.print(" ");
      Serial.print(overlap.number[0]);
      Serial.print(" ");
      Serial.print(inQValue);
      Serial.print(" ");
      Serial.println(overlap.number[FFTPTS]);
 */     
      deltaT = micros() - dataStartTime;
      Serial1.write(overlap.bytes, FFTPTS8);
      readyToReceiveFlag = false;
  
  
      for (int ii = 0; ii < FFTPTS; ++ii) 
      {
        overlap.number[ii] = 0;
        overlap.number[ii + FFTPTS] = 0;
      }
      ledPinValue = !ledPinValue;
      digitalWrite(ledPin, ledPinValue);
  
      ifftCounterI = 0;
      ifftCounterQ = FFTPTS;
      lastWriteTime = micros();
   }
  }
  while(Serial1.available() > 0)
  {
    char lastRecvd = Serial1.read();
    if (lastRecvd == '1')
    {
     readyToReceiveFlag = true;
    }
  }
}
void pwmRisePinHandler() 
{
  pwmIValue = !pwmIValue;
  digitalWrite(transPin, pwmIValue);
  digitalWrite(pwmIPin, pwmIValue);
}
void pwmFallPinHandler() 
{
  pwmQValue = !pwmQValue;
  digitalWrite(pwmQPin, pwmQValue);
}
