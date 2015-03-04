#include <arbduino.h>

#define INITIAL_PWM_FOR_FULL_BRIGHTNESS 80

class Ccfl : RTBSensor, RTBActuator {
  public:
	Ccfl( uint8_t sense_pin, uint8_t actuate_pin,
	  uint8_t ideal_full_current, uint8_t max_pwm )
	: _sense_pin(sense_pin), _actuate_pin(actuate_pin),
	  _ideal_full_current(ideal_full_current), _max_pwm(max_pwm),
	  _pwm_for_full_brightness(INITIAL_PWM_FOR_FULL_BRIGHTNESS),
	{
	}
	virtual void setup() {
		pinMode(_pin,OUTPUT);
	}
	virtual void sense() {
		#define READ_MULTITUDE 50
		uint16_t sum = 0;  // big enough to sum 64 10-bit ints
		for( int i=0; i<READ_MULTITUDE; i++ )
			sum += analogRead(pin);
		_actual_pat_current = sum / READ_MULTITUDE;
	}
	virtual void actuate() {
		float pat_brightness = 1;  // TODO implement
		analogWrite( _pin, _pwm_for_full_brightness*pat_brightness );
	}
  private:
	void refine_pwm_for_full_brightness() {
#define MAX_PWM 250 
#define jumpVal 1 
	}
	const uint8_t _sense_pin;  // current sensor
	const uint8_t _actuate_pin;  // control power to the CCFL
	const uint8_t _ideal_full_current;  // ideal measurement from current sensor (unitless)
	const uint8_t _max_pwm;  // hard limit to protect hardware
	uint16_t _actual_pat_current;  // measured current (affected by pattern)
	uint8_t _pwm_for_full_brightness;  // estimate of PWM to get full brightness
}
