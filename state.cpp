#include "state.h"

bool State::preempt( WORLD_PARAMS ) {
	return NULL;
}

State* StartingState::transition( WORLD_PARAMS ) { 
	return this;  // TODO 
} 
void StartingState::set_outputs( WORLD_PARAMS ) { 
	// TODO 
} 
void StartingState::enter_state( WORLD_PARAMS ) {} 
