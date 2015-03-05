#include "arbduino.h"


RTBButton::RTBButton( uint8_t pin, bool state ) :
  _pin(pin), _state(state) {
}
void RTBButton::setup() {
	pinMode(_pin,INPUT);
}
void RTBButton::sense() {
	_state = digitalRead(_pin);
}
bool RTBButton::state() const {
	return _state;
}


RTBTimedButton::RTBTimedButton( uint8_t pin, bool state ) :
  RTBButton(pin,state), _prev_state(state),
  _time_of_last_press(0), _time_of_last_release(0) {
}
void RTBTimedButton::sense() {
	_prev_state = _state;
	RTBButton::sense();
	if( _prev_state != _state ) {  // record the change
#define DELAYFACTOR 64
		millitime_t now = millis()/DELAYFACTOR;  // TODO:  get exact time from somewhere 
		( _state ? _time_of_last_press : _time_of_last_release ) = now;
	}
}
// if it was released just now, return duration of push, otherwise 0
millitime_t RTBTimedButton::completed_push() const {
	return !_state && _prev_state ?
	  _time_of_last_release - _time_of_last_press : 0;
}
#ifdef HAVE_A_USE_YET_FOR_TIME_PUSHED
millitime_t RTBTimedButton::time_pushed() const {
	if( _state && _prev_state ) {  // still pushed
	#define DELAYFACTOR 64
		millitime_t now = millis()/DELAYFACTOR;  // TODO:  get exact time from somewhere
		return _time_of_last_press - now;
	} else
		return 0;
}
#endif


RTBVoltageSensor::RTBVoltageSensor( uint8_t pin, float coeff ) :
  _pin(pin), _coeff(coeff) {
}
void RTBVoltageSensor::setup() {
	pinMode(_pin,INPUT);
}
void RTBVoltageSensor::sense() {
	_voltage = analogRead(_pin) / _coeff;
}
float RTBVoltageSensor::voltage() const {
	return _voltage;
}


RTBCurrentSensor::RTBCurrentSensor( uint8_t pin, float coeff, float offset ) :
  _pin(pin), _coeff(coeff), _offset(offset) {
}
void RTBCurrentSensor::setup() {
	pinMode(_pin,INPUT);
}
void RTBCurrentSensor::sense() {
	_current = ( analogRead(_pin) - _offset ) / _coeff;
}
float RTBCurrentSensor::current() const {
	return _current;
}


RTBLed::RTBLed( uint8_t pin, uint8_t state ) :
  _pin(pin), _state(state) {
}
void RTBLed::setup() {
	pinMode(_pin,OUTPUT);
}
void RTBLed::actuate() {
	digitalWrite( _pin, _state );
}


RTBRelay::RTBRelay( uint8_t pin, uint8_t state ) :
  _pin(pin), _state(state) {
}
void RTBRelay::setup() {
	pinMode(_pin,OUTPUT);
}
void RTBRelay::actuate() {
	digitalWrite( _pin, _state );
}


RTBSafetyRelay::RTBSafetyRelay( uint8_t pin, float thresholdsetc, uint8_t state ) :
	RTBRelay(pin,state), _thresholdsetc(thresholdsetc) {
}


RTBPowerLatch::RTBPowerLatch( uint8_t pin ) :
  _pin(pin), _state(true) {
}
void RTBPowerLatch::setup() {
	pinMode(_pin,OUTPUT);
	// ensure our power stays on as soon as we can
	actuate();
}
void RTBPowerLatch::actuate() {
	digitalWrite( _pin, _state );
}
void RTBPowerLatch::on() {
	_state = true;
}
void RTBPowerLatch::off() {
	_state = false;
}
