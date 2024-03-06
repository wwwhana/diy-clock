#include <TimeLib.h>
#include "SevSeg.h"
SevSeg sevseg;

String display = "NREADY__";
String mode = "CLOCK";
int decPlaces = 0;
uint8_t segs[8] = {};

unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 20;          // interval at which to blink (milliseconds)
int bright = 40;


void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  byte numDigits = 8;
  byte digitPins[] = { 15, 14, 16, 10, 21, 20, 19, 18 };
  byte segmentPins[] = { 3, 2, 8, 9, 7, 5, 6, 4 };



  bool resistorsOnSegments = true;
  byte hardwareConfig = COMMON_ANODE;

  bool updateWithDelays = true;  // Default 'false' is Recommended
  bool leadingZeros = false;     // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false;  // Use 'true' if your decimal point doesn't exist or isn't connected. Then, you only need to specify 7 segmentPins[]


  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
               updateWithDelays, leadingZeros, disableDecPoint);

  sevseg.setBrightness(bright);

  sevseg.setChars(display.c_str());
  sevseg.refreshDisplay();
}

void loop() {


  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (mode == "BLINK") {
      display = "";
      decPlaces = 0;
      sevseg.blank();
    } else if (mode == "STR") {

      Serial.println(display);

      sevseg.setChars(display.c_str());
      sevseg.getSegments(segs);

      setDecPoint(segs, decPlaces);


      sevseg.setSegments(segs);
    } else {
      display = "NREADY__";
      decPlaces = 0;

      char buffer[8];
      time_t t = now();

      int seconds = second(t);

      sprintf(buffer, "%02d%02d%02d%02d", hour(t), minute(t), month(t), day(t));
      sevseg.setChars(buffer);
      sevseg.getSegments(segs);

      int flag = 2 | ((seconds % 2 == 0) ? 32 : 0 );

      setDecPoint(segs, flag);

      sevseg.setSegments(segs);
    }

    sevseg.refreshDisplay();
  }
}

void setDecPoint(uint8_t segs[8], int flag) {
  if (flag < 0 || flag > 255) return;

  for (byte i = 0; i < 8 && flag > 0; i++) {
    byte state = bitRead(flag, i);
    if (state) {
      segs[7 - i] = segs[7 - i] | 0b10000000;
    }
  }
}

void serialEvent1() {
  if (Serial1.available() > 0) {
    String mode = Serial1.readStringUntil('|');
    Serial.println(mode);
    // Serial1.print(mode);

    if (mode == "SETTIME") {
      setTime();
    } else if (mode == "SETBR") {
      setBright();
    } else if (mode == "SETMODE") {
      setMode();
    } else if (mode == "SETSTR") {
      setSTR();
    } else if (mode == "GETTIME") {
      getTime();
    } else {
    }
  }
}

void getTime() {
  char buffer[20];
  time_t t = now();

  sprintf(buffer, "%04d.%02d.%02d %02d:%02d:%02d", year(t), month(t), day(t), hour(t), minute(t), second(t));

  if (Serial1.availableForWrite() > 0) {
    Serial1.println(buffer);
  }

  Serial.println(buffer);
}


void setTime() {
  int year = Serial1.readStringUntil('.').toInt();
  int month = Serial1.readStringUntil('.').toInt();
  int day = Serial1.readStringUntil(' ').toInt();
  int hour = Serial1.readStringUntil(':').toInt();
  int min = Serial1.readStringUntil(':').toInt();
  int sec = Serial1.readStringUntil('\n').toInt();

  char buffer[30];
  // sprintf(buffer, "%4d%2d%2d %2d:%2d:%2d", year, month, day, hour, min, sec);
  // Serial.println(buffer);

  setTime(hour, min, sec, day, month, year);
}

void setMode() {
  String tmp = Serial1.readStringUntil('\n');
  tmp.toUpperCase();

  mode = tmp;
}

void setBright() {
  int tmp_bright = Serial1.readStringUntil('\n').toInt();

  if (tmp_bright < -200) {
    tmp_bright = -200;
  } else if (tmp_bright > 200) {
    tmp_bright = 200;
  }

  bright = tmp_bright;

  sevseg.setBrightness(bright);
}


void setSTR() {
  String tmp_str = Serial1.readStringUntil('|');
  int tmp_decPlaces = Serial1.readStringUntil('\n').toInt();
  display = tmp_str;
  decPlaces = tmp_decPlaces;
}
