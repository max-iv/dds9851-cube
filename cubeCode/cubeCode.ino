#define BAUD_RATE 115200

struct TransmitData
{
  int intervalTime = 0;
  float avgInterval = 0.0;
  float cpm = 0.0;
};
struct ReceiveData
{
  float nsamples = 1.0;
};

const int geigerPin = 3;
const int geigerLedPin = 5;
boolean geigerLED = false;
unsigned long lastPulse = 0;

void setupPins()
{
  pinMode(geigerLedPin, OUTPUT);    
  pinMode(geigerPin, INPUT);
  geigerLED = false;  
  digitalWrite(geigerLedPin, geigerLED);
}
void processNewSetting(TransmitData* tData, ReceiveData* rData, ReceiveData* newData)
{
  lastPulse = 0;
  tData->intervalTime = 0;
  rData->nsamples = newData->nsamples;
}
boolean processData(TransmitData* tData, ReceiveData* rData)
{
  unsigned long nowTime = 0;
  if (pulseIn(geigerPin, HIGH) < 200) return false;
  nowTime = millis();
  geigerLED = !geigerLED;
  digitalWrite(geigerLedPin, geigerLED);
  if (lastPulse > 0)
  {
    if (tData->intervalTime > 0)
    {
      tData->intervalTime = (int) (nowTime - lastPulse);
      tData->avgInterval = tData->avgInterval + (((float) tData->intervalTime) - tData->avgInterval) / rData->nsamples;
    }
    else
    {
      tData->intervalTime = (int) (nowTime - lastPulse);
      tData->avgInterval = (float) tData->intervalTime;
    }
    lastPulse = nowTime;
    tData->cpm = 60000.0 / tData->avgInterval;
  }
  lastPulse = nowTime;
  return true;
}

const int microLEDPin = 13;
const int commLEDPin = 2;
boolean commLED = false;

struct TXinfo
{
  int cubeInit = 1;
  int newSettingDone = 0;
};
struct RXinfo
{
  int newSetting = 0;
};

struct TX
{
  TXinfo txInfo;
  TransmitData txData;
};
struct RX
{
  RXinfo rxInfo;
  ReceiveData rxData;
};
TX tx;
RX rx;
ReceiveData settingsStorage;

int sizeOfTx = 0;
int sizeOfRx = 0;

void setup()
{
  setupPins();
  pinMode(microLEDPin, OUTPUT);    
  pinMode(commLEDPin, OUTPUT);  
  digitalWrite(commLEDPin, commLED);
  digitalWrite(microLEDPin, commLED);

  sizeOfTx = sizeof(tx);
  sizeOfRx = sizeof(rx);
  Serial1.begin(BAUD_RATE);
  delay(2000);
}
void loop()
{
  boolean goodData = false;
  goodData = processData(&(tx.txData), &settingsStorage);
  if (goodData)
  {
    tx.txInfo.newSettingDone = 0;
    if(Serial1.available() > 0)
    { 
      commLED = !commLED;
      digitalWrite(commLEDPin, commLED);
      Serial1.readBytes((uint8_t*)&rx, sizeOfRx);
      
      if (rx.rxInfo.newSetting > 0)
      {
        processNewSetting(&(tx.txData), &settingsStorage, &(rx.rxData));
        tx.txInfo.newSettingDone = 1;
        tx.txInfo.cubeInit = 0;
      }
    }
    Serial1.write((uint8_t*)&tx, sizeOfTx);
  }
  
}
