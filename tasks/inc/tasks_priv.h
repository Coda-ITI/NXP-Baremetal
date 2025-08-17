#ifndef TASKS_PRIV_H
#define TASKS_PRIV_H

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

typedef enum
{
	DOOR_STATE_OPEN = 1U,
	DOOR_STATE_CLOSED = !DOOR_STATE_OPEN,
	DOOR_STATE_COUNT,
} SRV_enuDoorState_t;

typedef enum
{
	GEAR_STATE_NEUTRAL = 0U,
	GEAR_STATE_DRIVING = 1U,
	GEAR_STATE_REVERSE = 2U,
	GEAR_STATE_COUNT,
} SRV_enuGearState_t;

typedef enum
{
	REAR_LEFT_ULTRASONIC = 0U,
	REAR_CENTRE_ULTRASONIC_1 = 1U,
	REAR_CENTRE_ULTRASONIC_2 = 2U,
	REAR_RIGHT_ULTRASONIC = 3U,
	ULTRASONIC_COUNT,
} SRV_enuUltrasonicSensors_t;

typedef enum
{
	READ_DOOR_STATE_PRI = (tskIDLE_PRIORITY + 1U),
	READ_GEAR_STATE_PRI = (tskIDLE_PRIORITY + 1U),
	READ_SPEED_PRI = (tskIDLE_PRIORITY + 1U),
	READ_RPM_PRI = (tskIDLE_PRIORITY + 1U),
	READ_SAFE_DISTANCE_PRI = (tskIDLE_PRIORITY + 1U),
	SEND_DOOR_STATE_PRI = (tskIDLE_PRIORITY + 1U),
	SEND_GEAR_STATE_PRI = (tskIDLE_PRIORITY + 1U),
	SEND_SPEED_PRI = (tskIDLE_PRIORITY + 1U),
	SEND_RPM_PRI = (tskIDLE_PRIORITY + 1U),
	SEND_SAFE_DISTANCE_PRI = (tskIDLE_PRIORITY + 1U),
} SRV_enuTasksPriorities_t;

typedef enum
{
	READ_DOOR_STATE_DELAY = 20,
	READ_GEAR_STATE_DELAY = 10,
	READ_SPEED_DELAY = 10,
	READ_RPM_DELAY = 10,
	READ_SAFE_DISTANCE_DELAY = 50,
	SEND_DOOR_STATE_DELAY = 20,
	SEND_GEAR_STATE_DELAY = 20,
	SEND_SPEED_DELAY = 20,
	SEND_RPM_DELAY = 20,
	SEND_SAFE_DISTANCE_DELAY = 20,
} SRV_enuTasksDelay_t;

typedef struct {
    uint8_t frontRightDoorState : 2;
    uint8_t frontLeftDoorState 	: 2;
    uint8_t rearRightDoorState 	: 2;
    uint8_t rearLeftDoorState 	: 2;
} SRV_structDoorMsg_t;

typedef struct {
    uint8_t gearState;    
} SRV_structGearMsg_t;

typedef struct {
    uint16_t speed;
} SRV_structSpeedMsg_t;

typedef struct {
    uint16_t rpm;
} SRV_structRpmMsg_t;

typedef struct {
    float distance[ULTRASONIC_COUNT];
} SRV_structSafeDistanceMsg_t;

/* queues for storing readings */
QueueHandle_t doorStatesQueue;
QueueHandle_t gearStateQueue;
QueueHandle_t speedQueue;
QueueHandle_t rpmQueue;
QueueHandle_t safeDistanceQueue;

/* tasks for initiating reading */
void SRV_voidReadDoorStateTask(void *pvParameters);
void SRV_voidReadGearStateTask(void *pvParameters);
void SRV_voidReadSpeedTask(void *pvParameters);
void SRV_voidReadRpmTask(void *pvParameters);
void SRV_voidReadSafeDistanceTask(void *pvParameters);

/* tasks for sending readings via CAN */
void SRV_voidSendDoorStateThroughCANTask(void *pvParameters);
void SRV_voidSendGearStateThroughCANTask(void *pvParameters);
void SRV_voidSendSpeedThroughCANTask(void *pvParameters);
void SRV_voidSendRpmThroughCANTask(void *pvParameters);
void SRV_voidSendSafeDistanceThroughCANTask(void *pvParameters);

#endif