#include "main.h"
#include <stdio.h>
#include <string.h>

#include "system.h"
#include "joystick.h"
#include "fsm.h"

extern joystick_s js;
extern sm_t sm;

#define MORSE_UNIT_TIME_MS 100

int main(void){

	init_system();

	char morse_char[6] = {0};
    uint8_t morse_char_index = 0;

	while (1){
		switch(sm.state){
			case(DOT_DASH_CNT):
				if(sm.lock == UNLOCKED){
					if(js.button.low_duration < 3*MORSE_UNIT_TIME_MS){
						sm.state = SAVE_DOT;
					}else if(js.button.low_duration <= 7*MORSE_UNIT_TIME_MS){
						sm.state = SAVE_DASH;
					}else{
						sm.status = ERR_TIME_OVERFLOW;
						sm.state = HANDLE_ERROR;
					}
				}
				break;

			case(SAVE_DOT):
				morse_char[morse_char_index] = '.';
				if( morse_char_index > 5){
					sm.status = ERR_CHAR_OVERFLOW;
					sm.state = HANDLE_ERROR;
					break;
				}
				morse_char_index++;
				sm.state = CHAR_CNT;
				break;

			case(SAVE_DASH):
				morse_char[morse_char_index] = '-';
				if( morse_char_index > 5){
					sm.status = ERR_CHAR_OVERFLOW;
					sm.state = HANDLE_ERROR;
					break;
				}
				morse_char_index++;
				sm.state = CHAR_CNT;
				break;

			case(CHAR_CNT):
				if(js.button.high_duration < 3*MORSE_UNIT_TIME_MS){
					sm.state = DOT_DASH_CNT;
				}else{
					if(morse_char_index < 5){
						sm.status = ERR_CHAR_INCOMPLETE;
						sm.state = HANDLE_ERROR;
						break;
					}
					sm.state = SAVE_CHAR;
				}
				break;
			
			case(SAVE_CHAR):
				// save char
				// clear morse_char and morse_char_index
				if(js.button.high_duration < 7*MORSE_UNIT_TIME_MS){
					sm.state = DOT_DASH_CNT;
				}else{
					sm.state = IDLE;
				}
				break;

			case(SAVE_WORD):
				// save word
				sm.state = IDLE;
				break;

			case(HANDLE_ERROR):
				switch(sm.status){
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
				}
				sm.status = OK;
				sm.state = IDLE;
				break;

			case(IDLE):
				// if(js.button.state == JUST_RELEASED){
				// 	printf("low time: %d\r\n", js.button.low_duration);
				// 	if(js.button.low_duration!=0){
				// 		js.button.low_duration = 0;
				// 	}
				// 	js.button.state = RELEASED;
				// }
				break;
			
			default: break;
			// case(READ):
			// 	// if(stop == TRUE){
			// 		break;
			// 	// }
			// 	counter++;
			// 	printf("ADC value %d : %d / %d \r\n", counter, ad_res1, ad_res2);
			// 	break;
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
