#include "main.h"
#include "gpio.h"
#include "joystick.h"
#include "morse.h"

extern joystick_s js;
extern morse_s morse;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	HAL_EXTI_ClearPending(GPIO_Pin, EXTI_TRIGGER_RISING_FALLING);
	if(GPIO_Pin == GPIO_PIN_9){
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_RESET){
			// printf("H->L\r\n");
			js_set_signal_state(SIGNAL_LOW);
		}else{
			// printf("L->H\r\n");
			js_set_signal_state(SIGNAL_HIGH);
		}
	}
}

void MX_GPIO_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
