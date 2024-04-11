#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "system.h"
#include "morse.h"
#include "serial.h"

extern morse_s morse;

int main(void){

	init_system();
	morse_init(&morse);
	enable_print_menu();

	while (1){
		switch(morse_get_sm_state(&morse)){
			case(DOT_DASH_CNT):
				switch(get_low_status(&morse)){
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
				}
				break;

			case(CHAR_CNT):
				switch(get_high_status(&morse)){
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
				if(is_print_menu_enabled() == true){
					disable_print_menu();
					print_menu();
				}
				break;
			
			default: break;
		}
	}
}


/**
	* @brief  This function is executed in case of error occurrence.
	* @retval None
	*/
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1){
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
	* @brief  Reports the name of the source file and the source line number
	*         where the assert_param error has occurred.
	* @param  file: pointer to the source file name
	* @param  line: assert_param error line source number
	* @retval None
	*/
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
		 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
