#include "ccfl.h"

#define INITIAL_PWM_FOR_FULL_BRIGHTNESS 80

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
#define READ_MULTITUDE 10
#ifdef READ_MULTITUDE
	uint16_t sum = 0;  // big enough for sum of 64 10-bit ints
	// if the reading is faster than PWM, we'll need to wait a while
	for( int i=0; i<READ_MULTITUDE; i++ )
		sum += analogRead(_sense_pin);
	_actual_pat_current = sum / READ_MULTITUDE;
#else
	_actual_pat_current = analogRead(_sense_pin);
#endif
}
void Ccfl::actuate() {
	analogWrite( _actuate_pin, _pwm_for_full_brightness*_pat_brightness );
}
void Ccfl::set_pat_brightness( float pat_brightness ) {
	_pat_brightness = pat_brightness;
}
void Ccfl::refine_pwm_for_full_brightness() {
#define MAX_PWM 250 
#define jumpVal 1 
	// TODO:  based on _pat_brightness and _actual_pat_current
}
