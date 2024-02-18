#include "SevSeg.h"
SevSeg sevseg;

String display = "NREADY__";
int decPlaces = 0;
uint8_t segs[8] = {};

void setup() {
  Serial.begin(19200);
  Serial.setTimeout(5000);

  byte numDigits = 8;
  byte digitPins[] = { 5, 4, 3, 2, 9, 8, 7, 6 };
  byte segmentPins[] = { 20, 21, 16, 10, 14, 18, 15, 19 };

  bool resistorsOnSegments = true;
  byte hardwareConfig = COMMON_ANODE;

  bool updateWithDelays = false;  // Default 'false' is Recommended
  bool leadingZeros = false;      // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false;   // Use 'true' if your decimal point doesn't exist or isn't connected. Then, you only need to specify 7 segmentPins[]


  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
               updateWithDelays, leadingZeros, disableDecPoint);
}

void loop() {

  if (Serial.available() > 0) {
    display = Serial.readStringUntil('|');
    decPlaces = Serial.readStringUntil('\n').toInt();
  } 



  sevseg.setChars(display.c_str());
  sevseg.getSegments(segs);

  for (byte i = 0; i < 8 && decPlaces > 0; i++) {
    byte state = bitRead(decPlaces, i);
    if (state) {
      segs[7 - i] = segs[7 - i] |= 0b10000000;
    }
  }

  sevseg.setSegments(segs);
  sevseg.refreshDisplay();
}

