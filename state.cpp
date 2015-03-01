#include "state.h"
#include "ccfl.h"
#include "wavegen.h"

extern PidCcfl ccfl;

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

bool State::preempt( WORLD_PARAMS ) {
	return NULL;
}

State* StartingState::transition( WORLD_PARAMS ) { 
	return this;  // TODO 
} 
void StartingState::set_outputs( WORLD_PARAMS ) {
#ifdef ENABLE_PATTERN
	ccfl.set_wave_generator( &little_dip );
#endif
}
void StartingState::enter_state( WORLD_PARAMS ) {} 
