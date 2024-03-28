#include "main.h"
#include "adc.h"

ADC_HandleTypeDef hadc1;

uint32_t ad_res1, ad_res2;

void MX_ADC1_Init(void){

	ADC_ChannelConfTypeDef sConfig = {0};

	hadc1.Instance = ADC1;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	if (HAL_ADC_Init(&hadc1) != HAL_OK){Error_Handler();}

	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK){Error_Handler();}
}

static void change_adc_channel(uint32_t channel){
	ADC_ChannelConfTypeDef sConfig = {0};

	sConfig.Channel = channel;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK){
			Error_Handler();
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	static uint32_t current_channel = ADC_CHANNEL_6;
	if( current_channel == ADC_CHANNEL_6 ){
		ad_res1 = HAL_ADC_GetValue(&hadc1); // Read & Update The ADC Result
		current_channel = ADC_CHANNEL_7;
	}else if(current_channel == ADC_CHANNEL_7){
		ad_res2 = HAL_ADC_GetValue(&hadc1); // Read & Update The ADC Result
		current_channel = ADC_CHANNEL_6;
	}
	change_adc_channel(current_channel);
}
