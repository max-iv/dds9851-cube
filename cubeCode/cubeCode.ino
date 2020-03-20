#define BAUD_RATE 115200

struct TransmitData
{
  int ledState = 0;
};
struct ReceiveData
{
  int ledOn = 1;
  int ledToggle = 1;
  int loopDelay = 2000;
};

const int ledPin = 13;

void setupPins()
{
  pinMode(ledPin, OUTPUT);    
  digitalWrite(ledPin, LOW);
}
void processNewSetting(TransmitData* tData, ReceiveData* rData, ReceiveData* newData)
{
  rData->ledOn = newData->ledOn;
  rData->ledToggle = newData->ledToggle;
  rData->loopDelay = newData->loopDelay;
 }
boolean processData(TransmitData* tData, ReceiveData* rData)
{
  if(rData->ledToggle == 1)
  {
    tData->ledState = tData->ledState + 1; 
    if (tData->ledState > 1) tData->ledState = 0;
  }
  else
  {
    tData->ledState = rData->ledOn;
  }
  digitalWrite(ledPin,  tData->ledState);
  delay(rData->loopDelay);
  return true;
}

const int microLEDPin = 13;
const int commLEDPin = 2;
boolean commLED = true;

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
//  digitalWrite(microLEDPin, commLED);

  sizeOfTx = sizeof(tx);
  sizeOfRx = sizeof(rx);
  Serial1.begin(BAUD_RATE);
  delay(1000);
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
