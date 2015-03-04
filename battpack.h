#ifndef _INC_BATTPACK_H
#define _INC_BATTPACK_H

#include "config.h"
#include "arbduino.h"

#ifdef SERIAL_INTERACT
#include <stream>
#endif

#define NUM_BATTERIES 3

class BatteryPack : public RTBSensor, public RTBActuator {
  public:
	BatteryPack();
	virtual void setup();
	virtual void sense();
	virtual void actuate();
#ifdef SERIAL_INTERACT
	void report( Stream& stream );
#endif
  protected:
	float _voltages[NUM_BATTERIES];
};

#endif
