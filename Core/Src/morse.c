#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "morse.h"
#include "joystick.h"
#include "serial.h"

// #define MORSE_DEBUG
// #define MORSE_TEST
#ifdef MORSE_TEST
static void test_decoder();
#endif
// #define MORSE_DECODE_USING_LUT

#define START_POSITION 0b10000000 // just a mask for readability

morse_s morse;

static uint8_t decode_char_bin(char encoded_char);              // get binary representation of a character
static char decode_bin_char(uint8_t bin_char);                  // get character from binary representation
static bool synthetize_char(morse_s* morse, char target_char);
static bool synthetize_string(morse_s* morse, char* target_string, uint16_t length);

void morse_init(morse_s* self){
    printf("Init morse object\r\n");
    self->pulse.get_pulse_high_duration = js_get_pulse_high_duration;
    self->pulse.get_pulse_low_duration = js_get_pulse_low_duration;
    self->pulse.clear_pulse_low_duration = js_clear_low_pulse_duration;
    self->pulse.clear_pulse_high_duration = js_clear_high_pulse_duration;
    self->pulse.get_pulse_state = js_get_button_state;
    morse_restart(self); 
}

void morse_fsm_switch(morse_s* self){
    switch(self->sm.state){
        case(DOT_DASH_CNT):
            switch(self->sm.event){
                case(WAITING_FOR_HIGH):
                    break;
                case(DOT_FOUND):
                    printf("FMS: SAVE_DOT\r\n");
                    if(morse_add_symbol(&morse, DOT)!=OK){
                        morse_handle_status(&morse);
                        break;
                    }
                    morse_set_sm_state(&morse, CHAR_CNT);
                    break;
                case(DASH_FOUND):
                    printf("FMS: SAVE_DASH\r\n");
                    if(morse_add_symbol(&morse, DASH)!=OK){
                        morse_handle_status(&morse);
                        break;
                    }
                    morse_set_sm_state(&morse, CHAR_CNT);
                    break;
                case(ERR_LOW):	
                    morse_set_status(&morse, ERR_TIME_OVERFLOW);
                    morse_handle_status(&morse);
                    break;
                default: break;
            }
            break;

        case(CHAR_CNT):
            switch(self->sm.event){
                case(WAITING_FOR_HIGH):
                    break;
                case(CONTINUE_CHAR):
                    morse_set_sm_state(&morse, DOT_DASH_CNT);
                    break;
                case(CONTINUE_WORD):
                    printf("FMS: SAVE_CHAR -> ");
                    if(morse_add_char(&morse)!=OK){
                        morse_handle_status(&morse);
                        break;
                    }
                    if(is_com_complete(&morse)==false){
                        morse_set_sm_state(&morse, DOT_DASH_CNT);
                    }else{
                        printf("-------------\r\nFMS: SAVE_WORD --> ");
                        morse_save_word(&morse);
                        morse_handle_status(&morse);
                    }
                    break;

                default: break;
            }
            break;

        case(IDLE):
            break;
        
        default: break;
    }
}

sm_event_t morse_get_sm_event(morse_s* self){
    switch(self->sm.state){
        case(DOT_DASH_CNT):
            self->sm.event = get_low_status(self);
            return self->sm.event;
        case(CHAR_CNT):
            self->sm.event = get_high_status(self);
            return self->sm.event;
        default:
            return NO_EVENT;
    }
}

