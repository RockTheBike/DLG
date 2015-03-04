#ifndef _INC_ARBDUINO_H
#define _INC_ARBDUINO_H

#include <arduino.h>


// descriptive types
typedef uint32_t color_t;
typedef unsigned long millitime_t;


// Rock the Bike's sensors and actuators

class RTBSensor {
  public:
	virtual void setup() = 0;
	virtual void sense() = 0;
};

class RTBButton : public RTBSensor {
  public:
	RTBButton( const uint8_t pin ) :
	  _pin(pin) {
	}
	virtual void setup() {
		pinMode(_pin,INPUT);
	};
	virtual void sense() {
		_state = digitalRead(_pin);
	};
	bool state() const {
		return _state;
	}
  private:
	const uint8_t _pin;
	bool _state;
};

class RTBVoltageSensor : public RTBSensor {
  public:
	RTBVoltageSensor( const uint8_t pin, float coeff ) :
	  _pin(pin), _coeff(coeff) {
	}
	virtual void setup() {
		pinMode(_pin,INPUT);
	};
	virtual void sense() {
		_voltage = analogRead(_pin) / _coeff;
	};
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
	};
	virtual void sense() {
		_current = ( analogRead(_pin) - _offset ) / _coeff;  // TODO:  flip signs? 
	};
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

class RTBLed : public RTBActuator {
  public:
	RTBLed( const uint8_t pin, uint8_t state=LOW ) :
	  _pin(pin), _state(state) {
	}
	virtual void setup() {
		pinMode(_pin,OUTPUT);
	}
	virtual void actuate() {
		digitalWrite( _pin, _state );
	}
  private:
	const uint8_t _pin;
	uint8_t _state;
};

class RTBRelay : public RTBActuator {
  public:
	RTBRelay( const uint8_t pin, uint8_t state=LOW ) :
	  _pin(pin), _state(state) {
	}
	virtual void setup() {
		pinMode(_pin,OUTPUT);
	};
	virtual void actuate() {
		digitalWrite( _pin, _state );
	};
  private:
	const uint8_t _pin;
	uint8_t _state;
};

class RTBSafetyRelay : public RTBRelay {
  public:
	RTBSafetyRelay( const uint8_t pin, float thresholdsetc, uint8_t state=LOW ) :
	  RTBRelay(pin,state), _thresholdsetc(thresholdsetc) {
	}
  private:
	const int _thresholdsetc;
};

class RTBPowerLatch : public RTBActuator {
  public:
	RTBPowerLatch( const uint8_t pin ) :
	  _pin(pin), _state(true) {
	}
	virtual void setup() {
		pinMode(_pin,OUTPUT);
		// ensure our power stays on as soon as we can
		actuate();
	}
	virtual void actuate() {
		digitalWrite( _pin, _state );
	}
  private:
	const uint8_t _pin;
	bool _state;
};

#endif
