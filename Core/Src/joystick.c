#include <stdio.h>
#include "joystick.h"
#include "stdint.h"

joystick_s js = {.button.state = RELEASED, .button.low_duration = 0};

void js_clear_low_pulse_duration(){ js.button.low_duration = 0; }
void js_clear_high_pulse_duration(){ js.button.high_duration = 0; }
uint16_t js_get_pulse_low_duration(){ return js.button.low_duration; }
uint16_t js_get_pulse_high_duration(){ return js.button.high_duration; }
button_state_t js_get_button_state(){ return js.button.state; }

void print_joystick_position(joystick_position_t position){
    switch(position){
        case(CENTER):
            printf("C\r\n");
            break;
        case(TOP):
            printf("T\r\n");
            break;
        case(BOTTOM):
            printf("B\r\n");
            break;
        case(RIGHT):
            printf("R\r\n");
            break;
        case(LEFT):
            printf("L\r\n");
            break;
        case(TOP_LEFT):
            printf("TL\r\n");
            break;
        case(TOP_RIGHT):
            printf("TR\r\n");
            break;
        case(BOTTOM_LEFT):
            printf("BL\r\n");
            break;
        case(BOTTOM_RIGHT):
            printf("BR\r\n");
            break;
        default:
            printf("N\r\n");
    }
}