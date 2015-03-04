#ifndef _INC_DLG4_H
#define _INC_DLG4_H

#include "arbduino.h"

#define WORLD_PARAMS \
  millitime_t now, \
  uint32_t loopcount, \
  bool button

#define WORLD_ARGS \
  now, \
  loopcount, \
  button

#define WORLD_VALUES \
  now, \
  loopcount, \
  button.state()

class State;

void setup();
void setup_frequencies();
void loop();
void maybe_report( State* prev_state, State* next_state, WORLD_PARAMS );
void report( State* prev_state, State* next_state, WORLD_PARAMS );

#endif
