#include "SevSeg.h"
SevSeg sevseg;

String display = "NREADY__";
int decPlaces = 0;
uint8_t segs[8] = {};
bool debug = false;

unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 20;          // interval at which to blink (milliseconds)


void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  byte numDigits = 8;
  byte digitPins[] = { 15, 14, 16, 10, 21, 20, 19, 18 };
  byte segmentPins[] = { 3, 2, 8, 9, 7, 5, 6, 4 };

  bool resistorsOnSegments = true;
  byte hardwareConfig = NP_COMMON_CATHODE;

  bool updateWithDelays = true;  // Default 'false' is Recommended
  bool leadingZeros = false;     // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false;  // Use 'true' if your decimal point doesn't exist or isn't connected. Then, you only need to specify 7 segmentPins[]


  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
               updateWithDelays, leadingZeros, disableDecPoint);

  sevseg.setBrightness(40);

  sevseg.setChars(display.c_str());
  sevseg.refreshDisplay();
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (debug && Serial.availableForWrite()) {
      Serial.println(previousMillis);
    }

    sevseg.refreshDisplay();
  }
}

void serialEvent1() {
  if (Serial1.available() > 0) {
    String display_recv = Serial1.readStringUntil('|');
    String decPlaces_recv_str = Serial1.readStringUntil('\n');
    int decPlaces_recv = decPlaces_recv_str.toInt();


    if (display_recv == "[BR]") {

      sevseg.setBrightness(decPlaces_recv);

      return;
    } else if (display_recv == "[BLINK]") {
      sevseg.blank();
      display = "";
      decPlaces = 0;

      return;
    // } else if (display_recv == "[INTER]") {
    //   interval = decPlaces_recv;

    //   return;
    } else if (display_recv == "[DEBUG]") {
      debug = !debug;

      return;
    }

    if (debug) {
      if (Serial.availableForWrite()) {

        Serial.println(display_recv.c_str());
        Serial.println(decPlaces_recv_str.c_str());
      } else {
        debug = false;
      }
    }


    display = display_recv;
    sevseg.setChars(display.c_str());

    decPlaces = decPlaces_recv;
    sevseg.getSegments(segs);
    for (byte i = 0; i < 8 && decPlaces > 0; i++) {
      byte state = bitRead(decPlaces, i);
      if (state) {
        segs[7 - i] = segs[7 - i] |= 0b10000000;
      }
    }

    sevseg.setSegments(segs);
  }
}
