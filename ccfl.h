#ifndef _INC_CCFL_H
#define _INC_CCFL_H

#include "arbduino.h"

class Ccfl : public RTBSensor, public RTBActuator {
  public:
	Ccfl( uint8_t sense_pin, uint8_t actuate_pin,
	  uint16_t ideal_full_current, uint8_t max_pwm );
	virtual void setup();
	virtual void sense();
	virtual void actuate();
	void set_pat_brightness( float pat_brightness );
  private:
	void refine_pwm_for_full_brightness();
	const uint8_t _sense_pin;  // current sensor
	const uint8_t _actuate_pin;  // control power to the CCFL
	const uint16_t _ideal_full_current;  // ideal measurement from current sensor (unitless)
	const uint8_t _max_pwm;  // hard limit to protect hardware
	uint16_t _actual_pat_current;  // measured current (affected by pattern)
	uint8_t _pwm_for_full_brightness;  // estimate of PWM to get full brightness
	float _pat_brightness;
};

#endif
