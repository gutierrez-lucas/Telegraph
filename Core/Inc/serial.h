#ifndef __SERIAL__H
#define __SERIAL__H
#include <stdbool.h>

int _write(int file, char *data, int len);
void MX_USART1_UART_Init(void);
void print_menu();
void enable_print_menu(); 
void disable_print_menu();
bool is_print_menu_enabled();
#endif