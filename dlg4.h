#ifndef _INC_DLG4_H
#define _INC_DLG4_H

#include "config.h"

#include "arbduino.h"

#define WORLD_PARAMS \
  millitime_t now, \
  uint32_t loopcount, \
  float lowest_batt_voltage, \
  bool button_state, \
  millitime_t button_completed_push

#define WORLD_ARGS \
  now, \
  loopcount, \
  lowest_batt_voltage, \
  button_state, \
  button_completed_push

#define WORLD_VALUES \
  now, \
  loopcount, \
  battery_pack.lowest_voltage(), \
  button.state(), \
  button.completed_push()

class State;

void setup();
void setup_frequencies();
void loop();
void maybe_report( State* prev_state, State* next_state, WORLD_PARAMS );
void report( State* prev_state, State* next_state, WORLD_PARAMS );

#endif
