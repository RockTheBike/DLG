#include <arbduino.h>

class Ccfl : RTBSensor, RTBActuator {
  public:
	Ccfl( uint8_t sense_pin, uint8_t actuate_pin, uint8_t ideal_full_current ) :
	_sense_pin(sense_pin), _actuate_pin(actuate_pin), _ideal_full_current(ideal_full_current) {
	}
	virtual void setup() {
		neopixel.begin(); 
		
	}
	virtual void sense() {
		_voltage = analogRead(_pin) / _coeff;
	}
	virtual void actuate() {
		neopixel.show();
	}
  private:
	const uint8_t _sense_pin;
	const uint8_t _actuate_pin;
	const uint8_t _ideal_full_current;
}
