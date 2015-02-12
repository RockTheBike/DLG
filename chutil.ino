#define VERSION "Rock the Bike Cheap/Virtual Utility Box 0.0"

#include "arbduino.h"
#include <Adafruit_NeoPixel.h>

#define VOLTCOEFF 13.179 // correct value for new blue arbduino v2
#define AMPCOEFF 8.0682 // 583 - 512 = 71; 71 / 8.8 amps = 8.0682
#define AMPOFFSET 512.0 // when current sensor is at 0 amps this is the ADC value
RTBVoltageSensor pedal_voltage( A1, VOLTCOEFF );
RTBVoltageSensor battery_voltage( A0, VOLTCOEFF );
RTBCurrentSensor pedal_current( A2, AMPCOEFF, AMPOFFSET );
RTBCurrentSensor output_current( A3, AMPCOEFF, AMPOFFSET );

RTBAddressableLedStrip ledstrip( 9, 6 );
RTBAddressableLedStrip indicatorled( 10, 1 );
RTBSafetyRelay safety_relay( 2, 14.3 );
RTBRelay output_relay( 3 );



#define WORLD_PARAMS \
  millitime_t now, \
  float pedal_voltage, \
  float battery_voltage, \
  float pedal_current, \
  float output_current

#define WORLD_ARGS \
  now, \
  pedal_voltage, \
  battery_voltage, \
  pedal_current, \
  output_current

#define WORLD_VALUES \
  now, \
  pedal_voltage.voltage(), \
  battery_voltage.voltage(), \
  pedal_current.current(), \
  output_current.current()

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

class HappyState : public State {
	State* transition( WORLD_PARAMS );
	void set_outputs( WORLD_PARAMS );
	void enter_state( WORLD_PARAMS );
} happy_state;
State* HappyState::transition( WORLD_PARAMS ) {
	return this;  // TODO
}
void HappyState::set_outputs( WORLD_PARAMS ) {
	// TODO
}
void HappyState::enter_state( WORLD_PARAMS ) {}

class LowDangerState : public State {
	State* transition( WORLD_PARAMS );
	void set_outputs( WORLD_PARAMS );
	void enter_state( WORLD_PARAMS );
} low_danger_state;
State* LowDangerState::transition( WORLD_PARAMS ) {
	return this;  // TODO
}
void LowDangerState::set_outputs( WORLD_PARAMS ) {
	// TODO
}
void LowDangerState::enter_state( WORLD_PARAMS ) {}

class HighDangerState : public State {
	State* transition( WORLD_PARAMS );
	void set_outputs( WORLD_PARAMS );
	void enter_state( WORLD_PARAMS );
} high_danger_state;
State* HighDangerState::transition( WORLD_PARAMS ) {
	return this;  // TODO
}
void HighDangerState::set_outputs( WORLD_PARAMS ) {
	// TODO
}
void HighDangerState::enter_state( WORLD_PARAMS ) {}

State* current_state = &starting_state;


#define REPORT_INTERVAL 1000
millitime_t next_report = 0;
void report( State* prev_state, State* next_state, millitime_t now, float pedal_voltage, float battery_voltage, float pedal_current, float output_current ) {
	Serial.print( now/1000/60 );
	Serial.print( ':' );
	Serial.print( now/1000%60 );  // TODO "%02d"
	Serial.print( "  pedalers: " );
	Serial.print( pedal_voltage );
	Serial.print( "V, " );
	Serial.print( pedal_current );
	Serial.print( "A  battery: " );
	Serial.print( battery_voltage );
	Serial.print( "V  inverter: " );
	Serial.print( output_current );
	Serial.println( "A" );
}
void maybe_report( State* prev_state, State* next_state, millitime_t now, float pedal_voltage, float battery_voltage, float pedal_current, float output_current ) {
	if( now < next_report ) return;
	report( prev_state, next_state, WORLD_ARGS );
	next_report += REPORT_INTERVAL + ( next_report ? 0 : now );
}


void setup() {

	Serial.begin(57600);
	Serial.println(VERSION);

	pedal_voltage.setup();
	battery_voltage.setup();
	pedal_current.setup();
	output_current.setup();
	ledstrip.setup();
	indicatorled.setup();
	safety_relay.setup();
	output_relay.setup();

}


void loop() {

	// look at the world
	millitime_t now = millis();
	pedal_voltage.sense();
	battery_voltage.sense();
	pedal_current.sense();
	output_current.sense();

	// decide what state to transition to
	// (and let outgoing state relinquish stuff etc)
	State* next_state = current_state->transition( WORLD_VALUES );
	if( next_state != current_state )
		next_state->enter_state( WORLD_VALUES );
	next_state->set_outputs( WORLD_VALUES );

	// change the world
	ledstrip.actuate();
	indicatorled.actuate();
	safety_relay.actuate();
	output_relay.actuate();

	// occassionally say what we did
	maybe_report( current_state, next_state, WORLD_VALUES );

	current_state = next_state;

}
