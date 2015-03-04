#ifndef _INC_STATE_H
#define _INC_STATE_H

#include "dlg4.h"

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

class StartingState : public State { 
	State* transition( WORLD_PARAMS ); 
	void set_outputs( WORLD_PARAMS ); 
	void enter_state( WORLD_PARAMS ); 
};

#endif
