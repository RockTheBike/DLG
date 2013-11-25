/* dlg3 program */

// outputs
#define ONFET      1  // FIX/CHECK THIS
#define CCFL      2  // FIX/CHECK THIS
#define CHARGE123  3  // FIX/CHECK THIS
#define DRAIN3   4  // FIX/CHECK THIS
#define DRAIN2    6  // FIX/CHECK THIS
#define DRAIN1     7  // FIX/CHECK THIS
#define BOOST      9  // FIX/CHECK THIS

// inputs
#define B1P     8  // FIX/CHECK THIS
#define B2P      9  // FIX/CHECK THIS
#define BATTERY      10  // FIX/CHECK THIS
#define BUTTON_SENSE 11  // FIX/CHECK THIS
#define JACK_SENSE 12  // FIX/CHECK THIS
#define CCFL_SENSE 13  // FIX/CHECK THIS
#define B1THERM    12  // FIX/CHECK THIS
#define B2THERM    12  // FIX/CHECK THIS
#define B3THERM    12  // FIX/CHECK THIS

// consts
#define FULL  4.18 // volts at which charging stops
#define EMPTY 2.9 // volts at which using battery stops 

#define MODE_COUNT 3  // 0 when off, 1 is on, 2 and 3 are modes, then OFF 

#define MODE_0_RATE 1000 // timer counts of a period of lightness
#define MODE_0_PWM  1000 // how many of those counts are "on"
#define MODE_1_RATE 1000 // mode 1 will perhaps be a fader mode

#define MODE_1_PWM   500
#define MODE_2_RATE  400
#define MODE_2_PWM   300

#define MAX_BRIGHTNESS  175 // adc value of full CCFL power
#define MIN_BRIGHTNESS  145 // adc value of minimum CCFL power#define DESIRED_PERCENTAGE_BRIGHTNESS 0 
#define BRIGHTNESS_RANGE MAX_BRIGHTNESS - MIN_BRIGHTNESS
#define B1P_DIV         186 // divide adc value by this to get volts at B1P
#define B2P_DIV         121.79 // divisor for voltage at node B2P
#define BATTERY_DIV     81.48  // divisor for voltage at node BATTERY
#define JACK_PWR_DIV    33.49  // divisor for voltage at node JACK_PWR
#define THERM_DIV       1  // divide adc value by this to get Degrees F
#define TEMPERATURE_MAX 100  // max temperature in Degrees F
#define BUTTON_TIME     200  // minimum time since last button release
#define TURNOFF_TIME    1000  // time to hold down button to turn off

// vars
int mode = 0;
int brightCentage; // 0 to 100, like percentage of full available brightness
boolean lampOn = false;
boolean charging = false;  // 
boolean batteryFull = false;  // true if ALL of the batteries are >= FULL
boolean batteryDead = false;  // true if one of the batteries is below EMPTY
boolean batteryLow = false;  // true if one of the batteries is below BATT_LOW
boolean batteryHot = false;  // true if one of the batteries is > TEMPERATURE_MAX
unsigned long timeNow,lastWheelCharge, lastButtonUp, lastButtonDown = 0;  // 
float cell1,cell2,cell3;
float cellTemp1,cellTemp2,cellTemp3;

void setup() {
  pinMode(ONFET,OUTPUT);
  digitalWrite(ONFET,HIGH);  // brain wakes up, turns on ONFET to stay on

  pinMode(BOOST, OUTPUT);
  pinMode(CHARGE123, OUTPUT);
  pinMode(DRAIN3, OUTPUT);
  pinMode(DRAIN2, OUTPUT);
  pinMode(DRAIN1, OUTPUT);
  pinMode(CCFL, OUTPUT);

  getCellVoltages(); // sets batteryDead status
  getCellTemps();  // sets batteryHot status

  attachInterrupt(0,intHandler,CHANGE); // watch the last time the button state changed

  lastWheelCharge = millis();
}

void loop () {
  timeNow = millis(); 
  getJackActivity(); 
  getCellVoltages(); // sets batteryDead status
  getCellTemps();  // sets batteryHot status
  updateBrightness();  // sets ccfl pwm level  
  updateCharging();  // update charging pwm levels based on cell Volts and Temps
  
if ((timenow - lastButtonDown > TURNOFF_TIME) && (lastButtonDown > lastButtonUp)) {
  turnoOff(); //  customer is trying to turn light off.
// set the time this started.
// 
}

if (batteryDead) {
  turnOff();
  } else if (batteryLow) {
     mode = 9; //Emergency / Get you home mode. 
  } 
   
}

void updateCharging () {
 //check the jack voltage to see if we're in danger of running USB power too low. 
 balanceCells(); 
  
}

void balanceCells () {
  if (charging) {
    // if cell1 or cell2 is higher than cell3, discharge same
    if (cell1 > cell3)
      discharge(CELL1);
    if (cell2 > cell3)
      discharge(CELL2);

    // If only cell3 is full, charge using C until cell1 or cell2 full
    if (cell3 > FULL && cell1 < FULL && cell2 < FULL)
      charge(C); // until cell1 or cell2 full

    // if cell1 is higher than cell2, discharge cell1
    if (cell1 > cell2)
      discharge(cell1);
    // some more logic goes here, but needs to be thought out fully
}

void getCellVoltages()  {
  cell1 = analogRead(B1P) / B1P_DIV;
  cell2 = (analogRead(B2P) / B2P_DIV) - cell1;  
  cell3 = (analogRead(BATTERY) / BATTERY_DIV) - cell1 - cell2;  
  batteryDead = ((cell1 <= EMPTY) || (cell2 <= EMPTY) || (cell3 <= EMPTY));
// healthy is the opposite of batteryLow  batteryHealthy = ((cell1 >=  BATT_LOW && (cell2 >= BATT_LOW ) && (cell3 >= BATT_LOW ));
  batteryLow = ((cell1 <  BATT_LOW || (cell2 < BATT_LOW ) || (cell3 < BATT_LOW ));
  batteryFull = ((cell1 >= FULL) && (cell2 >= FULL) && (cell3 >= FULL ));
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

void updateBrightness () {
      switch (mode) {
      case 0:  // Charging but light not ON.
      brightCentage=0; 
      //set brightness to nothing
      break; 
      
      case 1:
      //Customer wants light to be on.  
      brightCentage=100; 
      break;
      
    case 2:
      //Customer wants light to pulse.
      
     //Pulse pseudocode: be on full brightness for first 200 ms
     //fade to 0 over next 100 ms
     //be at 0 for next 200 ms
     //my attempt follows. 
     
     pulseTimer = currentTime % 500;
     if (pulseTimer > 300 ) 
       brightCentage=0;  
     else if (blinkTimer > 200){
       brightCentage = blinkTimer % 100;
     }  else brightCentage = 100);
      break;
      }
 if (batteryLow && mode>0){     
    
         //Lub-Dup Pulse pseudocode: 
         
       //  Be on fullbrightness for first 100 ms
     //fade to 30 over next 100 ms
     //Then be on fullbright again for another 100ms
     //Then fade to zero over next 50ms 
     //Then be at 0 for next 400ms
          
    }
}

void intHandler() {
  // if INT0 is triggered, switch to next mode, which might be "off"
  timeNow = millis();
  if (!digitalRead(BUTTON_SENSE) { // button just got pressed
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
}

void turnOff() {
  digitalWrite(ONFET, LOW);
}

void charge(int pin) { 
  charging = true; 

}

