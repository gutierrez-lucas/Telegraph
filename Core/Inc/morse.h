#ifndef ___FSM__H__
#define ___FSM__H__

#include <stdint.h>
#include <stdbool.h>

#define MORSE_UNIT_TIME_MS 100

typedef enum{
		DOT_DASH_CNT,
		SAVE_DOT,
		SAVE_DASH,
		CHAR_CNT,
		SAVE_CHAR,
		SAVE_WORD,
		HANDLE_STATUS,
		IDLE
}sm_state_t;

typedef enum{
		LOCKED,
		UNLOCKED
}sm_locked_status_t;

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
		sm_locked_status_t lock;
}sm_t;

typedef enum{
	DOT,
	DASH
}morse_sym_t;

typedef struct morse_str{
	sm_t sm;
	morse_status_t status;
	uint8_t bin_char;
	uint8_t bin_char_index;
	char morse_word[100];
	uint8_t word_index;
}morse_s;

void morse_init(morse_s* self);
morse_status_t morse_add_symbol(morse_s* self, morse_sym_t sym);
void morse_handle_status(morse_s* self);
bool morse_set_status(morse_s* self, morse_status_t status);
bool morse_set_sm_state(morse_s* self, sm_state_t state);
void morse_fsm_unlock(morse_s* self);
void morse_fsm_lock(morse_s* self);
sm_state_t morse_get_sm_state(morse_s* self);
sm_locked_status_t morse_get_sm_locked_status(morse_s* self);
#endif