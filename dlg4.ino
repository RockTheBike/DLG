#define VERSION "Rock the Bike DLG 4.0"

#include "dlg4.h"
#include "state.h"
#include "battpack.h"
#include "ccfl.h"


// sensors and actuators
BatteryPack battery_pack;
PidCcfl ccfl( CCFL_CURRENT_SENSE, CCFL_PIN, IDEAL_FULL_CURRENT, MAX_PWM, KP, KI, KD );
RTBButton button( 2 );
RTBLed signal_led( 7 );
RTBPowerLatch power_latch( 4 );


// the state machine
StartingState starting_state;
State* current_state = &starting_state;


// reporting
millitime_t next_report = 0;
void report( State* prev_state, State* next_state, WORLD_PARAMS ) {
	Serial.print( loopcount );
	Serial.print( "loops " );
	Serial.print( now/1000/60 );
	Serial.print( ':' );
	Serial.print( now/1000%60 );  // TODO "%02d"
#ifdef SERIAL_INTERACT
	Serial.print( ' ' );
	battery_pack.report( Serial );
	Serial.print( ' ' );
	ccfl.report( Serial );
#endif
	Serial.println();
}
void maybe_report( State* prev_state, State* next_state, WORLD_PARAMS ) {
	if( now < next_report ) return;
	report( prev_state, next_state, WORLD_ARGS );
	next_report += REPORT_INTERVAL + ( next_report ? 0 : now );
}


void setup() {

	setup_frequencies();

	Serial.println(VERSION);

	power_latch.setup();  // ensure our power stays on as soon as we can
	button.setup();
	ccfl.setup();
	signal_led.setup();

}

#define DELAYFACTOR 64
void setup_frequencies() {
	// mostly taken from Jake's dlg3_test2.ino without full comprehension
	CLKPR = 0x80;  // enable write to clkps see page 37
	CLKPR = 0x00;  // set divisor to 1  see page 38
	TCCR0A = 0b10100011;
	TCCR0B = 0b00000001;
#define SERIAL_SPEED_ADJUSTMENT 2  // not sure why
	Serial.begin( 38400 * SERIAL_SPEED_ADJUSTMENT );
}

uint32_t loopcount = 0;
void loop() {

	// look at the world
	millitime_t now = millis()/DELAYFACTOR;
	button.sense();
	ccfl.sense();
	battery_pack.sense();

	// decide what state to transition to
	// (and let outgoing state relinquish stuff etc)
	State* next_state = current_state->transition( WORLD_VALUES );
	if( next_state != current_state )
		next_state->enter_state( WORLD_VALUES );
	next_state->set_outputs( WORLD_VALUES );

	// change the world
	ccfl.actuate();
	battery_pack.sense();
	signal_led.actuate();
	power_latch.actuate();

	// occassionally say what we did
	maybe_report( current_state, next_state, WORLD_VALUES );

	current_state = next_state;
	loopcount++;

}
