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

		if(morse_fsm_get_event(&morse) != NO_EVENT){
			morse_fsm_switch(&morse);
		}

		if(is_print_menu_enabled() == true){
			print_menu();
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
