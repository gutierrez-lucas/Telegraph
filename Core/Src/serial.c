#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "morse.h"
#include "serial.h"
#include "main.h"
#include <errno.h>
#include <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO

extern morse_s morse;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

char uart1_in_buffer[10]={'0'};
static bool print_once = 0;

UART_HandleTypeDef huart1;

int _write(int file, char *data, int len){
	 if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)){
			errno = EBADF;
			return -1;
	 }
	 HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, (uint8_t*)data, len, 1000);

	 return (status == HAL_OK ? len : 0);
}

void MX_USART1_UART_Init(void){
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;

	if (HAL_UART_Init(&huart1) != HAL_OK){
		Error_Handler();
	}
	
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	static int8_t pre_sm = 0;
	static int8_t spected_bytes[2] = {2,3};

	if(pre_sm == 1){
		pre_sm = 0;
		printf(" to %dms\r\n\n", atoi(uart1_in_buffer));
		set_unit_time_ms(&morse, atoi(uart1_in_buffer));
		uart1_in_buffer[2] = '\0';
	}else{
		if(!strcmp(uart1_in_buffer, "GT")){ // get time
			printf("[CMD] Current unit time in milliseconds: %d\r\n\n", get_unit_time_ms(&morse));
		}else if(!strcmp(uart1_in_buffer, "ST")){
			pre_sm = 1;
			printf("[CMD] Setting Morse Unit Time ");
		}else if(!strcmp(uart1_in_buffer, "CB")){
			printf("[CMD] Clearing word buffer\r\n\n");
			clear_msg_buffer(&morse);
		}else{
			memset(uart1_in_buffer, 0, sizeof(uart1_in_buffer));
		}
	}
	HAL_UART_Receive_IT(&huart1, uart1_in_buffer, spected_bytes[pre_sm]);
}

void print_menu(){
	if(morse.msg[0] != '\0'){
		printf("\r\nLast Word: %s", morse.msg);
		morse_clear_word(&morse);
	}
	printf("\r\nTelegraph Menu\r\n");
	printf("\r--------------\r\n");
	printf("GT: Get Morse Unit Time\r\n");
	printf("STxxx: Set Morse Unit Time to xxx miliseconds\r\n");
	printf("CB: Clear word buffer\r\n");
	printf("\r\n--------------");
	printf("\r\n");
	disable_print_menu();
}

bool is_print_menu_enabled(){
	return print_once;
}

void enable_print_menu(){
	print_once = true;
} 

void disable_print_menu(){
	print_once = false;
}