void morse_restart(morse_s* self){
    printf("Clear/config morse object\r\n");
    self->sm.state = IDLE;
    self->bin_char = 0;
    self->symbol_counter = 0;
    self->word_index = 0;
    self->status = OK;
    self->com_complete = false;
    self->pulse.clear_pulse_low_duration();
    self->pulse.clear_pulse_high_duration();
    self->unit_time_ms = 130;
#ifdef MORSE_TEST
    static bool once = false;
    if(once == false){
        once = true;
        test_decoder();
    }
#endif
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

sm_state_t morse_get_sm_state(morse_s* self){
    return self->sm.state;
}

void morse_handle_status(morse_s* self){
    printf("RES: ");
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
    morse_restart(&morse);
    enable_print_menu();    
}

sm_event_t get_high_status(morse_s* self){
	if( self->pulse.get_pulse_low_duration() != 0 ){ // waiting for high to low, so clear the previous low to high time
		self->pulse.clear_pulse_low_duration();
	}
	if( self->pulse.get_pulse_state() != JUST_RELEASED){
#ifdef MORSE_DEBUG 
		printf("FMS: CHAR_CNT HL=%dms\r\n", self->pulse.get_pulse_high_duration());
#endif
		if(self->pulse.get_pulse_high_duration() < 3*self->unit_time_ms){
			return(CONTINUE_CHAR);
		}else{
            if(self->pulse.get_pulse_high_duration() > 7*self->unit_time_ms){
                self->com_complete = true;
            }
			return(CONTINUE_WORD);
		}
	}else{
		return WAITING_FOR_LOW;
	}
}

sm_event_t get_low_status(morse_s* self){
	if( self->pulse.get_pulse_high_duration() != 0 ){
		self->pulse.clear_pulse_high_duration();
	}
	if( self->pulse.get_pulse_state() != PRESSED){
#ifdef MORSE_DEBUG 
		printf("FMS: D/DASH LD=%dms\r\n", js.button.low_duration);
#endif
		if(self->pulse.get_pulse_low_duration() < 3*self->unit_time_ms){
			return DOT_FOUND;
		}else if(self->pulse.get_pulse_low_duration() <= 7*self->unit_time_ms){
			return DASH_FOUND;
		}else{
			return ERR_LOW;
		}
	}else{
		return WAITING_FOR_HIGH;
	}
}

bool is_com_complete(morse_s* self){ return self->com_complete; }

static void clear_symbol_var(morse_s* self){
#ifdef MORSE_DEBUG 
    printf("[CLEAR_SYMBOL_VAR] Clearing symbol var\r\n");
#endif
    self->symbol_counter = 0;
    self->bin_char = 0;
}

bool morse_save_word(morse_s* self){
    self->morse_word[self->word_index] = '\0';
    printf(" %s\r\n-------------\r\n", self->morse_word);

    return true;
}

bool morse_clear_word(morse_s* self){
    memset(self->morse_word, 0, sizeof(self->morse_word));
    self->word_index = 0;

    return true;
}

#ifdef MORSE_DEBUG 
static void print_binary(uint8_t bin_char){ // just for dev purposes
    printf("Binary: ");
    for(int i = 7; i >= 0; i--){
        printf("%d", (bin_char & (1 << i)) >> i);
    }
    printf("\r\n");
}
#endif

morse_status_t morse_add_symbol(morse_s* self, morse_sym_t sym){
    static uint8_t symbol_position_mask = START_POSITION;

    if(self->symbol_counter < 6){
        if(sym == DOT){
            // self->bin_char &= ~(self->symbol_counter);
            // here goes a bit-clear, but its not necessary as the bin_char is initialized to 0
        }else if(sym == DASH){
#ifdef MORSE_DEBUG 
            print_binary(self->bin_char);
#endif
            self->bin_char |= (symbol_position_mask >> self->symbol_counter);
#ifdef MORSE_DEBUG 
            print_binary(self->bin_char);
#endif
        }else{
#ifdef MORSE_DEBUG 
            printf("[ADD_SYMBOL] Unknown symbol\r\n");
#endif
            self->status = ERR_CHAR_UNKNOWN;
            return FAIL; 
        }
        self->symbol_counter++;
        return OK;
    }else{
#ifdef MORSE_DEBUG 
        printf("[ADD_SYMBOL] Char overflow (sym_counter: %d)\r\n", self->symbol_counter);
#endif
        clear_symbol_var(self);
        symbol_position_mask = START_POSITION;
        self->status = ERR_CHAR_OVERFLOW; 
        return FAIL;
    }
}

morse_status_t morse_add_char(morse_s* self){
    morse_status_t ret; 
#ifdef MORSE_DEBUG 
    print_binary(self->bin_char);
#endif
    self->bin_char |= self->symbol_counter;    
#ifdef MORSE_DEBUG 
    print_binary(self->bin_char);
#endif
    self->morse_word[self->word_index] = decode_bin_char(self->bin_char);

    if(self->morse_word[self->word_index] != 0){
        printf(" %c\r\n", self->morse_word[self->word_index]);
        self->word_index++;
        ret = OK;
    }else{
#ifdef MORSE_DEBUG 
        printf("[ADD_CHAR] Unknown character\r\n");
#endif
        self->status = ERR_CHAR_UNKNOWN;
        ret = FAIL;
    }
    clear_symbol_var(self);

    return ret;
}

bool synthetize_char(morse_s* morse, char target_char){
#ifdef MORSE_DEBUG 
    printf("Target char: %c\r\n", target_char);
#endif
    uint8_t bin_char = decode_char_bin(target_char); 
#ifdef MORSE_DEBUG 
    printf("Decoded binary: ");
    print_binary(bin_char);
#endif
    if(bin_char == 0){
        printf("Unknown character\r\n");
        return false;
    }

    uint8_t sym_counter = (bin_char &~(0b11111000));
    uint8_t sym_position = START_POSITION;

    // printf("Symbol counter: %d\r\n", sym_counter);
    for(uint8_t i=0; i<sym_counter; i++){
        if(bin_char & sym_position){
            morse_add_symbol(morse, DASH);
        }else{
            morse_add_symbol(morse, DOT);
        }
        sym_position = sym_position >> 1;
    }
    return true;
}

#ifdef MORSE_TEST
static void test_decoder(){
    printf("START MORSE_TEST\r\n");
    synthetize_string(&morse, "HELLO WORLD", strlen("HELLO WORLD")); 
    morse_clear_word(&morse);
    printf("END MORSE_TEST\r\n");
}
#endif

bool synthetize_string(morse_s* morse, char* target_string, uint16_t length){
    for(uint8_t i=0; i<length; i++){
        if(target_string[i] == ' '){
            // add a not-so-trivial space
            continue;
        }
        if(synthetize_char(morse, target_string[i]) == false){
            return false;
        }else{
            morse_add_char(morse);
        }
    }
    return true;
}

static char decode_bin_char(uint8_t bin_char){
#ifdef MORSE_DECODE_USING_LUT
    const static char lut[256] =  
    {0,'E','I','S','H','5',0,0,0,0,0,0,0,'6',0,0,
     0,0,0,0,'B',0,0,0,0,0,0,0,0,'7',0,0,
     0,0,0,'D','L',0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,'Z',0,0,0,0,0,0,0,0,'8',0,0,
     0,0,'N','R','F',0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,'C',0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,'G','P',0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,'9',0,0,
     0,'T','A','U','V','4',0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,'X',0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,'K',0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,'Q',0,0,0,0,0,0,0,0,0,0,0,
     0,0,'M','W',0,'3',0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,'Y',0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,'o','J','2',0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,'1',0,0,0,0,0,0,0,'0',0,0};
    return lut[bin_char];
#else
    switch(bin_char){
        case(0b01000010):
            return('A');		
        case(0b10000100):
            return('B');		
        case(0b10100100):
            return('C');		
        case(0b10000011):
            return('D');		
        case(0b00000001):
            return('E');		
        case(0b00100100):
            return('F');		
        case(0b11000011):
            return('G');		
        case(0b00000100):
            return('H');		
        case(0b00000010):
            return('I');		
        case(0b01110100):
            return('J');		
        case(0b10100011):
            return('K');		
        case(0b01000100):
            return('L');		
        case(0b11000010):
            return('M');		
        case(0b10000010):
            return('N');		
        case(0b11100011):
            return('O');		
        case(0b01100100):
            return('P');		
        case(0b11010100):
            return('Q');		
        case(0b01000011):
            return('R');		
        case(0b00000011):
            return('S');
        case(0b10000001):
            return('T');
        case(0b00100011):
            return('U');
        case(0b00010100):
            return('V');
        case(0b01100011):
            return('W');
        case(0b10010100):
            return('X');
        case(0b10110100):
            return('Y');
        case(0b11000100):
            return('Z');
        case(0b11111101):
            return('0');
        case(0b01111101):
            return('1');
        case(0b00111101):
            return('2');
        case(0b00011101):
            return('3');
        case(0b00001101):
            return('4');
        case(0b00000101):
            return('5');
        case(0b10000101):
            return('6');
        case(0b11000101):
            return('7');
        case(0b11100101):
            return('8');
        case(0b11110101):
            return('9');
        default:
            return 0;
    }
#endif
}

static uint8_t decode_char_bin(char encoded_char){
    switch(encoded_char){
            case('A'):		
                return(0b01000010);
            case('B'):		
                return(0b10000100);
            case('C'):		
                return(0b10100100);
            case('D'):		
                return(0b10000011);
            case('E'):		
                return(0b00000001);
            case('F'):		
                return(0b00100100);
            case('G'):		
                return(0b11000011);
            case('H'):		
                return(0b00000100);
            case('I'):		
                return(0b00000010);
            case('J'):		
                return(0b01110100);
            case('K'):		
                return(0b10100011);
            case('L'):		
                return(0b01000100);
            case('M'):		
                return(0b11000010);
            case('N'):		
                return(0b10000010);
            case('O'):		
                return(0b11100011);
            case('P'):		
                return(0b01100100);
            case('Q'):		
                return(0b11010100);
            case('R'):		
                return(0b01000011);
            case('S'):
                return(0b00000011);
            case('T'):
                return(0b10000001);
            case('U'):
                return(0b00100011);
            case('V'):
                return(0b00010100);
            case('W'):
                return(0b01100011);
            case('X'):
                return(0b10010100);
            case('Y'):
                return(0b10110100);
            case('Z'):
                return(0b11000100);
            case('0'):
                return(0b11111101);
            case('1'):
                return(0b01111101);
            case('2'):
                return(0b00111101);
            case('3'):
                return(0b00011101);
            case('4'):
                return(0b00001101);
            case('5'):
                return(0b00000101);
            case('6'):
                return(0b10000101);
            case('7'):
                return(0b11000101);
            case('8'):
                return(0b11100101);
            case('9'):
                return(0b11110101);
            default: return(0b00000000);
    }
}

void set_unit_time_ms(morse_s* self, uint16_t unit_time_ms){
    self->unit_time_ms = unit_time_ms;
}

uint16_t get_unit_time_ms(morse_s* self){
    return self->unit_time_ms;
}
