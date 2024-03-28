#ifndef __JOY_STICK__H
#define __JOY_STICK__H

#include "stdint.h"

#define JS_POST_HIGH_LIMIT 3000
#define JS_POST_LOW_LIMIT 1000

typedef enum{
		CENTER,
		TOP,
		BOTTOM,
		LEFT,
		RIGHT,
		TOP_LEFT,
		TOP_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_RIGHT
}joystick_position_t;

typedef enum{
	PRESSED, 
	JUST_RELEASED,
	RELEASED
}button_state_t;

typedef struct button_str{
	button_state_t state;
	uint16_t low_duration;
	uint16_t high_duration;
}button_t;

typedef struct joystick_str{
	joystick_position_t position;
	button_t button;
}joystick_s;


void print_joystick_position(joystick_position_t position);
#endif