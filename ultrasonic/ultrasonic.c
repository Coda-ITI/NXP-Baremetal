#include "ultrasonic.h"
#include "ftm_common.h"
#include "FreeRTOSConfig.h"

#define ULTRASONIC_NO_OF_ULTRASONICS		4

#define FTM0_CH0		(0U)
#define FTM0_CH2		(2U)
#define FTM0_CH4		(4U)
#define FTM0_CH6		(6U)

#define FTM1_CH0		(0U)
#define FTM1_CH2		(2U)
#define FTM1_CH4		(4U)
#define FTM1_CH6		(6U)


#define ULTRASONIC_PULSE_TICKS		11

static status_t error = STATUS_SUCCESS;

static ftm_state_t stateInputCapture = {{0}};

static ftm_state_t stateOutputCompare = {{0}};

static volatile ULTRASONIC_readingCallBack_t Ultrasonic_arrCallbacks[ULTRASONIC_NO_OF_ULTRASONICS] = {NULL};

typedef struct{
	GPIO_Type * port;
	uint32_t pin;
	uint32_t channel;
}Ultrasonic_stPulseData_t;

static const Ultrasonic_stPulseData_t ULTRASONIC_arrPulsePins[ULTRASONIC_NO_OF_ULTRASONICS] = {
		[ULTRASONIC_NO_1] = {
				.port = PTC,
				.channel=FTM1_CH0,
				.pin = (1 << 23)
		},
		[ULTRASONIC_NO_2] = {
				.port = PTC,
				.channel=FTM1_CH2,
				.pin = (1 << 11)
		},
		[ULTRASONIC_NO_3] = {
				.port = PTC,
				.channel=FTM1_CH4,
				.pin = (1 << 10)
		},
		[ULTRASONIC_NO_4] = {
				.port = PTA,
				.channel=FTM1_CH6,
				.pin = (1 << 3)
		}
};

static float Ultrasonic_arrMostRecentMeasurements[ULTRASONIC_NO_OF_ULTRASONICS] = {0};

static float ULTRASONIC_f32CalibrationFactor = 0.0034;


void FTM0_CH0_ICU_callBack(ic_event_t event, void *userData);
void FTM0_CH2_ICU_callBack(ic_event_t event, void *userData);
void FTM0_CH4_ICU_callBack(ic_event_t event, void *userData);
void FTM0_CH6_ICU_callBack(ic_event_t event, void *userData);

static void FTM1_ch0_OC_Callback(void);
static void FTM1_ch2_OC_Callback(void);
static void FTM1_ch4_OC_Callback(void);
static void FTM1_ch6_OC_Callback(void);

void FTM0_Overflow_callBack(void);
void FTM1_Overflow_callback(void);





static inline void ULTRASONIC_vInitFTM0(void)
{
	  INT_SYS_ClearPending(FTM0_Ovf_Reload_IRQn);
	  INT_SYS_EnableIRQ(FTM0_Ovf_Reload_IRQn);
	  INT_SYS_InstallHandler(FTM0_Ovf_Reload_IRQn, FTM0_Overflow_callBack, NULL);

	  error = FTM_DRV_Init(INST_FLEXTIMER_IC_1, &flexTimer_ic_1_InitConfig, &stateInputCapture);
	  DEV_ASSERT(error == STATUS_SUCCESS);

	  error = FTM_DRV_InitInputCapture(INST_FLEXTIMER_IC_1, &flexTimer_ic_1_InputCaptureConfig);
	  DEV_ASSERT(error == STATUS_SUCCESS);
}

static inline void ULTRASONIC_vInitFTM1(void)
{
	  error = FTM_DRV_Init(INST_FLEXTIMER_OC_1, &flexTimer_oc_1_InitConfig, &stateOutputCompare);

	  error = FTM_DRV_InitOutputCompare(INST_FLEXTIMER_OC_1, &flexTimer_oc_1_OutputCompareConfig);

	  INT_SYS_ClearPending(FTM1_Ovf_Reload_IRQn);
	  INT_SYS_EnableIRQ(FTM1_Ovf_Reload_IRQn);
	  INT_SYS_InstallHandler(FTM1_Ovf_Reload_IRQn, FTM1_Overflow_callback, NULL);

	  INT_SYS_ClearPending(FTM1_Ch0_Ch1_IRQn);
	  INT_SYS_EnableIRQ(FTM1_Ch0_Ch1_IRQn);
	  INT_SYS_InstallHandler(FTM1_Ch0_Ch1_IRQn, FTM1_ch0_OC_Callback, NULL);

	  INT_SYS_ClearPending(FTM1_Ch2_Ch3_IRQn);
	  INT_SYS_EnableIRQ(FTM1_Ch2_Ch3_IRQn);
	  INT_SYS_InstallHandler(FTM1_Ch2_Ch3_IRQn, FTM1_ch2_OC_Callback, NULL);

	  INT_SYS_ClearPending(FTM1_Ch4_Ch5_IRQn);
	  INT_SYS_EnableIRQ(FTM1_Ch4_Ch5_IRQn);
	  INT_SYS_InstallHandler(FTM1_Ch4_Ch5_IRQn, FTM1_ch4_OC_Callback, NULL);

	  INT_SYS_ClearPending(FTM1_Ch6_Ch7_IRQn);
	  INT_SYS_EnableIRQ(FTM1_Ch6_Ch7_IRQn);
	  INT_SYS_InstallHandler(FTM1_Ch6_Ch7_IRQn, FTM1_ch6_OC_Callback, NULL);
}



