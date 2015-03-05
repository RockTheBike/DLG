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
	RTBButton( uint8_t pin, bool state=false );
	virtual void setup();
	virtual void sense();
	bool state() const;
  protected:
	const uint8_t _pin;
	bool _state;
};

class RTBTimedButton : public RTBButton {
  public:
	RTBTimedButton( uint8_t pin, bool state=false );
	virtual void sense();
	millitime_t completed_push() const;
#ifdef HAVE_A_USE_YET_FOR_TIME_PUSHED
	millitime_t time_pushed() const;
#endif
  protected:
	bool _prev_state;
	millitime_t _time_of_last_press;
	millitime_t _time_of_last_release;
};

class RTBVoltageSensor : public RTBSensor {
  public:
	RTBVoltageSensor( uint8_t pin, float coeff );
	virtual void setup();
	virtual void sense();
	float voltage() const;
  private:
	const uint8_t _pin;
	const float _coeff;
	float _voltage;
};

class RTBCurrentSensor : public RTBSensor {
  public:
	RTBCurrentSensor( uint8_t pin, float coeff, float offset );
	virtual void setup();
	virtual void sense();
	float current() const;
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
	RTBLed( uint8_t pin, uint8_t state=LOW );
	virtual void setup();
	virtual void actuate();
  private:
	const uint8_t _pin;
	uint8_t _state;
};

class RTBRelay : public RTBActuator {
  public:
	RTBRelay( uint8_t pin, uint8_t state=LOW );
	virtual void setup();
	virtual void actuate();
  private:
	const uint8_t _pin;
	uint8_t _state;
};

class RTBSafetyRelay : public RTBRelay {
  public:
	RTBSafetyRelay( uint8_t pin, float thresholdsetc, uint8_t state=LOW );
  private:
	const int _thresholdsetc;
};

class RTBPowerLatch : public RTBActuator {
  public:
	RTBPowerLatch( uint8_t pin );
	virtual void setup();
	virtual void actuate();
	void on();
	void off();
  private:
	const uint8_t _pin;
	bool _state;
};

#endif
