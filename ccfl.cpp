#include "ccfl.h"

Ccfl::Ccfl( uint8_t sense_pin, uint8_t actuate_pin,
	uint16_t ideal_full_current, uint8_t max_pwm )
: _sense_pin(sense_pin), _actuate_pin(actuate_pin),
	_ideal_full_current(ideal_full_current), _max_pwm(max_pwm),
	_pwm_for_full_brightness(INITIAL_PWM_FOR_FULL_BRIGHTNESS),
	_pat_brightness(1)
{
}
void Ccfl::setup() {
	pinMode(_actuate_pin,OUTPUT);
	// don't need to set pinMode for _sense_pin
}
void Ccfl::sense() {
#ifdef READ_MULTITUDE
	uint16_t sum = 0;  // big enough for sum of 64 10-bit ints
	// if the reading is faster than PWM, we'll need to wait a while
	for( int i=0; i<READ_MULTITUDE; i++ )
		sum += analogRead(_sense_pin);
	_actual_pat_current = sum / READ_MULTITUDE;
#else
	_actual_pat_current = analogRead(_sense_pin);
#endif
	refine_pwm_for_full_brightness();
}
void Ccfl::actuate() {
	analogWrite( _actuate_pin, _pwm_for_full_brightness*_pat_brightness );
}
void Ccfl::set_pat_brightness( float pat_brightness ) {
	_pat_brightness = pat_brightness;
}

PidCcfl::PidCcfl( uint8_t sense_pin, uint8_t actuate_pin,
  uint16_t ideal_full_current, uint8_t max_pwm,
  float kp, float ki, float kd )
: Ccfl( sense_pin, actuate_pin, ideal_full_current, max_pwm ),
  _kp(kp), _ki(ki), _kd(kd), _integrated_error(0), _prev_error(0)
{
}

void PidCcfl::refine_pwm_for_full_brightness() {
	if( _pat_brightness < 0.95 )  return;
	// TODO:  scale properly according to pattern (instead of skipping when pattern isn't full)
	uint16_t unpat_current = _actual_pat_current/_pat_brightness;  // a first approximation 
	int16_t error = (int16_t)_ideal_full_current - unpat_current;
	_integrated_error += error;
	int16_t deriv_error = error - _prev_error;
	_prev_error = error;
	int16_t new_pwm = _pwm_for_full_brightness +
	  _kp * error +
	  _ki * _integrated_error +
	  _kd * deriv_error;
	if( new_pwm > MAX_PWM )
		_pwm_for_full_brightness = MAX_PWM;
	else if( new_pwm < MIN_PWM )
		_pwm_for_full_brightness = MIN_PWM;
	else
		_pwm_for_full_brightness = new_pwm;
}

#ifdef SERIAL_INTERACT
void PidCcfl::report( Stream& stream ) {
	stream.print("actual_pat_current:");
	stream.print(_actual_pat_current);
	stream.print(" pwm_for_full_brightness:");
	stream.print(_pwm_for_full_brightness);
	stream.print(" pat_brightness:");
	stream.print(_pat_brightness);
	stream.print(" integrated_error:");
	stream.print(_integrated_error);
	stream.print(" prev_error:");
	stream.print(_prev_error);
}
#endif
