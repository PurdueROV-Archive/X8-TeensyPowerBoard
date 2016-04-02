#include "hydraulics.h"


Hydraulics::Hydraulics (void) {
	esc.attach(ESC_PIN);
	inc = (MAX_VALUE - MIN_VALUE) / (1000 / RAMP_TIMING);
	return;
}

uint8_t Hydraulics::set (uint8_t target) {
	if (target < MIN_VALUE || target > MAX_VALUE) {
		value = 0;
	} else if (target >= MIN_VALUE && value < MIN_VALUE) {
		value = MIN_VALUE;
	} else if (target < value) {
		value = target;
	} else if (value < target) {
		value += inc;
	}
	
	if (value > MAX_VALUE) value = MAX_VALUE;
	
	esc.write(value);
	return value;
}
