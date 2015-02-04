#include "arduino.h"
#include <Adafruit_NeoPixel.h>


// descriptive types
typedef uint32_t color_t;
typedef unsigned long millitime_t;


// Rock the Bike's sensors and actuators

class RTBSensor {
  public:
	virtual void setup() = 0;
	virtual void sense() = 0;
};

class RTBVoltageSensor : public RTBSensor {
  public:
	RTBVoltageSensor( const uint8_t pin, float coeff ) :
	  _pin(pin), _coeff(coeff) {
	}
	virtual void setup() {
		pinMode(_pin,INPUT);
	}
	virtual void sense() {
		_voltage = analogRead(_pin) / _coeff;
	}
	float voltage() const {
		return _voltage;
	}
  private:
	const uint8_t _pin;
	const float _coeff;
	float _voltage;
};

class RTBCurrentSensor : public RTBSensor {
  public:
	RTBCurrentSensor( const uint8_t pin, float coeff, float offset ) :
	  _pin(pin), _coeff(coeff), _offset(offset) {
	}
	virtual void setup() {
		pinMode(_pin,INPUT);
	}
	virtual void sense() {
		_current = ( analogRead(_pin) - _offset ) / _coeff;  // TODO:  flip signs? 
	}
	float current() const {
		return _current;
	}
  private:
	const uint8_t _pin;
	const float _coeff;
	const float _offset;
	float _current;
};

class RTBActuator {
  public:
	virtual void setup() = 0;
	virtual void actuate() = 0;
};

class RTBAddressableLedStrip : public RTBActuator {
  public:
	RTBAddressableLedStrip( const uint8_t pin, int length ) :
	  neopixel(length,pin,NEO_GRB|NEO_KHZ800), _length(length) {
	}
	virtual void setup() {
		neopixel.begin();
		neopixel.show(); // Initialize all pixels to 'off'
	}
	virtual void actuate() {
		neopixel.show();
	}
	void setPixelColor( uint16_t n, uint8_t r, uint8_t g, uint8_t b ) {
		neopixel.setPixelColor(n, r,g,b );
	}
	void setPixelColor( uint16_t n, uint32_t c ) {
		neopixel.setPixelColor(n,c);
	}
	void setBrightness(uint8_t b) {
		neopixel.setBrightness(b);
	}
	void clear() {
		neopixel.clear();
	};
  private:
	Adafruit_NeoPixel neopixel;
	const int _length;
};

class RTBSafetyRelay : public RTBActuator {
  public:
	RTBSafetyRelay( const uint8_t pin, float thresholdsetc, uint8_t state=LOW ) :
	  _pin(pin), _thresholdsetc(thresholdsetc), _state(state) {
	}
	virtual void setup() {
		pinMode(_pin,OUTPUT);
	}
	virtual void actuate() {
		digitalWrite( _pin, _state );
	};
  private:
	const uint8_t _pin;
	const int _thresholdsetc;
	uint8_t _state;
};
