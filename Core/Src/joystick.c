#include <stdio.h>
#include "joystick.h"
#include "stdint.h"

joystick_s js = {.button.state = RELEASED, .button.low_duration = 0};

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