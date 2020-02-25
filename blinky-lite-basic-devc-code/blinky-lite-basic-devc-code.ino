#define BAUD_RATE 115200
#define UPDATE_INTERVAL 1000000

const int led13Pin = 13;
const int blueLEDPin = 2;
int blueLED = 0;
boolean readyToReceiveFlag = true;
unsigned long  nowTime;
unsigned long  lastWriteTime = 0;

struct Readings
{
  int ledState = 0;
};
Readings readings;

struct Settings
{
  int ledState = 1;
};
Settings settings;

void setup()
{
  pinMode(led13Pin, OUTPUT);    
  pinMode(blueLEDPin, OUTPUT);    
  Serial1.begin(BAUD_RATE);
  delay(2000);
  putSettings();
  digitalWrite(blueLEDPin, blueLED);
  lastWriteTime = micros();
}

void loop()
{
  if (readyToReceiveFlag)
  {
    nowTime = micros();
    if ((nowTime - lastWriteTime) > UPDATE_INTERVAL)
    {
      getReadings();
      Serial1.write((uint8_t*)&readings, 4);
      readyToReceiveFlag = false;
    }
  }
  if (!readyToReceiveFlag)
  {
    while(Serial1.available() > 0)
    {
      Serial1.readBytes((uint8_t*)&settings, 4);
      putSettings();
      lastWriteTime = nowTime;
      readyToReceiveFlag = true;
    }
  }
}
void putSettings()
{
  digitalWrite(led13Pin, settings.ledState);
  readings.ledState = settings.ledState;
}
void getReadings()
{
    ++blueLED;
    if (blueLED > 1) blueLED = 0;
    digitalWrite(blueLEDPin, blueLED);
}
