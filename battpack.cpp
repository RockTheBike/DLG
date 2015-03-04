#include "battpack.h"

BatteryPack::BatteryPack() {
}

void BatteryPack::setup() {
	// don't need to set pinMode for sense pins
}

void BatteryPack::sense() {
	// TODO:  consider READ_MULTITUDE
	_voltages[0] = analogRead(B1P) / B1P_COEFF;
	_voltages[1] = analogRead(B2P) / B2P_COEFF - _voltages[0];
	_voltages[2] = analogRead(FULL_BATTERY) / BATTERY_COEFF - _voltages[0] - _voltages[1];
}

void BatteryPack::actuate() {
	// TODO:  control charging
}

#ifdef SERIAL_INTERACT
void BatteryPack::report( Stream& stream ) {
	stream.print("v:");
	stream.print(_voltages[0]);
	stream.print(",");
	stream.print(_voltages[1]);
	stream.print(",");
	stream.print(_voltages[2]);
}
#endif
