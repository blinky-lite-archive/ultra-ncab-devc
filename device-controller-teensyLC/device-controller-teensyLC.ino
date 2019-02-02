#define FFTPTS 128
#define FFTPTS2 256
#define FFTPTS8 1024
#define SAMPLEINTERVAL 2000

const int pwmPin = 9;
const int pwmRisePin = 8;
const int pwmFallPin = 7;
const int transPin = 23;
const int pwmIPin = 6;
const int pwmQPin = 5;
const int ledPin = 13;
const int inIPin = A0;
const int inQPin = A1;

int pwmValue = 128;
int pwmFrequency = 80000;
int pwmResolution = 8;
boolean ledPinValue = false;
volatile boolean pwmIValue = false;
volatile boolean pwmQValue = false;
float inIValue;
float inQValue;

int ifftCounterI = 0;
int ifftCounterQ = FFTPTS;
unsigned long lastWriteTime = 0;
unsigned long nowTime;
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
  pinMode(ledPin, OUTPUT);
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
    overlap.number[ifftCounterI] = (float) analogRead(inIPin);
    overlap.number[ifftCounterQ] = (float) analogRead(inQPin);

    nowTime = micros();
    if ((nowTime - lastWriteTime) > SAMPLEINTERVAL)
    {
      lastWriteTime = nowTime;
      ++ifftCounterI;
      ++ifftCounterQ;
    }
    if (ifftCounterI == FFTPTS)
    {
      deltaT = micros() - dataStartTime;

      //      Serial.print(overlap.number[0]);
      //      Serial.print(" ");
      //      Serial.println(overlap.number[FFTPTS]);

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
  while (Serial1.available() > 0)
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
