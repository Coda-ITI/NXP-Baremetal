#ifndef ADC_HAL_H_
#define ADC_HAL_H_
#include "sdk_project_config.h"


extern void ADC_HAL_vInit(void);

extern uint16_t ADC_HAL_u16GetPot1ReadingBlocking(void);

extern uint16_t ADC_HAL_u16GetPot2ReadingBlocking(void);

#endif


