#define BAUD_RATE 115200

const boolean printText = false;

const int led13Pin = 13;
const int blueLEDPin = 2;
const int geigerPin = 3;
const int geigerLedPin = 5;

boolean blueLED = false;
unsigned long duration;
unsigned long lastPulse;
unsigned long now;
float cpm;

struct Readings
{
  int interval;
  float avgInterval;
  float cpm;
  char extraInfo[52];
};
Readings readings;

struct Settings
{
  float nsamples;
  char extraInfo[60];
};
Settings settings;

void setup()
{
  pinMode(led13Pin, OUTPUT);    
  pinMode(blueLEDPin, OUTPUT);    
  pinMode(geigerLedPin, OUTPUT);    
  digitalWrite(geigerLedPin, LOW);
  pinMode(geigerPin, INPUT);    
  Serial1.begin(BAUD_RATE);
  delay(2000);
  digitalWrite(blueLEDPin, blueLED);
  lastPulse = 0;
  now = 0;
  readings.interval = 0;
  readings.avgInterval = 0.0;
  settings.nsamples = 1.0;
  readings.cpm = 0.0;
  for (int ii = 0; ii < 52; ++ii) readings.extraInfo[ii] = -1;

}

void loop()
{
  duration = pulseIn(geigerPin, HIGH);
  if (duration > 200)
  {
    now = millis();
    if (lastPulse > 0)
    {
      if (readings.interval > 0)
      {
        readings.interval = (int) (now - lastPulse);
        readings.avgInterval = readings.avgInterval + (((float) readings.interval) - readings.avgInterval) / settings.nsamples;
      }
      else
      {
        readings.interval = (int) (now - lastPulse);
        readings.avgInterval = (float) readings.interval;
      }
      lastPulse = now;
      readings.cpm = 60000.0 / readings.avgInterval;
    }
    lastPulse = now;

    if (printText)
    {
      Serial1.println(" ");
      Serial1.print("interval : ");
      Serial1.println(readings.interval);
      Serial1.print("avgInterval : ");
      Serial1.println(readings.avgInterval);
      Serial1.print("cpm : ");
      Serial1.println(readings.cpm);
    }
    else
    {
      Serial1.write((uint8_t*)&readings, 64);
      if(Serial1.available() > 0)
      { 
        Serial1.readBytes((uint8_t*)&settings, 64);
        lastPulse = 0;
        now = 0;
        readings.interval = 0;
      }
    }
    blueLED = !blueLED;
    digitalWrite(blueLEDPin, blueLED);
    digitalWrite(geigerLedPin, !blueLED);
    digitalWrite(led13Pin, blueLED);
    
  }  
}
