#include "state.h"
#include "ccfl.h"
#include "wavegen.h"

extern PidCcfl ccfl;

float  little_dip_levels[] = {  1.0,  0.5,  1.0 };
wavet_t little_dip_times[] = { 1000, 1500, 2000 };
WaveGenerator little_dip( sizeof(little_dip_levels)/sizeof(*little_dip_levels),
  little_dip_levels, little_dip_times);

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
