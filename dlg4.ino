#define VERSION "Rock the Bike DLG 4.0"

#include "arbduino.h"

// sensors
RTBButton button( somepin );

// combo sensor+actuators
#define AMPCOEFF 8.0682 // 583 - 512 = 71; 71 / 8.8 amps = 8.0682 
#define AMPOFFSET 512.0 // when current sensor is at 0 amps this is the ADC value 
#define IDEAL_FULL_CURRENT guess 
RTBCcfl ccfl( somepin, somepin, AMPCOEFF, AMPOFFSET, IDEAL_FULL_CURRENT ); 

// actuators
RTBLed signal_led( somepin );
RTBPowerLock power_lock( somepin );



#define WORLD_PARAMS \
  millitime_t now, \
  float ccfl_current, \
  float button

#define WORLD_ARGS \
  now, \
  ccfl_current, \
  button

#define WORLD_VALUES \
  now, \
  ccfl_current.current(), \
  button.state()

class State {
  public:
	// a chance to claim control outside normal operation
	virtual bool preempt( WORLD_PARAMS );
	// decide the next step based on the world, do any transition side effects if state differs
	virtual State* transition( WORLD_PARAMS ) = 0;
	// get ready to change the world
	virtual void set_outputs( WORLD_PARAMS ) = 0;
	virtual void enter_state( WORLD_PARAMS ) = 0;
};
bool State::preempt( WORLD_PARAMS ) {
	return NULL;
}

class StartingState : public State { 
	State* transition( WORLD_PARAMS ); 
	void set_outputs( WORLD_PARAMS ); 
	void enter_state( WORLD_PARAMS ); 
} starting_state; 
State* StartingState::transition( WORLD_PARAMS ) { 
	return this;  // TODO 
} 
void StartingState::set_outputs( WORLD_PARAMS ) { 
	// TODO 
	static const color_t colors[] = { 
	  Adafruit_NeoPixel::Color(255,0,0), 
	  Adafruit_NeoPixel::Color(255,255,0), 
	  Adafruit_NeoPixel::Color(0,255,0), 
	  Adafruit_NeoPixel::Color(0,255,255), 
	  Adafruit_NeoPixel::Color(0,0,255), 
	  Adafruit_NeoPixel::Color(255,0,255) }; 
	indicatorled.setBrightness( 8 ); 
#define NUM_COLORS (sizeof(colors)/sizeof(*colors)) 
	indicatorled.setPixelColor( 0, colors[now/200%NUM_COLORS] ); 
} 
void StartingState::enter_state( WORLD_PARAMS ) {} 


State* current_state = &starting_state;


#define REPORT_INTERVAL 1000
millitime_t next_report = 0;
void report( State* prev_state, State* next_state, millitime_t now, float ccfl_current, float button ) {
	Serial.print( now/1000/60 );
	Serial.print( ':' );
	Serial.print( now/1000%60 );  // TODO "%02d"
	Serial.println( "A" ); 
}
void maybe_report( State* prev_state, State* next_state, millitime_t now, float ccfl_current, float button ) {
	if( now < next_report ) return;
	report( prev_state, next_state, WORLD_ARGS );
	next_report += REPORT_INTERVAL + ( next_report ? 0 : now );
}


void setup() {

	Serial.begin(57600);
	Serial.println(VERSION);

	power_latch.setup();  // ensure our power stays on as soon as we can
	button.setup();
	ccfl.setup();
	signal_led.setup();

}


void loop() {

	// look at the world
	millitime_t now = millis();
	button.sense();
	ccfl.sense();

	// decide what state to transition to
	// (and let outgoing state relinquish stuff etc)
	State* next_state = current_state->transition( WORLD_VALUES );
	if( next_state != current_state )
		next_state->enter_state( WORLD_VALUES );
	next_state->set_outputs( WORLD_VALUES );

	// change the world
	ccfl.actuate();
	signal_led.actuate();
	power_latch.actuate();

	// occassionally say what we did
	maybe_report( current_state, next_state, WORLD_VALUES );

	current_state = next_state;

}
