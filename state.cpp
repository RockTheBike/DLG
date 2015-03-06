#include "state.h"
#include "ccfl.h"
#include "wavegen.h"

StartingState starting_state;
LifesaverState lifesaver_state;
OffState off_state;

extern PidCcfl ccfl;
extern RTBPowerLatch power_latch;

// boring full power
float  full_levels[] = { 1.0, 1.0 };
wavet_t full_times[] = {   1,   2 };
WaveGenerator full( sizeof(full_levels)/sizeof(*full_levels),
  full_levels, full_times);

// a simple and gentle dip
float  little_dip_levels[] = {  1.0,  0.5,  1.0 };
wavet_t little_dip_times[] = { 1000, 1500, 2000 };
WaveGenerator little_dip( sizeof(little_dip_levels)/sizeof(*little_dip_levels),
  little_dip_levels, little_dip_times);

// slow heartbeat
float  slow_lubdub_levels[] = {   1.0, 1.0, 0.0, 0.0,   1.0, 1.0, 0.0, 0.0,   0.0   };
wavet_t slow_lubdub_times[] = {   100, 300, 500, 700,   800,1000,1200,1400,  1600   };
WaveGenerator slow_lubdub( sizeof(slow_lubdub_levels)/sizeof(*slow_lubdub_levels),
  slow_lubdub_levels, slow_lubdub_times);

// syncopated heartbeat
float  synco_lubdub_levels[] = {  1.0, 0.0,  0.0, 1.0, 1.0, 0.0,   0.0  };
wavet_t synco_lubdub_times[] = {   50, 250,  300, 350, 500, 700,  1000  };
WaveGenerator synco_lubdub( sizeof(synco_lubdub_levels)/sizeof(*synco_lubdub_levels),
  synco_lubdub_levels, synco_lubdub_times);

// lightsaber in heartbeat
float  lightsaber_lubdub_levels[] = {  1.0, 0.0,  0.0, 1.0, 1.0, 0.25, 0.0,   0.0  };
wavet_t lightsaber_lubdub_times[] = {   50, 250,  300, 350, 500,  501, 800,  1100  };
WaveGenerator lightsaber_lubdub( sizeof(lightsaber_lubdub_levels)/sizeof(*lightsaber_lubdub_levels),
  lightsaber_lubdub_levels, lightsaber_lubdub_times);

// strobe
float  strobe_levels[] = { 1.0, 0.0, 0.0, 1.0 };
wavet_t strobe_times[] = {  49,  50, 499, 500 };
WaveGenerator strobe( sizeof(strobe_levels)/sizeof(*strobe_levels),
  strobe_levels, strobe_times);

// harsh square (for testing)
float  harsh_square_levels[] = { 1.0, 0.0, 0.0, 1.0 };
wavet_t harsh_square_times[] = { 249, 250, 499, 500 };
WaveGenerator harsh_square( sizeof(harsh_square_levels)/sizeof(*harsh_square_levels),
  harsh_square_levels, harsh_square_times);

// for power off
float  flat_line_levels[] = { 0.0, 0.0 };
wavet_t flat_line_times[] = {   1,   2 };
WaveGenerator flat_line( sizeof(flat_line_levels)/sizeof(*flat_line_levels),
  flat_line_levels, flat_line_times);

int lightmode_index = 0;
WaveGenerator* lightmodes[] = {
  &full,
  //&little_dip,
  //&slow_lubdub,
  //&synco_lubdub,
  &lightsaber_lubdub,
  &strobe,
  &harsh_square,
};
#define NUM_LIGHTMODES (sizeof(lightmodes)/sizeof(*lightmodes))


bool State::preempt( WORLD_PARAMS ) {
	return NULL;
}

const char* StartingState::name() {
	return "starting";
}
State* StartingState::transition( WORLD_PARAMS ) {
	if( lowest_batt_voltage < BATT_EMPTY )
		return &off_state;
	if( lowest_batt_voltage < BATT_LOW )
		return &lifesaver_state;
	if( button_completed_push == 0 )  // not a push
		return this;
	if( button_completed_push < 50 ) {  // just (bouncy) noise
		Serial.println("ignored bounce");
		return this;
	}
	if( button_completed_push < 500 ) {
		lightmode_index = ( lightmode_index + 1 ) % NUM_LIGHTMODES;
		Serial.print("switching mode to ");
		Serial.print( lightmode_index );
		Serial.print("/");
		Serial.println( NUM_LIGHTMODES );
		ccfl.set_wave_generator( lightmodes[lightmode_index] );
		return this;
	}
	if( button_completed_push < 1000 ) {
		Serial.println("turning off.");
		return &off_state;
	}
	// longer push is probably noise of eg shoving into backpack
	return this;
}
void StartingState::set_outputs( WORLD_PARAMS ) {
	power_latch.on();
}
void StartingState::enter_state( WORLD_PARAMS ) {
	ccfl.set_wave_generator( lightmodes[lightmode_index] );
}


const char* LifesaverState::name() {
	return "lifesaver";
}
State* LifesaverState::transition( WORLD_PARAMS ) {
	if( lowest_batt_voltage < BATT_EMPTY )
		return &off_state;
	if( button_completed_push == 0 )  // not a push
		return this;
	if( button_completed_push < 50 ) {  // just (bouncy) noise
		Serial.println("ignored bounce");
		return this;
	}
	if( button_completed_push < 1000 ) {
		Serial.println("turning off.");
		return &off_state;
	}
	// longer push is probably noise of eg shoving into backpack
	// TODO:  but holding down should turn off
	return this;
}
void LifesaverState::set_outputs( WORLD_PARAMS ) {
	power_latch.on();
}
void LifesaverState::enter_state( WORLD_PARAMS ) {
	ccfl.set_wave_generator( &lightsaber_lubdub );
}


const char* OffState::name() {
	return "off";
}
State* OffState::transition( WORLD_PARAMS ) {
	if( button_completed_push < 500 )  // just (bouncy) noise
		return this;
	Serial.println("turning on.");
	return &starting_state;
}
void OffState::set_outputs( WORLD_PARAMS ) {
	ccfl.set_wave_generator( &flat_line );
	power_latch.off();
}
void OffState::enter_state( WORLD_PARAMS ) {}
