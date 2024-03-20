#include <Adafruit_AHTX0.h>
#include "ArduinoJson.h"
#include <SoftwareSerial.h>

#define rxPin 8
#define txPin 9

SoftwareSerial piSerial(rxPin, txPin);

unsigned long latestTime;
const unsigned long duration = 10;
unsigned long latestSegmentTime;
const unsigned long segmentDuration = 2000;

int brightness = 70;

const float ARef = 1.1;

Adafruit_AHTX0 aht;

int lightsensor = 5;

void setup() {


  // Arduino USB <-> DEBUG/UPDATE
  Serial.begin(19200);
  // Arduino TX/RX <-> Segment Arduino
  Serial1.begin(9600);

  // OrangePi <-> Arduino 20/21s
  piSerial.begin(9600);

  while (!aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    delay(10);
    continue;
  }

  pinMode(lightsensor, INPUT);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  latestTime = millis() - duration;
  latestSegmentTime = millis() - segmentDuration;
}

void loop() {
  const unsigned long current = millis();


  piSerialEvent();


  if (current - latestSegmentTime > segmentDuration) {
    latestSegmentTime = current;

    sendTemperatureHumidityBrightness();
  };

  if (current - latestTime > duration) {
    latestTime = current;

    DynamicJsonDocument doc = getSensorData();

    serializeJson(doc, Serial);
    serializeJson(doc, piSerial);
    Serial.println();
    piSerial.println();
  };
}

void serialEventRun(void) {
  if (Serial.available()) serialEvent();
}

void sendCommand(String cmd) {
  if (Serial1.availableForWrite()) {
    Serial1.println(cmd);
    Serial.println("sent:" + cmd);
  }
}

void piSerialEvent() {
  String mode = piSerial.readStringUntil('|');
  String args = piSerial.readStringUntil('\n');
  mode.toUpperCase();

  if (mode == "SETBR") {
    int tmp_bright = args.toInt();

    if (tmp_bright < -200) {
      tmp_bright = -200;
    } else if (tmp_bright > 200) {
      tmp_bright = 200;
    }
    brightness = tmp_bright;
  } else if (mode != "") {
    String cmd = mode + "|" + args;
    sendCommand(cmd);

  }
}

void serialEvent() {
  String cmd = Serial.readStringUntil('\n');

  sendCommand(cmd);

}

void sendTemperatureHumidityBrightness() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);  // populate temp and humidity objects with fresh data

  char buff[100];
  char tmp[6];
  char tmp2[6];

  if (Serial1.availableForWrite()) {
    dtostrf(humidity.relative_humidity, 4, 1, tmp);
    dtostrf(temp.temperature, 4, 1, tmp2);
    sprintf(buff, "SETHUMIDITY|%s", tmp);
    sendCommand(buff);
    delay(100);


    sprintf(buff, "SETTEMPERATURE|%s", tmp2);
    sendCommand(buff);

    delay(100);

    sprintf(buff, "SETBR|%d", brightness);
    sendCommand(buff);

  }
}

DynamicJsonDocument getSensorData() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);  // populate temp and humidity objects with fresh data

  DynamicJsonDocument doc(200);
  doc["temperature"] = temp.temperature;
  doc["temperature_unit"] = "°C";

  doc["humidity"] = humidity.relative_humidity;
  doc["humidity_unit"] = "%";

  doc["battery_voltage"] = batteryVolts();

  doc["shadow"] = darkness();

  return doc;
}

bool darkness() {
  if (digitalRead(lightsensor) == LOW)  //it is dark
  {
    return true;
  } else {
    return false;
  }
}

float batteryPercent() {
  const float volt = batteryVolts();

  return volt / 1 * 100;
}

float batteryVolts() {
  float raw = analogRead(A6);
  float x = raw / 1023;
  float voltage = x * ARef * 4;

  return voltage;
}
