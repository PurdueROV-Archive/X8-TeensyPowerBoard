

#ifndef HYDRAULICS
#define HYDRAULICS

#include "Arduino.h"

#define RAMP_TIMING  20 //wait (ms) in parent loop
#define MIN_VALUE    40
#define MAX_VALUE    179
#define ESC_PIN      6

#include "Servo.h"

class Hydraulics {
	public:
		Hydraulics();
		uint8_t set(uint8_t);
	private:
		Servo esc;
		uint8_t inc;
		uint8_t target;
		uint8_t value;
};

#endif
