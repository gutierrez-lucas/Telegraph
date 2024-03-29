#include <stdio.h>
#include "morse.h"
#include "serial.h"

static char decode_bin_char(uint8_t bin_char);

void morse_init(morse_s* self){
    printf("Initializing morse object\n");

    self->sm.state = IDLE;
    self->sm.lock = LOCKED;
    self->bin_char = 0;
    self->bin_char_index = 0;
    self->word_index = 0;
    self->status = OK;
}

void morse_fsm_lock(morse_s* self){
    self->sm.lock = LOCKED;
}
void morse_fsm_unlock(morse_s* self){
    self->sm.lock = UNLOCKED;
}

static inline bool check_state(sm_state_t state){ return(state <= IDLE ? true : false); }
bool morse_set_sm_state(morse_s* self, sm_state_t state){
    if(check_state(state)){
        self->sm.state = state;
        return true;
    }else{
        return false;
    }
}

static inline bool check_status(morse_status_t status){ return(status <= OK ? true : false); }
bool morse_set_status(morse_s* self, morse_status_t status){
    if(check_status(status)){
        self->status = status;
        return true;
    }else{
        return false;
    }
}

morse_status_t morse_add_char(morse_s* self){
    if(self->bin_char_index == 6){
        self->morse_word[self->word_index] = decode_bin_char(self->bin_char);
        if(self->morse_word[self->word_index] != 0){
            self->word_index++;
            return OK;
        }else{
            self->status = ERR_CHAR_UNKNOWN;
            return FAIL;
        }
    }else{
        self->status = ERR_CHAR_INCOMPLETE;
        return FAIL;
    }
}

morse_status_t morse_add_symbol(morse_s* self, morse_sym_t sym){
    if(self->bin_char_index < 6){
        if(sym == DOT){
            self->bin_char &= ~(self->bin_char_index);
        }else if(sym == DASH){
            self->bin_char |= self->bin_char_index;
        }else{
            self->status = ERR_CHAR_UNKNOWN;
            return FAIL; 
        }
        self->bin_char_index++;
        return OK;
    }else{
        self->bin_char_index = 0;
        self->status = ERR_CHAR_OVERFLOW; 
        return FAIL;
    }
}

void morse_handle_status(morse_s* self){
    switch(self->status){
        case(OK):
            printf("OK\r\n");
            break;
        case(ERR_CHAR_OVERFLOW):
            printf("ERR_CHAR_OVERFLOW\r\n");
            break;
        case(ERR_CHAR_INCOMPLETE):
            printf("ERR_CHAR_INCOMPLETE\r\n");
            break;
        case(ERR_TIME_OVERFLOW):
            printf("ERR_TIME_OVERFLOW\r\n");
            break;
        case(ERR_CHAR_UNKNOWN):
            printf("ERR_CHAR_UNKNOWN\r\n");
            break;
        default: 
            printf("Unknown status\r\n");
            break;
    }
}

sm_state_t morse_get_sm_state(morse_s* self){
    return self->sm.state;
}

sm_locked_status_t morse_get_sm_locked_status(morse_s* self){
    return self->sm.lock;
}

static char decode_bin_char(uint8_t bin_char){
    const static char lut[256] =  {'5','E','I','S','H',0,0,0,'6',0,0,0,0,0,0,0,0,0,0,0,'B',0,0,0,'7',0,0,0,0,0,0,0,0,0,0,'D','L',0,0,0,0,
                                    0,0,0,0,0,0,0,0,0,0,0,'Z',0,0,0,'8',0,0,0,0,0,0,0,0,0,'N','R','F',0,0,0,0,0,0,0,0,0,0,0,0,0,
                                    0,0,'C',0,0,0,0,0,0,0,0,0,0,0,0,0,0,'G','P',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'9',0,0,
                                    0,0,0,0,0,'4','T','A','U','V',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'X',0,0,0,0,0,0,0,0,0,0,0,0,0,0,'K',
                                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'Q',0,0,0,0,0,0,0,0,0,0,0,'3',0,'M','W',0,0,0,0,0,0,0,0,0,
                                    0,0,0,0,0,0,0,'Y',0,0,0,0,0,0,0,0,0,0,0,'2',0,0,'o','J',0,0,0,0,0,0,0,0,0,0,0,'1',0,0,0,0,0,
                                    0,0,'0',0,0,0,0,0,0,0};
    return lut[bin_char];
}