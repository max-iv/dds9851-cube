#define BAUD_RATE 115200
#define SETTING_EXTRA_BUFFER_SIZE 14
#define READING_EXTRA_BUFFER_SIZE 10
const boolean printDiagnosticText = false;

const int led13Pin = 13;
const int blueLEDPin = 2;
const int geigerPin = 3;
const int geigerLedPin = 5;

boolean blueLED = false;
boolean geigerLED = false;
unsigned long duration;
unsigned long lastPulse;
unsigned long nowTime;

struct Readings
{
  int cubeInit;
  int newSettingDone;
  int intervalTime;
  float avgInterval;
  float cpm;
  float nsamples;
  int extraInfo[READING_EXTRA_BUFFER_SIZE];
};
Readings readings;

struct Settings
{
  int newSetting;
  float nsamples;
  int extraInfo[SETTING_EXTRA_BUFFER_SIZE];
};
Settings settings;

void setup()
{
  setupPins();
  Serial1.begin(BAUD_RATE);
  delay(2000);
  lastPulse = 0;
  nowTime = 0;
  initReadingsAndSettings();

}

void loop()
{
  duration = pulseIn(geigerPin, HIGH);
  if (duration > 200)
  {
    nowTime = millis();
    if (lastPulse > 0)
    {
      if (readings.intervalTime > 0)
      {
        readings.intervalTime = (int) (nowTime - lastPulse);
        readings.avgInterval = readings.avgInterval + (((float) readings.intervalTime) - readings.avgInterval) / readings.nsamples;
      }
      else
      {
        readings.intervalTime = (int) (nowTime - lastPulse);
        readings.avgInterval = (float) readings.intervalTime;
      }
      lastPulse = nowTime;
      readings.cpm = 60000.0 / readings.avgInterval;
    }
    lastPulse = nowTime;

    if (printDiagnosticText)
    {
      Serial1.println(" ");
      Serial1.print("interval : ");
      Serial1.println(readings.intervalTime);
      Serial1.print("avgInterval : ");
      Serial1.println(readings.avgInterval);
      Serial1.print("cpm : ");
      Serial1.println(readings.cpm);
    }
    else
    {
      readings.newSettingDone = 0;
      
      if(Serial1.available() > 0)
      { 
        blueLED = !blueLED;
        digitalWrite(blueLEDPin, blueLED);
        Serial1.readBytes((uint8_t*)&settings, 64);
        
        if (settings.newSetting > 0)
        {
          lastPulse = 0;
          nowTime = 0;
          readings.intervalTime = 0;
          readings.nsamples = settings.nsamples;
          readings.newSettingDone = 1;
          readings.cubeInit = 0;
        }
      }
      Serial1.write((uint8_t*)&readings, 64);

    }
    geigerLED = !geigerLED;
    digitalWrite(geigerLedPin, geigerLED);
  }  
}
void setupPins()
{
  pinMode(led13Pin, OUTPUT);    
  pinMode(blueLEDPin, OUTPUT);    
  pinMode(geigerLedPin, OUTPUT);    
  pinMode(geigerPin, INPUT);
  blueLED = false;  
  geigerLED = false;  
  digitalWrite(blueLEDPin, blueLED);
  digitalWrite(led13Pin, blueLED);
  digitalWrite(geigerLedPin, geigerLED);
}
void initReadingsAndSettings()
{
  readings.intervalTime = 0;
  readings.avgInterval = 0.0;
  readings.cpm = 0.0;
  readings.newSettingDone = 0;
  
  readings.nsamples = 1.0;
  settings.newSetting = 0;
  readings.cubeInit = 1;
  for (int ii = 0; ii < READING_EXTRA_BUFFER_SIZE; ++ii) readings.extraInfo[ii] = 0;
  for (int ii = 0; ii < SETTING_EXTRA_BUFFER_SIZE; ++ii) settings.extraInfo[ii] = 0;
}
