#include <Adafruit_AS7341.h>

Adafruit_AS7341 as7341;

// LED Pins
const int led1 = 1;
const int led2 = 2;
const int led3 = 3;
const int buttonPin = 4;

// Timing variables
unsigned long stageStartMicros;
unsigned long lastMeasurementMicros;
const unsigned long measurementIntervalMicros = 100000; // 100ms

int ledStage = 0;
int measurementCount = 0;
bool haltMeasurement = false;
bool buttonLastState = HIGH;
bool resetAllowed = true; // Only allow reset when true

void setup() {
  Serial.begin(115200);
  delay(1000);

  if (!as7341.begin()) {
    Serial.println("AS7341 not found");
    while (1);
  }

  as7341.setATIME(100);
  as7341.setASTEP(100);
  as7341.setGain(AS7341_GAIN_256X);
  as7341.setLEDCurrent(20);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);

  Serial.println("F1_415,F2_445,F3_480,F4_515,F5_555,F6_590,F7_630,F8_680,CLEAR,NIR,LABEL");

  resetMeasurement();
}

void loop() {
  checkSerialCommand();
  checkButton();

  if (haltMeasurement) return;

  handleLEDStage();
}

void handleLEDStage() {
  // Set LEDs according to stage
  switch (ledStage) {
    case 0: // white
      as7341.enableLED(true);
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      break;
    case 1: // red
      as7341.enableLED(false);
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      break;
    case 2: // ir
      as7341.enableLED(false);
      digitalWrite(led1, LOW);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, LOW);
      break;
    case 3: // green
      as7341.enableLED(false);
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, HIGH);
      break;
    case 4: // none
      as7341.enableLED(false);
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      break;
    default:
      break;
  }

  // Take measurements at 100ms intervals
  unsigned long currentMicros = micros();
  if ((long)(currentMicros - lastMeasurementMicros) >= (long)measurementIntervalMicros) {
    lastMeasurementMicros += measurementIntervalMicros;
    readSensorAndPrint();
    measurementCount++;

    if (measurementCount >= 10) { // After 10 measurements, move to next stage
      ledStage++;
      measurementCount = 0;
      delay(10); // Let LEDs settle
      lastMeasurementMicros = micros();
    }
    if (ledStage > 4) {
      haltMeasurement = true;
      resetAllowed = true; // Allow reset after cycle completes
      Serial.println("Measurement halted. Press button or send 'r' to restart.");
    }
  }
}

void readSensorAndPrint() {
  uint16_t readings[12];
  if (as7341.readAllChannels(readings)) {
    Serial.print(readings[0]); Serial.print(",");
    Serial.print(readings[1]); Serial.print(",");
    Serial.print(readings[2]); Serial.print(",");
    Serial.print(readings[3]); Serial.print(",");
    Serial.print(readings[6]); Serial.print(",");
    Serial.print(readings[7]); Serial.print(",");
    Serial.print(readings[8]); Serial.print(",");
    Serial.print(readings[9]); Serial.print(",");
    Serial.print(readings[10]); Serial.print(",");
    Serial.print(readings[11]); Serial.print(",");

    switch (ledStage) {
      case 0: Serial.println("white"); break;
      case 1: Serial.println("red"); break;
      case 2: Serial.println("ir"); break;
      case 3: Serial.println("green"); break;
      case 4: Serial.println("none"); break;
      default: Serial.println("none"); break;
    }
  } else {
    Serial.println("0,0,0,0,0,0,0,0,0,0,error");
  }
}

void checkSerialCommand() {
  if (Serial.available()) {
    char cmd = Serial.read();
    if ((cmd == 'r' || cmd == 'R') && resetAllowed) {
      // Serial.println("");
      resetAllowed = false; // Disable reset until cycle completes
      resetMeasurement();
    }
  }
}

void checkButton() {
  bool buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && buttonLastState == HIGH && resetAllowed) {
    delay(50); // basic debounce
    if (digitalRead(buttonPin) == LOW) {
      // Serial.println("Measurement reset by button");
      // Serial.println("");
      resetAllowed = false; // Disable reset until cycle completes
      resetMeasurement();
    }
  }
  buttonLastState = buttonState;
}

void resetMeasurement() {
  stageStartMicros = micros();
  lastMeasurementMicros = micros();
  ledStage = 0;
  measurementCount = 0;
  haltMeasurement = false;
}
