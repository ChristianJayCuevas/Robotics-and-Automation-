#include <SoftwareSerial.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;
const byte RATE_SIZE = 10; // Increase this for more averaging. 4 is a good starting point.
byte rates[RATE_SIZE]; // Array to store heart rate readings for averaging
byte rateSpot = 0; // Index for inserting the next heart rate reading into the array
long lastBeat = 0;
float beatsPerMinute = 0; // Initial value set to 0
int beatAvg;
String str;
int h = 0;
int t = 0;
int fingerDetected = 0;

unsigned long lastPrintTime = 0;
const unsigned long printInterval = 1500;
void setup(){
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.swap();
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) { // Start communication using fast I2C speed
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1); // Infinite loop to halt further execution if sensor is not found
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");
  particleSensor.setup(); // Configure sensor with default settings for heart rate monitoring
  particleSensor.setPulseAmplitudeRed(0x0A); // Set the red LED pulse amplitude (intensity) to a low value as an indicator
  particleSensor.setPulseAmplitudeGreen(0);
}
void loop()
{
  long irValue = particleSensor.getIR();
  if (irValue < 50000) {
    fingerDetected = 0;
    beatsPerMinute = 0; 
    beatAvg = 0;
  } else {
    fingerDetected = 1;
    if (checkForBeat(irValue) == true) { 
      long delta = millis() - lastBeat; 
      lastBeat = millis();
      beatsPerMinute = 60 / (delta / 1000.0);
      if (beatsPerMinute < 255 && beatsPerMinute > 20) {
        rates[rateSpot++] = (byte)beatsPerMinute; 
        rateSpot %= RATE_SIZE; 
        beatAvg = 0;
        for (byte x = 0; x < RATE_SIZE; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }
  }
  unsigned long currentMillis = millis();
  if (currentMillis - lastPrintTime >= printInterval) {
    Serial.print(fingerDetected); 
    Serial.print("A");
    delay(10);
    Serial.print(beatAvg);
    Serial.print("B");
    Serial.print("\n");
    lastPrintTime = currentMillis;
  }
}