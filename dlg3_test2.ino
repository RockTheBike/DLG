// this file tracks the features we have not yet ported from dlg3_test2.ino to the nice state machine

// but we don't want to compile it
#if 0

#define CHARGE123    9  // PB1/9 prototype
#define DRAIN1       11 // PB3/11 prototype
#define DRAIN2       3  // PD3/3
#define DRAIN3       10 // PB2/10 prototype
#define BOOST        6  // PD6/6 prototype
#define B1THERM      A0
#define B2THERM      A1
#define B3THERM      A2
#define JACK_SENSE   A7


int offCount = 0;  // counts how many off requests we've seen
#define OFF_THRESH 5 // how many to make us turn off
#define DELAYFACTOR 64 // millis() and delay() this many times faster  [fake/real]

void loop() {

  printAnalogs();

  if (batt1 < BATT_EMPTY) die("batt1 dead! ");
  if (batt2 < BATT_EMPTY) die("batt2 dead! ");
  if (batt3 < BATT_EMPTY) die("batt3 dead! ");

  delay(25*DELAYFACTOR); // actual time = n * DELAYFACTOR milliseconds

  if (digitalRead(BUTTON_SENSE)) {
    Serial.print("+");
    offCount++;
  } else {
    offCount = 0; // if (--offCount < 0) offCount = 0;
  }

  if (offCount > OFF_THRESH) die("offCount > OFF_THRESH");
}

float averageRead(int pin) {
  #define READ_MULTITUDE 50
  float analogAdder = 0;
  for (int i = 0; i < READ_MULTITUDE; i++) analogAdder += analogRead(pin);
  analogAdder /= READ_MULTITUDE;
  return analogAdder;
}

#endif
