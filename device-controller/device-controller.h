boolean writeToSerial1 = false ;
const int buffLength = 128;
char inputBuff[buffLength];
int buffPointer = 0;
String inputTopic = "";
String inputPayload = "";

String getInputTopic()
{
  return inputTopic;
}
String getInputPayload()
{
  return inputPayload;
}
void setupCommunications(boolean useSerial1, int baudRate)
{
  writeToSerial1 = useSerial1;
  if (writeToSerial1) {Serial1.begin(baudRate);} else {Serial.begin(baudRate);}
}
void printStringToSerial(String inputString)
{
  if (writeToSerial1) {Serial1.print(inputString);} else {Serial.print(inputString);}
}
void parseInputString(String inputString)
{
  inputTopic = inputString.substring(inputString.indexOf("<topic>") + 7, inputString.indexOf("</topic>"));
  inputPayload = inputString.substring(inputString.indexOf("<payload>") + 9, inputString.indexOf("</payload>"));
}
boolean dataOnSerial()
{
  inputTopic = "";
  inputPayload = "";
  boolean validMessage = false;
  if(writeToSerial1)
  {
    String inputString = "";
    while(Serial1.available() > 0)
    {
      char lastRecvd = Serial1.read();
      if (lastRecvd == '!')
      {
        inputString = String((char*) inputBuff);
        buffPointer = 0;
        for (int ii = 0; ii < buffLength; ++ii) inputBuff[ii] = '\0';
        parseInputString(inputString);
        if (inputTopic.length() > 0) validMessage = true;
        return validMessage;
      }
      else
      {
        inputBuff[buffPointer] = lastRecvd;
        ++buffPointer;
      }
    }
  }
  else
  {
    String inputString = "";
    while(Serial.available() > 0)
    {
      char lastRecvd = Serial.read();
      if (lastRecvd == '!')
      {
        inputString = String((char*) inputBuff);
        buffPointer = 0;
        for (int ii = 0; ii < buffLength; ++ii) inputBuff[ii] = '\0';
        parseInputString(inputString);
        if (inputTopic.length() > 0) validMessage = true;
        return validMessage;
      }
      else
      {
        inputBuff[buffPointer] = lastRecvd;
        ++buffPointer;
      }
    }
  }
  return validMessage;
}
void printMessage(String topic, String payload)
{
  printStringToSerial("<topic>" + topic + "</topic><payload>" + payload + "</payload>!");
}
int stringToInt(String inputString)
{
  return inputString.toInt();
}
String intToString(int inputInt)
{
  return String(inputInt);
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
float stringToFloat(String inputString)
{
  return inputString.toFloat();
}
boolean stringToBoolean(String inputString)
{
  boolean val = false;
  if (inputString.toLowerCase() == "true") val = true;
  return val;
}
String booleanToString(boolean inputBoolean)
{
  String ouputString = "false";
  if (inputBoolean) ouputString = "true";
  return String(inputBoolean);
}
