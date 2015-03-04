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
#define B1P          A3
#define B2P          A4
#define BATTERY      A5
#define JACK_SENSE   A7

#define B1P_COEFF     203.518 // ADC counts per volt
#define B2P_COEFF     122.460
#define BATTERY_COEFF 81.936
#define BATT_EMPTY   2.95 // how many volts to DIE at

int offCount = 0;  // counts how many off requests we've seen
#define OFF_THRESH 5 // how many to make us turn off
#define DELAYFACTOR 64 // millis() and delay() this many times faster  [fake/real]
float batt1,batt2,batt3; // voltage of battery cells

void loop() {
  // TODO:  do we want a full PID system here?

  // what we should see with pwm_for_max_brightness perfectly calibrated to ideal_max_current
  float ideal_instantaneous_current = ideal_max_current * SOME_FACTOR * pattern_brightness*pattern_brightness;

  if (senseRead < ideal_instantaneous_current) {
    digitalWrite(LED_PIN,HIGH);  // LED ON
    actual pwm_for_max_brightness += jumpVal;
    if (pwm_for_max_brightness > MAX_PWM) pwm_for_max_brightness = MAX_PWM;
  }
  if (senseRead > ideal_instantaneous_current) {
    digitalWrite(LED_PIN,LOW);  // LED OFF
    pwm_for_max_brightness -= jumpVal;
    if (pwm_for_max_brightness < 1) pwm_for_max_brightness = 1;
  }

  getBattVoltages();
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

void getBattVoltages() {
  batt1 = averageRead(B1P) / B1P_COEFF;
  batt2 = (averageRead(B2P) / B2P_COEFF) - batt1;
  batt3 = (averageRead(BATTERY) / BATTERY_COEFF) - batt2 - batt1;
}

#endif
