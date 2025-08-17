/*
 * tasks.c
 *
 *  Created on: Aug 17, 2025
 *      Author: Nemesis
 */

#include "tasks_priv.h"
#include "tasks.h"

void SRV_voidRTOSMain(void)
{
	// board setup

    // creating queues
    doorStatesQueue		= xQueueCreate(1, sizeof(SRV_structDoorMsg_t));
    gearStateQueue   	= xQueueCreate(1, sizeof(SRV_structGearMsg_t));
    speedQueue    		= xQueueCreate(1, sizeof(SRV_structSpeedMsg_t));
    rpmQueue 			= xQueueCreate(1, sizeof(SRV_structRpmMsg_t));
    safeDistanceQueue  	= xQueueCreate(1, sizeof(SRV_structSafeDistanceMsg_t));

    if (
    	(doorStatesQueue != NULL) 	&&
    	(gearStateQueue != NULL) 	&&
    	(speedQueue != NULL) 		&&
    	(rpmQueue != NULL) 			&&
    	(safeDistanceQueue != NULL)
    )
    {
    	// creating tasks
    	xTaskCreate(SRV_voidReadDoorStateTask, "ReadDoorState", 256, NULL, READ_DOOR_STATE_PRI, NULL);
    	xTaskCreate(SRV_voidReadGearStateTask, "ReadGearState", 256, NULL, READ_GEAR_STATE_PRI, NULL);
    	xTaskCreate(SRV_voidReadSpeedTask, "ReadSpeed", 256, NULL, READ_SPEED_PRI, NULL);
    	xTaskCreate(SRV_voidReadRpmTask, "ReadRpm", 256, NULL, READ_RPM_PRI, NULL);
    	xTaskCreate(SRV_voidReadSafeDistanceTask, "ReadSafeDistance", 256, NULL, READ_SAFE_DISTANCE_PRI, NULL);

    	xTaskCreate(SRV_voidSendDoorStateThroughCANTask, "SendDoorStateThroughCAN", 512, NULL, SEND_DOOR_STATE_PRI, NULL);
    	xTaskCreate(SRV_voidSendGearStateThroughCANTask, "SendGearStateThroughCAN", 512, NULL, SEND_GEAR_STATE_PRI, NULL);
    	xTaskCreate(SRV_voidSendSpeedThroughCANTask,"SendSpeedThroughCAN", 512, NULL, SEND_SPEED_PRI, NULL);
    	xTaskCreate(SRV_voidSendRpmThroughCANTask, "SendRpmThroughCAN", 512, NULL, SEND_RPM_PRI, NULL);
    	xTaskCreate(SRV_voidSendSafeDistanceThroughCANTask, "SendSafeDistanceThroughCAN", 512, NULL, SEND_SAFE_DISTANCE_PRI, NULL);

    	// starting scheduler
    	vTaskStartScheduler();
    }

    for(;;);
}

// ========================= TASK IMPLEMENTATIONS =========================
void SRV_voidReadDoorStateTask(void *pvParameters)
{
    for(;;)
    {
    	// getDoorStateAsync();
        vTaskDelay(pdMS_TO_TICKS(20));

        /*
         * 	in door state callback function
         	SRV_structDoorMsg_t msg;
        	msg.frontLeftDoorState;
        	msg.frontRightDoorState;
        	msg.rearLeftDoorState;
        	msg.rearRightDoorState;
        	xQueueOverwrite(doorStatesQueue, &msg);
         */
    }
}

void SRV_voidReadGearStateTask(void *pvParameters)
{

    for(;;)
    {
    	// getGearStateAsync();
        vTaskDelay(pdMS_TO_TICKS(10));

        /*
         * 	in gear state callback function
    		SRV_structGearMsg_t msg;
    		msg.gearState
        	xQueueOverwrite(gearQueue, &msg);
         */
    }
}

void SRV_voidReadSpeedTask(void *pvParameters)
{
    for(;;)
    {
    	// getSpeedAsync();
        vTaskDelay(pdMS_TO_TICKS(10));
        /*
         * 	in speed adc callback function
    		SRV_structSpeedMsg_t msg;
    		msg.speed
        	xQueueOverwrite(speedQueue, &msg);
         */
    }
}

void SRV_voidReadRpmTask(void *pvParameters)
{
    for(;;)
    {
    	// getRpmAsync();
        vTaskDelay(pdMS_TO_TICKS(50));
        /*
         * 	in rpm adc callback function
    		SRV_structRPMMsg_t msg;
    		msg.rpm
        	xQueueOverwrite(rpmQueue, &msg);
         */
    }
}

void SRV_voidReadSafeDistanceTask(void *pvParameters)
{
    for(;;)
    {
    	// getSafeDistanceAsync();
        vTaskDelay(pdMS_TO_TICKS(50));
        /*
         * 	in ultrasonic callback function
    		SRV_structSafeDistanceMsg_t msg;
    		for (int i = REAR_LEFT_ULTRASONIC; i < ULTRASONIC_COUNT; i++)
				msg.distance[i] = reading[i];
        	xQueueOverwrite(safeDistanceQueue, &msg);
         */
    }
}

void SRV_voidSendDoorStateThroughCANTask(void *pvParameters)
{
	SRV_structDoorMsg_t door;
    for(;;)
    {
    	// peek into latest value
    	xQueuePeek(doorStatesQueue, &door, 0);

    	// serialize/construct CAN frame

    	// send CAN message async

    	// delay
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void SRV_voidSendGearStateThroughCANTask(void *pvParameters)
{
	SRV_structGearMsg_t gearState;
    for(;;)
    {
    	// peek into latest value
    	xQueuePeek(gearStateQueue, &gearState, 0);

    	// serialize/construct CAN frame

    	// send CAN message async

    	// delay
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void SRV_voidSendSpeedThroughCANTask(void *pvParameters)
{
	SRV_structSpeedMsg_t speed;
    for(;;)
    {
    	// peek into latest value
    	xQueuePeek(speedQueue, &speed, 0);

    	// serialize/construct CAN frame

    	// send CAN message async

    	// delay
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void SRV_voidSendRpmThroughCANTask(void *pvParameters)
{
	SRV_structRpmMsg_t rpm;
    for(;;)
    {
    	// peek into latest value
    	xQueuePeek(rpmQueue, &rpm, 0);

    	// serialize/construct CAN frame

    	// send CAN message async

    	// delay
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void SRV_voidSendSafeDistanceThroughCANTask(void *pvParameters)
{
	SRV_structSafeDistanceMsg_t safeDistance;
    for(;;)
    {
    	// peek into latest value
    	xQueuePeek(safeDistanceQueue, &safeDistance, 0);

    	// serialize/construct CAN frame

    	// send CAN message async

    	// delay
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}