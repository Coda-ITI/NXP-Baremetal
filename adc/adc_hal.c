#include "adc_hal.h"


void ADC_HAL_vInit(void)
{
	ADC_DRV_Reset(INST_ADC_CONFIG_1);

	ADC_DRV_ConfigConverter(INST_ADC_CONFIG_1, &adc_config_1_ConvConfig0);

	ADC_DRV_ConfigChan(INST_ADC_CONFIG_1, 0, &adc_config_1_ChnConfig0);

	ADC_DRV_ConfigChan(INST_ADC_CONFIG_1, 0, &adc_config_1_ChnConfig0);


	ADC_DRV_Reset(INST_ADC_CONFIG_2);

	ADC_DRV_ConfigConverter(INST_ADC_CONFIG_2, &adc_config_2_ConvConfig0);

	ADC_DRV_ConfigChan(INST_ADC_CONFIG_2, 0, &adc_config_2_ChnConfig0);

	ADC_DRV_ConfigChan(INST_ADC_CONFIG_2, 0, &adc_config_2_ChnConfig0);
}


uint16_t ADC_HAL_u16GetPot1ReadingBlocking(void)
{
	uint16_t value = 0;

	ADC_DRV_ConfigChan(INST_ADC_CONFIG_1, 0, &adc_config_1_ChnConfig0);
	ADC_DRV_WaitConvDone(INST_ADC_CONFIG_1);
	ADC_DRV_GetChanResult(INST_ADC_CONFIG_1, 0, &value);

	return value;
}

uint16_t ADC_HAL_u16GetPot2ReadingBlocking(void)
{
	uint16_t value = 0;

	ADC_DRV_ConfigChan(INST_ADC_CONFIG_2, 0, &adc_config_2_ChnConfig0);
	ADC_DRV_WaitConvDone(INST_ADC_CONFIG_2);
	ADC_DRV_GetChanResult(INST_ADC_CONFIG_2, 0, &value);

	return value;
}
