#ifndef _INC_CCFL_H
#define _INC_CCFL_H

#include "config.h"

#include "arbduino.h"

#ifdef SERIAL_INTERACT
#include <stream>
#endif

class Ccfl : public RTBSensor, public RTBActuator {
  public:
	Ccfl( uint8_t sense_pin, uint8_t actuate_pin,
	  uint16_t ideal_full_current, uint8_t max_pwm );
	virtual void setup();
	virtual void sense();
	virtual void actuate();
	void set_pat_brightness( float pat_brightness );
  protected:
	const uint8_t _sense_pin;  // current sensor
	const uint8_t _actuate_pin;  // control power to the CCFL
	const uint16_t _ideal_full_current;  // ideal measurement from current sensor (unitless)
	const uint8_t _max_pwm;  // hard limit to protect hardware
	uint16_t _actual_pat_current;  // measured current (affected by pattern)
	uint8_t _pwm_for_full_brightness;  // estimate of PWM to get full brightness
	float _pat_brightness;
	virtual void refine_pwm_for_full_brightness() = 0;
};

class PidCcfl : public Ccfl {
  public:
	PidCcfl( uint8_t sense_pin, uint8_t actuate_pin,
	  uint16_t ideal_full_current, uint8_t max_pwm,
	  float kp, float ki, float kd );
  protected:
	virtual void refine_pwm_for_full_brightness();
	float _kp;
	float _ki;
	float _kd;
	float _integrated_error;
	int16_t _prev_error;
#ifdef SERIAL_INTERACT
  public:
	void report( Stream& stream );
#endif
};

#endif
