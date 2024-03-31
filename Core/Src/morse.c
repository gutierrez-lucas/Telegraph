#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "morse.h"
#include "serial.h"

// #define MORSE_DEBUG
#define MORSE_TEST
// #define MORSE_DECODE_USING_LUT

static uint8_t decode_char_bin(char encoded_char);
static char decode_bin_char(uint8_t bin_char);
bool synthetize_char(morse_s* morse, char target_char);
bool synthetize_string(morse_s* morse, char* target_string, uint16_t length);
morse_s morse;
#define START_POSITION 0b10000000
static uint8_t symbol_position_mask = START_POSITION;
static void test_decoder();

void morse_init(morse_s* self){
    printf("Initializing morse object\n");
    self->sm.state = IDLE;
    self->sm.lock = LOCKED;
    self->bin_char = 0;
    self->symbol_counter = 0;
    self->word_index = 0;
    self->status = OK;
#ifdef MORSE_TEST
    test_decoder();
#endif
}

static void clear_symbol_var(morse_s* self){
#ifdef MORSE_DEBUG 
    printf("[CLEAR_SYMBOL_VAR] Clearing symbol var\r\n");
#endif
    symbol_position_mask = START_POSITION;
    self->symbol_counter = 0;
    self->bin_char = 0;
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
        printf("[ADD_CHAR] Letter: %c\r\n", self->morse_word[self->word_index]);
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