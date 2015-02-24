#include "state.h"
#include "ccfl.h"

extern Ccfl ccfl;

bool State::preempt( WORLD_PARAMS ) {
	return NULL;
}

State* StartingState::transition( WORLD_PARAMS ) { 
	return this;  // TODO 
} 
void StartingState::set_outputs( WORLD_PARAMS ) {
	// TODO 
	// for now, just make a little pattern
	int t = now % 2000;
	float pattern =
	  t < 700 ? 1.0 :
	  t < 800 ? 0.5 :
	  t < 900 ? 0.3 :
	  t < 1200 ? 0.0 :
	  t < 1300 ? 0.3 :
	  t < 1400 ? 0.5 :
	  1;
	ccfl.set_pat_brightness( pattern );
}
void StartingState::enter_state( WORLD_PARAMS ) {} 
