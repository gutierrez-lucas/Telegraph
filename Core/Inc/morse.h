#ifndef ___FSM__H__
#define ___FSM__H__

#include <stdint.h>
#include <stdbool.h>

#define MORSE_UNIT_TIME_MS 150
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
	uint8_t symbol_counter;
	char morse_word[100];
	uint8_t word_index;
	bool com_complete;
}morse_s;

typedef enum{
	WAITING_FOR_HIGH, 
	DOT_FOUND,
	DASH_FOUND, 
	ERR_LOW
}low_time_t;

typedef enum{
	WAITING_FOR_LOW, 
	CONTINUE_CHAR,
	CONTINUE_WORD,
	ERR_HIGH
}high_time_t;

void morse_restart(morse_s* self);
bool is_com_complete(morse_s* self);
high_time_t get_high_status(morse_s* self);
low_time_t get_low_status(morse_s* self);
void morse_init(morse_s* self);
morse_status_t morse_add_symbol(morse_s* self, morse_sym_t sym);
morse_status_t morse_add_char(morse_s* self);
void morse_handle_status(morse_s* self);
bool morse_set_status(morse_s* self, morse_status_t status);
bool morse_set_sm_state(morse_s* self, sm_state_t state);
bool morse_save_word(morse_s* self);
sm_state_t morse_get_sm_state(morse_s* self);
sm_locked_status_t morse_get_sm_locked_status(morse_s* self);
#endif