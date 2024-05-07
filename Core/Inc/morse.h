#ifndef ___FSM__H__
#define ___FSM__H__

#include <stdint.h>
#include <stdbool.h>

#define USE_JOYSTICK

// opaque definition 
typedef struct morse_str morse_s;

// functions exposed for morse usage
void morse_init(morse_s* self, uint16_t start_unit_time);
void morse_run(morse_s* self);

// functions exposed for serial configuration system
void morse_show_buffer(morse_s* self);
void set_unit_time_ms(morse_s* self, uint16_t unit_time_ms);
uint16_t get_unit_time_ms(morse_s* self);
void clear_msg_buffer(morse_s* self);
bool morse_clear_word(morse_s* self);

#endif