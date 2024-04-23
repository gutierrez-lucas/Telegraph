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
	PRESSED,
	RELEASED,
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
	uint32_t (*get_pulse_high_duration)();
	uint32_t (*get_pulse_low_duration)();
	void (*set_pulse_state)(uint8_t);
	void (*set_pulse_high_duration)(uint32_t);
	void (*set_pulse_low_duration)(uint32_t);
	void (*add_ms_pulse_low)(uint32_t);
	void (*add_ms_pulse_high)(uint32_t);
}pulse_s;

typedef struct morse_str{
	char msg[20][20];
	sm_t sm;					// state machine struct
	morse_status_t status;		// internal status, OK or ERR codes
	uint8_t bin_char;			// binary representation of the morse character
	uint8_t symbol_counter;		// how many symbols (dots/dash) are in the current character
	uint8_t word_index;
	uint8_t word_counter;
	pulse_s pulse;
	uint16_t unit_time_ms;
}morse_s;

void morse_init(morse_s* self);

sm_event_t morse_fsm_get_event(morse_s* self);
void morse_fsm_switch(morse_s* self);

void morse_restart(morse_s* self);

bool morse_set_sm_state(morse_s* self, sm_state_t state);
sm_state_t morse_get_sm_state(morse_s* self);

bool morse_set_status(morse_s* self, morse_status_t status);
void morse_handle_status(morse_s* self);

bool morse_save_word(morse_s* self);
morse_status_t morse_add_symbol(morse_s* self, morse_sym_t sym);
morse_status_t morse_add_char(morse_s* self);

void set_unit_time_ms(morse_s* self, uint16_t unit_time_ms);
uint16_t get_unit_time_ms(morse_s* self);
bool morse_clear_word(morse_s* self);
void clear_msg_buffer(morse_s* self);
#endif