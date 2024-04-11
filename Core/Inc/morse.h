#ifndef ___FSM__H__
#define ___FSM__H__

#include <stdint.h>
#include <stdbool.h>

typedef enum{			// posible states for the state morse machine
	DOT_DASH_CNT,		// acquiring dots and dashes
	CHAR_CNT,			// geting a character form the acquired dots and dashes
	IDLE
}sm_state_t;

typedef enum{ 			// all posible events for the morse state machine
	WAITING_FOR_HIGH, 
	DOT_FOUND,          // less than 3 units
	DASH_FOUND, 		// more than 3 units but less than 7
	ERR_LOW,
	WAITING_FOR_LOW, 	
	CONTINUE_CHAR,		// less than 3 units
	CONTINUE_WORD,		// more than 3 units. if its less than 7 units, the complete flag is set to true
	ERR_HIGH,
	NO_EVENT
}sm_event_t;

typedef enum{
	ERR_CHAR_OVERFLOW,
	ERR_CHAR_INCOMPLETE,
	ERR_TIME_OVERFLOW,
	ERR_CHAR_UNKNOWN,
	FAIL,
	OK
}morse_status_t;

typedef struct sm_str{
	sm_state_t state;
	sm_event_t event;
}sm_t;

typedef enum{
	DOT,
	DASH
}morse_sym_t;

typedef struct pulse_str{
	uint8_t (*get_pulse_state)();
	uint16_t (*get_pulse_high_duration)();
	uint16_t (*get_pulse_low_duration)();
	void (*clear_pulse_low_duration)();
	void (*clear_pulse_high_duration)();
}pulse_s;

typedef struct morse_str{
	sm_t sm;					// state machine struct
	morse_status_t status;		// internal status, OK or ERR codes
	uint8_t bin_char;			// binary representation of the morse character
	uint8_t symbol_counter;		// how many symbols (dots/dash) are in the current character
	char morse_word[100];
	uint8_t word_index;
	bool com_complete;			// flag to indicate if the communication is complete
	pulse_s pulse;
	uint16_t unit_time_ms;
}morse_s;

void morse_init(morse_s* self);
sm_event_t morse_get_sm_event(morse_s* self);
void morse_fsm_switch(morse_s* self);
void morse_restart(morse_s* self);
bool morse_set_sm_state(morse_s* self, sm_state_t state);
bool morse_set_status(morse_s* self, morse_status_t status);
sm_state_t morse_get_sm_state(morse_s* self);
void morse_handle_status(morse_s* self);

sm_event_t get_high_status(morse_s* self);
sm_event_t get_low_status(morse_s* self);

bool morse_save_word(morse_s* self);
morse_status_t morse_add_symbol(morse_s* self, morse_sym_t sym);
morse_status_t morse_add_char(morse_s* self);

bool is_com_complete(morse_s* self);

void set_unit_time_ms(morse_s* self, uint16_t unit_time_ms);
uint16_t get_unit_time_ms(morse_s* self);
bool morse_clear_word(morse_s* self);
#endif