void ULTRASONIC_vInit(void)
{
	ULTRASONIC_vInitFTM0();
	ULTRASONIC_vInitFTM1();
}

void ULTRASONIC_vSetCalibrationFactor(float factor)
{
	ULTRASONIC_f32CalibrationFactor = factor;
}

float ULTRASONIC_vGetCalibrationFactor(void)
{
	return ULTRASONIC_f32CalibrationFactor;
}

ULTRASONIC_enuStatus_t ULTRASONIC_enuSendPulse(ULTRASONIC_enuNO_t ultrasonic)
{
	ULTRASONIC_enuStatus_t status = ULTRASONIC_STATUS_SUCCESS;
	if(ultrasonic > ULTRASONIC_NO_4)
	{
		status = ULTRASONIC_STATUS_INVALID_NO;
	}
	else
	{
		PINS_DRV_SetPins(
				  ULTRASONIC_arrPulsePins[ultrasonic].port,
				  ULTRASONIC_arrPulsePins[ultrasonic].pin
		);
		FTM_DRV_UpdateOutputCompareChannel(INST_FLEXTIMER_OC_1, ULTRASONIC_arrPulsePins[ultrasonic].channel,ULTRASONIC_PULSE_TICKS , ULTRASONIC_PULSE_TICKS, false);
		FTM1->CONTROLS[ULTRASONIC_arrPulsePins[ultrasonic].channel].CnSC |= (FTM_CnSC_ELSA_MASK | FTM_CnSC_ELSB_MASK);
		FTM1->CONTROLS[ULTRASONIC_arrPulsePins[ultrasonic].channel].CnSC |= (FTM_CnSC_CHIE_MASK | FTM_CnSC_CHF_MASK);
	}
	return status;
}

ULTRASONIC_enuStatus_t ULTRASONIC_enuGetMeasurement(ULTRASONIC_enuNO_t ultrasonic,float* measurement)
{
	ULTRASONIC_enuStatus_t status = ULTRASONIC_STATUS_SUCCESS;
	if(measurement == NULL)
	{
		status = ULTRASONIC_NULL_PTR;
	}
	else if(ultrasonic > ULTRASONIC_NO_4)
	{
		status = ULTRASONIC_STATUS_INVALID_NO;
	}
	else
	{
		*measurement = Ultrasonic_arrMostRecentMeasurements[ultrasonic];
	}

	return status;
}

ULTRASONIC_enuStatus_t ULTRASONIC_enuInstallMeasurementCallBack(ULTRASONIC_enuNO_t ultrasonic,ULTRASONIC_readingCallBack_t callBack)
{
	ULTRASONIC_enuStatus_t status = ULTRASONIC_STATUS_SUCCESS;
	if(ultrasonic > ULTRASONIC_NO_4)
	{
		status = ULTRASONIC_STATUS_INVALID_NO;
	}
	else
	{
		Ultrasonic_arrCallbacks[ultrasonic] = callBack;
	}
	return status;
}


void FTM0_Overflow_callBack(void)
{
	FTM_DRV_ClearStatusFlags(INST_FLEXTIMER_IC_1, FTM_TIME_OVER_FLOW_FLAG);
}

void FTM1_Overflow_callback(void)
{
	FTM_DRV_ClearStatusFlags(INST_FLEXTIMER_OC_1, FTM_TIME_OVER_FLOW_FLAG);
}



void FTM0_CH0_ICU_callBack(ic_event_t event, void *userData)
{
	uint32_t val = FTM_DRV_GetInputCaptureMeasurement(INST_FLEXTIMER_IC_1, FTM0_CH0);
	Ultrasonic_arrMostRecentMeasurements[ULTRASONIC_NO_1] = ULTRASONIC_f32CalibrationFactor * val;
	if(Ultrasonic_arrCallbacks[ULTRASONIC_NO_1] != NULL)
	{
		Ultrasonic_arrCallbacks[ULTRASONIC_NO_1](Ultrasonic_arrMostRecentMeasurements[ULTRASONIC_NO_1]);
		#define LED0_PORT PTE
		#define LED0_PIN  21
		#define LED1_PORT PTE
		#define LED1_PIN  22
		if(val < 300)
		{
			PINS_DRV_SetPins(LED0_PORT, 1 << LED0_PIN);
			PINS_DRV_SetPins(LED1_PORT, 1 << LED1_PIN);

		}
		else
		{
			PINS_DRV_ClearPins(LED0_PORT,  1 << LED0_PIN);
			PINS_DRV_ClearPins(LED1_PORT,  1 << LED1_PIN);

		}
	}

}

