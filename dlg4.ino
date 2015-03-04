#define VERSION "Rock the Bike DLG 4.0"

#include "dlg4.h"
#include "state.h"
#include "ccfl.h"


// sensors and actuators
#define IDEAL_FULL_CURRENT 450
#define MAX_PWM 250
Ccfl ccfl( A6, 5, IDEAL_FULL_CURRENT, MAX_PWM );
RTBButton button( 2 );
RTBLed signal_led( 7 );
RTBPowerLatch power_latch( 4 );


// the state machine
StartingState starting_state;
State* current_state = &starting_state;


// reporting
#define REPORT_INTERVAL 1000
millitime_t next_report = 0;
void report( State* prev_state, State* next_state, WORLD_PARAMS ) {
	Serial.print( now/1000/60 );
	Serial.print( ':' );
	Serial.print( now/1000%60 );  // TODO "%02d"
	Serial.println( "A" ); 
}
void maybe_report( State* prev_state, State* next_state, WORLD_PARAMS ) {
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
