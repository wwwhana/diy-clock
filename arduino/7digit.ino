#include <TimeLib.h>
#include "SevSeg.h"
SevSeg sevseg;

String display = "NREADY__";
String mode = "DETAILCLOCK";
String latestClockMode = "DETAILCLOCK";
time_t timmer_start = now();
long timmer_duration = 0;  // second
int decPlaces = 0;
uint8_t segs[8] = {};

float humidity = 99.9f;
float temperature = -99.9f;

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
      time_t current = now();

      if (timmer_duration >= 0 && current - timmer_start >= timmer_duration) {
        mode = latestClockMode;
        timmer_duration = -1;
      }

      sevseg.setChars(display.c_str());
      sevseg.getSegments(segs);

      setDecPoint(segs, decPlaces);


      sevseg.setSegments(segs);
    } else if (mode == "CLOCK") {
      latestClockMode = "CLOCK";
      showSimpleCLock();
    } else {
      latestClockMode = mode;
      time_t t = now();
      int seconds = second(t);
      int flag = seconds % 15 / 5;

      switch (flag) {
        case 1:
          showDate();
          break;
        case 2:
          showHumidityCelsius();
          break;
        default:
          showDetailCLock();
      }
    }

    sevseg.refreshDisplay();
  }
}

void showSimpleCLock() {
  display = "NREADY__";
  decPlaces = 0;

  char buffer[8];
  time_t t = now();

  int seconds = second(t);

  sprintf(buffer, "%02d%02d%02d%02d", hour(t), minute(t), month(t), day(t));
  sevseg.setChars(buffer);
  sevseg.getSegments(segs);

  int flag = 2 | ((seconds % 2 == 0) ? 32 : 0);

  setDecPoint(segs, flag);

  sevseg.setSegments(segs);
}

void showDetailCLock() {
  display = "NREADY__";
  decPlaces = 0;

  char buffer[8];
  time_t t = now();

  int seconds = second(t);
  int hours = hour(t);

  sprintf(buffer, "%02d%02d%02d %c", hours % 12 > 0 ? hours % 12 : 12, minute(t), second(t), hours < 12 ? 'A' : 'P');
  sevseg.setChars(buffer);
  sevseg.getSegments(segs);

  int flag = 8 | ((seconds % 2 == 0) ? 40 : 0);

  setDecPoint(segs, flag);

  sevseg.setSegments(segs);
}

void showDate() {
  char buffer[8];
  time_t t = now();

  sprintf(buffer, "%04d%02d%02d", year(t), month(t), day(t));
  sevseg.setChars(buffer);
  sevseg.getSegments(segs);

  int flag = 10;

  setDecPoint(segs, flag);

  sevseg.setSegments(segs);
}

void showHumidityCelsius() {
  char buffer[8];
  char tempaBuffer[8];
  char humidityBuffer[8];
  int flag = 18;

  dtostrf(humidity, 4, 1, humidityBuffer);
  dtostrf(temperature, 4, 1, tempaBuffer);

  if (temperature > 0) {

    sprintf(buffer, "H%s%s ", humidityBuffer, tempaBuffer);
  } else {
    dtostrf(temperature * -1, 4, 1, tempaBuffer);
    sprintf(buffer, "H%s%s ", humidityBuffer, tempaBuffer);
    flag = 26;
  }

  sevseg.setChars(buffer);
  sevseg.getSegments(segs);

  segs[7] = 225;  //°C(upper)

  setDecPoint(segs, flag);
  sevseg.setSegments(segs);
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

void serialEventRun(void) {
  // if (Serial.available()) serialEvent();
  if (Serial1.available()) serial1Event();
}

void serial1Event() {
  if (Serial1.available() > 0) {
    String mode = Serial1.readStringUntil('|');
    mode.toUpperCase();

    Serial.println(mode);

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
    } else if (mode == "SETHUMIDITY") {
      setHumidity();
    } else if (mode == "SETTEMPERATURE") {
      setTemperature();
    } else {
    }
  }
}

void getTime() {

  char buffer[6];
  time_t t = now();

  sprintf(buffer, "%04d.", year(t));
  Serial1.write(buffer);
  sprintf(buffer, "%02d.", month(t));
  Serial1.write(buffer);
  sprintf(buffer, "%02d ", day(t));
  Serial1.write(buffer);

  sprintf(buffer, "%02d:", hour(t));
  Serial1.write(buffer);

  sprintf(buffer, "%02d:", minute(t));
  Serial1.write(buffer);

  sprintf(buffer, "%02d\n", second(t));
  Serial1.write(buffer);

  // Serial1.flush();
}


void setTime() {
  int year = Serial1.readStringUntil('.').toInt();
  int month = Serial1.readStringUntil('.').toInt();
  int day = Serial1.readStringUntil(' ').toInt();
  int hour = Serial1.readStringUntil(':').toInt();
  int min = Serial1.readStringUntil(':').toInt();
  int sec = Serial1.readStringUntil('\n').toInt();

  setTime(hour, min, sec, day, month, year);
}

void setMode() {
  String tmp = Serial1.readStringUntil('\n');
  tmp.toUpperCase();

  mode = tmp;
}

void setHumidity() {
  String tmp = Serial1.readStringUntil('\n');
  humidity = tmp.toFloat();
}

void setTemperature() {
  String tmp = Serial1.readStringUntil('\n');
  temperature = tmp.toFloat();
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
  int tmp_decPlaces = Serial1.readStringUntil('|').toInt();
  long tmp_timmer_duration = Serial1.readStringUntil('\n').toInt();
  display = tmp_str;
  decPlaces = tmp_decPlaces;
  timmer_duration = tmp_timmer_duration;
  timmer_start = now();
}