void FTM0_CH2_ICU_callBack(ic_event_t event, void *userData)
{
	volatile uint32_t val = FTM_DRV_GetInputCaptureMeasurement(INST_FLEXTIMER_IC_1, FTM0_CH2);
	Ultrasonic_arrMostRecentMeasurements[ULTRASONIC_NO_2] = ULTRASONIC_f32CalibrationFactor * val;
	if(Ultrasonic_arrCallbacks[ULTRASONIC_NO_2] != NULL)
	{
		Ultrasonic_arrCallbacks[ULTRASONIC_NO_2](Ultrasonic_arrMostRecentMeasurements[ULTRASONIC_NO_2]);
	}

}

void FTM0_CH4_ICU_callBack(ic_event_t event, void *userData)
{
	volatile uint32_t val = FTM_DRV_GetInputCaptureMeasurement(INST_FLEXTIMER_IC_1, FTM0_CH4);
	Ultrasonic_arrMostRecentMeasurements[ULTRASONIC_NO_3] = ULTRASONIC_f32CalibrationFactor * val;
	if(Ultrasonic_arrCallbacks[ULTRASONIC_NO_3] != NULL)
	{
		Ultrasonic_arrCallbacks[ULTRASONIC_NO_3](Ultrasonic_arrMostRecentMeasurements[ULTRASONIC_NO_3]);
	}
}

void FTM0_CH6_ICU_callBack(ic_event_t event, void *userData)
{
	volatile uint32_t val = FTM_DRV_GetInputCaptureMeasurement(INST_FLEXTIMER_IC_1, FTM0_CH6);
	Ultrasonic_arrMostRecentMeasurements[ULTRASONIC_NO_4] = ULTRASONIC_f32CalibrationFactor * val;
	if(Ultrasonic_arrCallbacks[ULTRASONIC_NO_4] != NULL)
	{
		Ultrasonic_arrCallbacks[ULTRASONIC_NO_4](Ultrasonic_arrMostRecentMeasurements[ULTRASONIC_NO_4]);
	}
}

static void FTM1_ch0_OC_Callback(void)
{
	  PINS_DRV_ClearPins(ULTRASONIC_arrPulsePins[ULTRASONIC_NO_1].port,ULTRASONIC_arrPulsePins[ULTRASONIC_NO_1].pin);
	  FTM1->CONTROLS[FTM1_CH0].CnSC &= ~(FTM_CnSC_ELSA_MASK | FTM_CnSC_ELSB_MASK);
	  FTM1->CONTROLS[FTM1_CH0].CnSC &= ~(FTM_CnSC_CHIE_MASK | FTM_CnSC_CHF_MASK);
}

static void FTM1_ch2_OC_Callback(void)
{
	  PINS_DRV_ClearPins(ULTRASONIC_arrPulsePins[ULTRASONIC_NO_2].port,ULTRASONIC_arrPulsePins[ULTRASONIC_NO_2].pin);
	  FTM1->CONTROLS[FTM1_CH2].CnSC &= ~(FTM_CnSC_ELSA_MASK | FTM_CnSC_ELSB_MASK);
	  FTM1->CONTROLS[FTM1_CH2].CnSC &= ~(FTM_CnSC_CHIE_MASK | FTM_CnSC_CHF_MASK);
}

static void FTM1_ch4_OC_Callback(void)
{
	  PINS_DRV_ClearPins(ULTRASONIC_arrPulsePins[ULTRASONIC_NO_3].port,ULTRASONIC_arrPulsePins[ULTRASONIC_NO_3].pin);
	  FTM1->CONTROLS[FTM1_CH4].CnSC &= ~(FTM_CnSC_ELSA_MASK | FTM_CnSC_ELSB_MASK);
	  FTM1->CONTROLS[FTM1_CH4].CnSC &= ~(FTM_CnSC_CHIE_MASK | FTM_CnSC_CHF_MASK);
}

static void FTM1_ch6_OC_Callback(void)
{
	  PINS_DRV_ClearPins(ULTRASONIC_arrPulsePins[ULTRASONIC_NO_4].port,ULTRASONIC_arrPulsePins[ULTRASONIC_NO_4].pin);
	  FTM1->CONTROLS[FTM1_CH6].CnSC &= ~(FTM_CnSC_ELSA_MASK | FTM_CnSC_ELSB_MASK);
	  FTM1->CONTROLS[FTM1_CH6].CnSC &= ~(FTM_CnSC_CHIE_MASK | FTM_CnSC_CHF_MASK);
}

