/* dlg3 program */

// outputs
#define ONFET      4  // 4 is PD4
#define CCFL_PIN   5  // 10 originally, 5 now
#define CHARGE123  9 // PB1/9 prototype
#define DRAIN1     11 // PB3/11 prototype
#define DRAIN2     3 // PD3/3
#define DRAIN3     10 // PB2/10 prototype
#define BOOST      6 // PD6/6 prototype
#define LED        7  // 7 is PD7

// inputs
#define B1P        A3  // PC3/ADC3
#define B2P        A4  // PC4/ADC4
#define BATTERY    A5  // PC5/ADC5
#define BUTTON_SENSE 2 // PD2/INT0
#define JACK_SENSE 21  // A7 = 21
#define CCFL_SENSE 20  // A6 = 20
#define B1THERM    A0  // PC0/A0
#define B2THERM    A1  // PC1/A1
#define B3THERM    A2  // PC2/A2

// consts
#define BATT_FULL  4.18 // volts at which charging stops
#define BATT_LOW 3.3  // volts below which battery is "low"
#define BATT_EMPTY 2.9 // volts at which using battery stops
#define MAX_PWM 250  // maximum PWM value for CCFL activation
#define DISCHARGE_PWM 254  // what analogWrite value for drain1,2,3 on
#define MAX_BRIGHTNESS  500 // adc value of full CCFL power
#define MIN_BRIGHTNESS  105 // adc value of minimum CCFL power
#define BRIGHTNESS_RANGE MAX_BRIGHTNESS - MIN_BRIGHTNESS
#define B1P_DIV         186 // divide adc value by this to get volts at B1P
#define B2P_DIV         121.79 // divisor for voltage at node B2P
#define BATTERY_DIV     81.48  // divisor for voltage at node BATTERY
#define JACK_PWR_DIV    33.49  // divisor for voltage at node JACK_PWR
#define THERM_DIV       1  // divide adc value by this to get Degrees F
#define TEMPERATURE_MAX 90  // max temperature in Degrees F
#define TEMPERATURE_DANGER 105  // max temperature in Degrees F
#define BUTTON_TIME     200  // minimum time since last button release
#define TURNOFF_TIME    700  // time to hold down button to turn off
#define DARKCHARGE_VOLTAGE 5.0 // minimum voltage to turn on and darkcharge
#define MODE_COUNT 3  // 0 when off, 1 is full-on, 2 and 3 are modes, then OFF
// vars
int mode = 0;  // we start off with "off" as our mode
int brightCentage; // 0 to 100, like percentage of full available brightness
int x = 0;  // general purpose integer
boolean charging = false;  // if we are charging this is true
boolean darkCharging = false;  // if we only turned on because a charger plugd in
boolean batteryFull = false;  // true if ALL of the batteries are >= BATT_FULL
boolean batteryDead = false;  // true if one of the batteries is below BATT_EMPTY
boolean batteryLow = false;  // true if one of the batteries is below BATT_LOW
boolean batteryHot = false;  // true if one of the batteries is > TEMPERATURE_MAX
unsigned long timeNow,lastWheelCharge, lastButtonUp, lastButtonDown = 0;  //
float cell1,cell2,cell3;
float cellTemp1,cellTemp2,cellTemp3;
float jackVoltage;  // voltage at jack according to getJackActivity

void setup() {
  pinMode(ONFET,OUTPUT);
  digitalWrite(ONFET,HIGH);  // brain wakes up, turns on ONFET to stay on

  pinMode(BOOST, OUTPUT);
  pinMode(CHARGE123, OUTPUT);
  pinMode(DRAIN3, OUTPUT);
  pinMode(DRAIN2, OUTPUT);
  pinMode(DRAIN1, OUTPUT);
  pinMode(CCFL, OUTPUT);
  pinMode(LED, OUTPUT);

  getJackActivity(); // fine out voltage at charging jack
  if (jackVoltage > DARKCHARGE_VOLTAGE) {
    darkCharging = true;  // charge without turning on the light
  } else {
    mode = 1;  // since we were turned on by a finger, light up
  }
  getCellVoltages(); // sets batteryDead status
  getCellTemps();  // sets batteryHot status

  attachInterrupt(0,intHandler,CHANGE); // watch the last time the button state changed
}

void loop () {
  timeNow = millis();
  getJackActivity();
  getCellVoltages(); // sets batteryDead status
  getCellTemps();  // sets batteryHot status
  updateBrightness();  // sets ccfl pwm level
  updateCharging();  // update charging pwm levels based on cell Volts and Temps
  // uhhh.. R17 and R28 need to be bigger to fit in the ADC
  // you could swap R17 and R24, that would work, and
  // change R28 to another 1Mohm for 4.2v maximum read (at 12.6v)
  // what were you thinking?
  // consider changing Q9 to an NFET, it might work better or not,
  // but NFET or PFET are equally capable of doing the job if driven right
  // check gate of Q13 for missing junction dot

  if ((timeNow - lastButtonDown > TURNOFF_TIME) && (lastButtonDown > lastButtonUp)) {
    turnOff(); //  customer is trying to turn light off. (watch out for overflow!?
  }

  if (batteryDead) turnOff();
  if (batteryLow) {
     mode = 9; //Emergency / Get you home mode.
  }
}

