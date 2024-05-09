#include <SoftwareSerial.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "MAX30105.h"
#include "heartRate.h"
#define BLYNK_TEMPLATE_ID   "TMPL6iw-wOkpS"
#define BLYNK_TEMPLATE_NAME "HeartJack"
#define BLYNK_AUTH_TOKEN    "TKY1Ra9AhvvsrEVsvWqmcYv0rUdYPoLh"
#define BLYNK_PRINT Serial
char ssid[] = "TEAM_CUEVAS-1";
char pass[] = "25February1980CUEVAScezar";
SoftwareSerial EspSerial(2, 3);
ESP8266 wifi(&EspSerial);
LiquidCrystal_I2C lcd(0x27, 16, 2);
MAX30105 particleSensor;
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute = 0;
int beatAvg;
int time = 0;
int option = 0;
int powerD = 0;
int heartRate;
int FingerDetection;
int automate = 0;

const int optionButtonPin = 8; // Connect the option button to digital pin 8
const int powerDButtonPin = 9; // Connect the powerD button to digital pin 9

// Variables to store previous button states
int prevOptionButtonState = HIGH; // Assume the buttons are not pressed initially
int prevPowerDButtonState = HIGH;

unsigned long lastPrintTime = 0;
const unsigned long printInterval = 10000;

char c;
String dataIn;
int8_t indexofA, indexofB;

String data1,data2;

#define ESP8266_BAUD 9600

void setup() {
  Serial.begin(115200);
  EspSerial.begin(ESP8266_BAUD);
  delay(10);
  lcd.init();
  lcd.backlight();
  lcd.print("Initializing...");
  delay(3000);
  lcd.clear();
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  pinMode(optionButtonPin, INPUT_PULLUP); // Set option button pin as input with internal pull-up resistor
  pinMode(powerDButtonPin, INPUT_PULLUP); 
  Blynk.begin(BLYNK_AUTH_TOKEN, wifi, ssid, pass);
}

void loop() {
  Blynk.run();
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.virtualWrite(V3, heartRate);
  Blynk.virtualWrite(V4, FingerDetection);
  Blynk.syncVirtual(V0);
  


  while(Serial.available()>0)
  {
    c = Serial.read();
    Serial.println("Data1 = " + data1);
    Serial.println("Data2 = " + data2);
    heartRate = data2.toInt();
    FingerDetection = data1.toInt();
  
    if(c=='\n')
    {
      break;
    }
    else
    {
      dataIn+=c;
    }
  }
  if(c=='\n')
  {
    Parse_the_Data();
    c=0;
    dataIn="";
  }

  Serial.print("Avg BPM = ");
  Serial.print(heartRate);
  Serial.println();
  lcd.setCursor(0, 0);

  if(FingerDetection == 0)
  {
    lcd.print("No Finger      ");
  }
  else
  {
    lcd.print("Finger Detected    ");
  }

  lcd.setCursor(0, 1);
  lcd.print("BPM: ");
  lcd.print(heartRate);
  lcd.print("   ");
  updateLCD();

  if (option == 0 && powerD == 1 && automate == 1)
  {
    static unsigned long beatTimer = 0;
    unsigned long beatInterval = 5000;
    unsigned long currentMillis = millis();
    if (currentMillis - beatTimer >= beatInterval)
    {
      beatTimer = currentMillis;
      if(FingerDetection == 1 && heartRate < 10)
      {
        Heart30_2();
      }
      else
      {
        killDev();
      }
    }
  }

  if (option == 1 && powerD == 1 && automate == 1)
  {
    static unsigned long beatTimer1 = 0;
    unsigned long beatInterval1 = 5000;
    unsigned long currentMillis1 = millis();
    if (currentMillis1 - beatTimer1 >= beatInterval1)
    {
      beatTimer1 = currentMillis1;
      if(FingerDetection == 1 && heartRate < 10)
      {
        Heart15_2();
      }
      else
      {
        killDev();
      }
    }
  }

  if (option == 0 && powerD == 1 && automate == 0) {
    Heart30_2();
  }
  if (option == 1 && powerD == 1 && automate == 0) {
    Heart15_2();
  }
  if (powerD == 0) {
    killDev();
  }
}
void updateLCD() {
  static unsigned long lcdTimer = 0;
  unsigned long lcdInterval = 500;
  
  if (millis() - lcdTimer >= lcdInterval) {
    lcdTimer = millis();
    lcd.setCursor(8, 2);
    lcd.print("       ");
    lcd.setCursor(8, 2);
    lcd.print(millis());
  }
}
void Heart30_2() {
  analogWrite(6, 20);
  analogWrite(7, 0);
  time += 1000;
  if (time >= 30000) {
    time = 0;
    analogWrite(6, 0);
    analogWrite(7, 0);
    delay(2000);
  }
}
void Heart15_2() {
  analogWrite(6, 20);
  analogWrite(7, 0);
  time += 1000;
  if (time >= 15000) {
    time = 0;
    analogWrite(6, 0);
    analogWrite(7, 0);
    delay(2000);
  }
}
void killDev() {
  analogWrite(6, 0);
  analogWrite(7, 0);
}
bool isNumeric(String str) {
  for (unsigned int i = 0; i < str.length(); i++) {
    if (!isdigit(str.charAt(i))) {
      return false;
    }
  }
  return true;
}
void Parse_the_Data()
{
  indexofA = dataIn.indexOf("A");
  indexofB = dataIn.indexOf("B");
 
  data1 = dataIn.substring(0, indexofA);
  data2 = dataIn.substring(indexofA+1, indexofB); 
}

BLYNK_WRITE(V1)
{   
  powerD = param.asInt(); // Get value as integer
}
BLYNK_WRITE(V2)
{   
  option = param.asInt(); // Get value as integer
}

BLYNK_WRITE(V0)
{   
  automate = param.asInt(); // Get value as integer
}

