#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#include "sdk_project_config.h"

typedef enum
{
	ULTRASONIC_NO_1,
	ULTRASONIC_NO_2,
	ULTRASONIC_NO_3,
	ULTRASONIC_NO_4
}ULTRASONIC_enuNO_t;

typedef enum{
	ULTRASONIC_STATUS_SUCCESS,
	ULTRASONIC_STATUS_INVALID_NO,
	ULTRASONIC_NULL_PTR
}ULTRASONIC_enuStatus_t;

typedef void(*ULTRASONIC_readingCallBack_t)(float);

extern void ULTRASONIC_vInit(void);

extern void ULTRASONIC_vSetCalibrationFactor(float factor);

extern float ULTRASONIC_vGetCalibrationFactor(void);

extern ULTRASONIC_enuStatus_t ULTRASONIC_enuSendPulse(ULTRASONIC_enuNO_t ultrasonic);

extern ULTRASONIC_enuStatus_t ULTRASONIC_enuGetMeasurement(ULTRASONIC_enuNO_t ultrasonic,float* measurement);

extern ULTRASONIC_enuStatus_t ULTRASONIC_enuInstallMeasurementCallBack(ULTRASONIC_enuNO_t ultrasonic,ULTRASONIC_readingCallBack_t callBack);


#endif
