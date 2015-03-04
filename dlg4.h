#ifndef _INC_DLG4_H
#define _INC_DLG4_H

#include "arbduino.h"

#define WORLD_PARAMS \
  millitime_t now, \
  float ccfl_current, \
  bool button

#define WORLD_ARGS \
  now, \
  ccfl_current, \
  button

#define WORLD_VALUES \
  now, \
  ccfl.actual_pat_current(), \
  button.state()

class State;

void setup();
void loop();
void maybe_report( State* prev_state, State* next_state, WORLD_PARAMS );
void report( State* prev_state, State* next_state, WORLD_PARAMS );

#endif
