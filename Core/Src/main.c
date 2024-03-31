#include "main.h"
#include <stdio.h>
#include <string.h>

#include "system.h"
#include "joystick.h"
#include "morse.h"

extern joystick_s js;
extern morse_s morse;

int main(void){

	init_system();
	morse_init(&morse);

	while (1){
		switch(morse_get_sm_state(&morse)){
			case(DOT_DASH_CNT):
				if( js.button.high_duration != 0 ){
					js.button.high_duration = 0;
				}
				// if(morse_get_sm_locked_status(&morse) == UNLOCKED){
				if( js.button.state != PRESSED){
					printf("FMS: D/DASH LD=%dms\r\n", js.button.low_duration);
					if(js.button.low_duration < 3*MORSE_UNIT_TIME_MS){
						morse_set_sm_state(&morse, SAVE_DOT);
					}else if(js.button.low_duration <= 7*MORSE_UNIT_TIME_MS){
						morse_set_sm_state(&morse, SAVE_DASH);
					}else{
						morse_set_status(&morse, ERR_TIME_OVERFLOW);
						morse_set_sm_state(&morse, HANDLE_STATUS);
					}
				}
				break;

			case(SAVE_DOT):
				printf("FMS: SAVE_DOT\r\n");
				if(morse_add_symbol(&morse, DOT)!=OK){
					morse_set_sm_state(&morse, HANDLE_STATUS);
					break;
				}
				morse_set_sm_state(&morse, CHAR_CNT);
				break;

			case(SAVE_DASH):
				printf("FMS: SAVE_DASH\r\n");
				if(morse_add_symbol(&morse, DASH)!=OK){
					morse_set_sm_state(&morse, HANDLE_STATUS);
					break;
				}
				morse_set_sm_state(&morse, CHAR_CNT);
				break;

			case(CHAR_CNT):
				if( js.button.low_duration != 0 ){
					js.button.low_duration = 0;
				}
				if( js.button.state != JUST_RELEASED){
					printf("FMS: CHAR_CNT HL=%dms\r\n", js.button.high_duration);
					if(js.button.high_duration < 3*MORSE_UNIT_TIME_MS){
						morse_set_sm_state(&morse, DOT_DASH_CNT);
					}else{
						morse_set_sm_state(&morse, SAVE_CHAR);
					}
				}
				break;
			
			case(SAVE_CHAR):
				printf("FMS: SAVE_CHAR\r\n");
				if(morse_add_char(&morse)!=OK){
					morse_set_sm_state(&morse, HANDLE_STATUS);
					break;
				}
				if(js.button.high_duration < 7*MORSE_UNIT_TIME_MS){
					morse_set_sm_state(&morse, DOT_DASH_CNT);
				}else{
					morse_set_sm_state(&morse, SAVE_WORD);
				}
				break;

			case(SAVE_WORD):
				printf("FMS: SAVE_WORD\r\n");
				morse_save_word(&morse);
				morse_set_sm_state(&morse, HANDLE_STATUS);
				break;

			case(HANDLE_STATUS):
				printf("FMS: HANDLE_STATUS\r\n");
				morse_handle_status(&morse);
				morse_set_status(&morse, OK); 
				morse_set_sm_state(&morse, IDLE);
				js.button.low_duration = 0;
				js.button.high_duration = 0;
				break;

			case(IDLE):
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