void setBrightness () {

  unsigned long senseRead = 0;
  for (int i = 0; i < 50; i++) senseRead += analogRead(CCFL_SENSE);
  senseRead /= 50;
  if (senseRead < brightness) {
    digitalWrite(LED_PIN,HIGH);  // LED ON
    pwmVal += jumpVal;
    if (pwmVal > MAX_PWM) pwmVal = MAX_PWM;
    analogWrite(CCFL_PIN,pwmVal);
  }
  if (senseRead > brightness) {
    digitalWrite(LED_PIN,LOW);  // LED OFF
    pwmVal -= jumpVal;
    if (pwmVal < 1) pwmVal = 1;
    analogWrite(CCFL_PIN,pwmVal);
  }

  Serial.print(pwmVal,HEX);
  Serial.print("    ");
  Serial.println(senseRead);
}

void updateCharging () {
 //check the jack voltage to see if we're in danger of running USB power too low.
 // balanceCells(); // DANGER, DISCHARGING IS POSSIBLE IN THIS SUBROUTINE

}

void balanceCells () {
  if (charging) {
    // if cell1 or cell2 is higher than cell3, discharge same
    if (cell1 > cell3)
      analogWrite(DRAIN1,DISCHARGE_PWM);
    if (cell2 > cell3)
      analogWrite(DRAIN2,DISCHARGE_PWM);

    // If only cell3 is full, charge using C until cell1 or cell2 full
    if (cell3 > BATT_FULL && cell1 < BATT_FULL && cell2 < BATT_FULL)
      // charge(C); // until cell1 or cell2 full

    // if cell1 is higher than cell2, discharge cell1
    if (cell1 > cell2)
      analogWrite(DRAIN1,DISCHARGE_PWM);
    // some more logic goes here, but needs to be thought out fully
  }
}

void getCellVoltages()  {
  cell1 = analogRead(B1P) / B1P_DIV;
  cell2 = (analogRead(B2P) / B2P_DIV) - cell1;
  cell3 = (analogRead(BATTERY) / BATTERY_DIV) - cell1 - cell2;
  batteryDead = ((cell1 <= BATT_EMPTY) || (cell2 <= BATT_EMPTY) || (cell3 <= BATT_EMPTY));
// healthy is the opposite of batteryLow  batteryHealthy = ((cell1 >=  BATT_LOW && (cell2 >= BATT_LOW ) && (cell3 >= BATT_LOW ));
  batteryLow = ((cell1 <  BATT_LOW) || (cell2 < BATT_LOW) || (cell3 < BATT_LOW ));
  batteryFull = ((cell1 >= BATT_FULL) && (cell2 >= BATT_FULL) && (cell3 >= BATT_FULL ));
}

void getCellTemps()  {
  // Includes emergency temp-related shutdown.
  cellTemp1 = analogRead(B1THERM) / THERM_DIV;
  cellTemp2 = analogRead(B2THERM) / THERM_DIV;
  cellTemp3 = analogRead(B3THERM) / THERM_DIV;
  batteryHot = ((cellTemp1 >= TEMPERATURE_MAX) || (cellTemp2 >= TEMPERATURE_MAX) || (cellTemp3 >= TEMPERATURE_MAX)) ;
  if ((cellTemp1 >= TEMPERATURE_DANGER) || (cellTemp2 >= TEMPERATURE_DANGER) || (cellTemp3 >= TEMPERATURE_DANGER)) {
    digitalWrite (ONFET, LOW);
  }
}

void updateBrightness () {
  switch (mode) {
    case 0:  // Charging but light not ON.
      brightCentage=0; //set brightness to nothing
      break;
    case 1:  //Customer wants light to be on.
      brightCentage=100; //set brightness to 100%
      break;
    case 2:  //Customer wants light to pulse.
       x = timeNow % 500;  // total time of one cycle = 500 ms
       if (x < 200) brightCentage = 100;  // be on full brightness for first 200 ms
       else if (x < 301) brightCentage = 300 - x; // fade to 0 over next 100 ms
       else brightCentage = 0; // be at 0 for last 200 ms
       break;
    case 3:
       x = timeNow % (100 + 100 + 100 + 50 + 400);  //Lub-Dup Pulse pseudocode
       if (x < 100) brightCentage = 100;  //  Be on fullbrightness for first 100 ms
       else if (x < 201) brightCentage = ((200 - x)*0.7)+30; //fade to 30 over next 100 ms
       else if (x < 301) brightCentage = 100;  //Then be on fullbright again for another 100ms
       else if (x < 351) brightCentage = (350 - x)*2;  //Then fade to zero over next 50ms
       else brightCentage = 0;  //Then be at 0 for next 400ms
       break;
    case 9: // battery is low, get-you-home mode
      brightCentage =  (timeNow % 1500 < 500) * 100;
      // the above should be 100% on for 500mS out of every 1500mS
      break;
  }  // switch (mode)
}

void intHandler() {
  // if INT0 is triggered, switch to next mode, which might be "off"
  timeNow = millis();
  if (digitalRead(BUTTON_SENSE)) { // button just got pressed
    if ((timeNow - lastButtonUp) > BUTTON_TIME) {  // it was not a bounce
      mode = (mode+1) % MODE_COUNT; // go to next mode
      // if (mode == 0) mode = 1;  // this line disables turn-off except by holddown
      lastButtonDown = timeNow;
    }
  } else { // button was released
    lastButtonUp = timeNow;// count from release of button
  }
}

void getJackActivity() {
  jackVoltage = analogRead(JACK_SENSE) / JACK_PWR_DIV;  // measure jack voltage
}

void turnOff() {
  digitalWrite(ONFET, LOW);
}
