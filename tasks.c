/*
 * tasks.c
 *
 *  Created on: Aug 17, 2025
 *      Author: Nemesis
 */

#include "tasks_priv.h"
#include "can_hal.h"
#include "ultrasonic.h"
#include "interrupt_manager.h"
#include "FreeRTOSConfig.h"
#include "adc_hal.h"



static void Ultrasonic1_callBack(float val);
static void Ultrasonic2_callBack(float val);
static void Ultrasonic3_callBack(float val);
static void Ultrasonic4_callBack(float val);

static void Can_rxCallBack(CAN_HAL_enuCAN_RX_MB_t rx_mb_id);

void board_init(void)
{

    CLOCK_DRV_Init(&clockMan1_InitConfig0);

	INT_SYS_DisableIRQGlobal();

	status_t error;

	error = PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0, g_pin_mux_InitConfigArr0);
	DEV_ASSERT(error == STATUS_SUCCESS);

	ULTRASONIC_enuInstallMeasurementCallBack(ULTRASONIC_NO_1, Ultrasonic1_callBack);
	ULTRASONIC_enuInstallMeasurementCallBack(ULTRASONIC_NO_2, Ultrasonic2_callBack);
	ULTRASONIC_enuInstallMeasurementCallBack(ULTRASONIC_NO_3, Ultrasonic3_callBack);
	ULTRASONIC_enuInstallMeasurementCallBack(ULTRASONIC_NO_4, Ultrasonic4_callBack);

	ULTRASONIC_vInit();

	CAN_HAL_vInit();

	CAN_HAL_vInstallCallBack(Can_rxCallBack);

	ADC_HAL_vInit();

	INT_SYS_EnableIRQGlobal();
}


void SRV_voidRTOSMain(void)
{
	// board setup

    //creating queues

    speedQueue    		= xQueueCreate(1, sizeof(SRV_structSpeedMsg_t));
    rpmQueue 			= xQueueCreate(1, sizeof(SRV_structRpmMsg_t));

//    if (
//    	(doorStatesQueue != NULL) 	&&
//    	(gearStateQueue != NULL) 	&&
//    	(speedQueue != NULL) 		&&
//    	(rpmQueue != NULL) 			&&
//    	(safeDistanceQueue != NULL)
//    )
//    {
//    	// creating tasks
//
	    xTaskCreate(SRV_voidReadSafeDistanceTask, "ReadSafeDistance", 2048, NULL, READ_SAFE_DISTANCE_PRI, NULL);
    	xTaskCreate(SRV_voidReadSpeedTask, "ReadSpeed", 256, NULL, READ_SPEED_PRI, NULL);
    	xTaskCreate(SRV_voidReadRpmTask, "ReadRpm", 256, NULL, READ_RPM_PRI, NULL);
    	xTaskCreate(SRV_voidSendDoorStateThroughCANTask, "SendDoorStateThroughCAN", 512, NULL, SEND_DOOR_STATE_PRI, NULL);
    	xTaskCreate(SRV_voidSendGearStateThroughCANTask, "SendGearStateThroughCAN", 512, NULL, SEND_GEAR_STATE_PRI, NULL);
    	xTaskCreate(SRV_voidSendSpeedThroughCANTask,"SendSpeedThroughCAN", 512, NULL, SEND_SPEED_PRI, NULL);
    	xTaskCreate(SRV_voidSendRpmThroughCANTask, "SendRpmThroughCAN", 512, NULL, SEND_RPM_PRI, NULL);

    	// starting scheduler
    	vTaskStartScheduler();
//    }

    for(;;);
}



void SRV_voidReadSpeedTask(void *pvParameters)
{
    for(;;)
    {
        vTaskDelay(pdMS_TO_TICKS(READ_SPEED_DELAY));
        SRV_structSpeedMsg_t msg;
        msg.speed = ADC_HAL_u16GetPot1ReadingBlocking();
    	xQueueOverwrite(speedQueue, &msg);
    }
}

void SRV_voidReadRpmTask(void *pvParameters)
{
    for(;;)
    {
        vTaskDelay(pdMS_TO_TICKS(READ_RPM_DELAY));
        SRV_structRpmMsg_t msg;
        msg.rpm = ADC_HAL_u16GetPot2ReadingBlocking();
        xQueueOverwrite(rpmQueue,&msg);
    }
}

void SRV_voidReadSafeDistanceTask(void *pvParameters)
{
    for(;;)
    {
    	ULTRASONIC_enuSendPulse(ULTRASONIC_NO_1);
    	ULTRASONIC_enuSendPulse(ULTRASONIC_NO_2);
    	ULTRASONIC_enuSendPulse(ULTRASONIC_NO_3);
    	ULTRASONIC_enuSendPulse(ULTRASONIC_NO_4);

        vTaskDelay(pdMS_TO_TICKS(READ_SAFE_DISTANCE_DELAY));
    }
}

void SRV_voidSendDoorStateThroughCANTask(void *pvParameters)
{
    for(;;)
    {
    	uint32_t doorsStatus1 = PINS_DRV_ReadPins(PTE);

    	uint32_t doorsStatus2 = PINS_DRV_ReadPins(PTB);

    	uint32_t doorsStatus3 = PINS_DRV_ReadPins(PTA);

    	uint8_t doorsStatusData = ((doorsStatus1 & (1 << 11)) >> 11) | ((doorsStatus1 & (1 << 10))>>9) | ((doorsStatus2 & (1 << 13)) >> 11)
    			| ((doorsStatus3 & (1 << 6)) >> 3);

        flexcan_data_info_t txInfo = {
            .msg_id_type = FLEXCAN_MSG_ID_STD,
            .data_length = 1,
            .fd_enable = false,
            .is_remote = false,
            .enable_brs = false
        };

        uint8_t txData[1] = {doorsStatusData};

        FLEXCAN_DRV_Send(INST_FLEXCAN_CONFIG_1, CAN_HAL_DOORSTATES_TX_MB, &txInfo, CAN_HAL_DOORSTATES_CANID, txData);

        vTaskDelay(pdMS_TO_TICKS(SEND_DOOR_STATE_DELAY));
    }
}

void SRV_voidSendGearStateThroughCANTask(void *pvParameters)
{
    for(;;)
    {
    	uint32_t pinsMask = (1 << 17) | (1 << 18);

    	uint32_t pinsVal = PINS_DRV_ReadPins(PTB);

    	uint32_t pinsState = pinsMask & pinsVal;

    	uint8_t gearState = 0;

    	switch(pinsState)
    	{
    		case 0:
    			gearState = 2;
    			break;

    		case (1 << 17):
    			gearState = 1;
				break;
    		case (1 << 18):
				gearState = 3;
				break;
    		default:
    			break;

    	}

        flexcan_data_info_t txInfo = {
            .msg_id_type = FLEXCAN_MSG_ID_STD,
            .data_length = 1,
            .fd_enable = false,
            .is_remote = false,
            .enable_brs = false
        };


        uint8_t txData[1] = {gearState};



        FLEXCAN_DRV_Send(INST_FLEXCAN_CONFIG_1, CAN_HAL_GEAR_SATE_TX_MB, &txInfo, CAN_HAL_GEAR_SATE_CANID, txData);

        vTaskDelay(pdMS_TO_TICKS(SEND_GEAR_STATE_DELAY));
    }
}

void SRV_voidSendSpeedThroughCANTask(void *pvParameters)
{
    SRV_structSpeedMsg_t speedSt;
    for(;;)
    {
        xQueuePeek(speedQueue, &speedSt, 0);

        uint16_t speedVal = speedSt.speed;

        flexcan_data_info_t txInfo = {
            .msg_id_type = FLEXCAN_MSG_ID_STD,
            .data_length = 2,
            .fd_enable = false,
            .is_remote = false,
            .enable_brs = false
        };

        uint8_t txData[2] = {
            (uint8_t)(speedVal & 0xFF),
            (uint8_t)((speedVal >> 8) & 0xFF)
        };

        FLEXCAN_DRV_Send(INST_FLEXCAN_CONFIG_1, CAN_HAL_SPEED_TX_MB, &txInfo,CAN_HAL_SPEED_CANID, txData);

        vTaskDelay(pdMS_TO_TICKS(SEND_SPEED_DELAY));
    }
}

void SRV_voidSendRpmThroughCANTask(void *pvParameters)
{
	SRV_structRpmMsg_t rpmSt;
    for(;;)
    {
    	// peek into latest value
    	xQueuePeek(rpmQueue, &rpmSt, 0);

    	union{
			uint16_t rpmVal;
			uint8_t txData[2];
		}rpmData;

		rpmData.rpmVal = rpmSt.rpm;

		flexcan_data_info_t txInfo = {
			.msg_id_type = FLEXCAN_MSG_ID_STD,
			.data_length = 2,
			.fd_enable = false,
			.is_remote = false,
			.enable_brs = false
		};

		FLEXCAN_DRV_Send(INST_FLEXCAN_CONFIG_1, CAN_HAL_RPM_TX_MB, &txInfo, CAN_HAL_RPM_CANID, rpmData.txData);

        vTaskDelay(pdMS_TO_TICKS(SEND_RPM_DELAY));
    }
}


void Ultrasonic1_callBack(float val)
{
    flexcan_data_info_t txInfo = {
        .msg_id_type = FLEXCAN_MSG_ID_STD,
        .data_length = 4,
        .fd_enable = false,
        .is_remote = false,
        .enable_brs = false
    };

    union{
    	float value;
		uint8_t arr[4];
    }float_converter;

    float_converter.value = val;

    uint8_t txData[4] = {0};

    for (int i = 0; i < 4; i++) {
        txData[i] = float_converter.arr[i];
    }

   FLEXCAN_DRV_Send(INST_FLEXCAN_CONFIG_1, CAN_HAL_ULTRASONIC1_TX_MB, &txInfo, CAN_HAL_ULTRASONIC1_CANID, txData);
}

void Ultrasonic2_callBack(float val)
{
    flexcan_data_info_t txInfo = {
        .msg_id_type = FLEXCAN_MSG_ID_STD,
        .data_length = 4,
        .fd_enable = false,
        .is_remote = false,
        .enable_brs = false
    };

    union{
    	float value;
		uint8_t arr[4];
    }float_converter;

    float_converter.value = val;

    uint8_t txData[4] = {0};

    for (int i = 0; i < 4; i++) {
        txData[i] = float_converter.arr[i];
    }

    FLEXCAN_DRV_Send(INST_FLEXCAN_CONFIG_1, CAN_HAL_ULTRASONIC2_TX_MB, &txInfo, CAN_HAL_ULTRASONIC2_CANID, txData);

}

void Ultrasonic3_callBack(float val)
{
    flexcan_data_info_t txInfo = {
        .msg_id_type = FLEXCAN_MSG_ID_STD,
        .data_length = 4,
        .fd_enable = false,
        .is_remote = false,
        .enable_brs = false
    };

    union{
    	float value;
		uint8_t arr[4];
    }float_converter;

    float_converter.value = val;

    uint8_t txData[4] = {0};

    for (int i = 0; i < 4; i++) {
        txData[i] = float_converter.arr[i];
    }

    FLEXCAN_DRV_Send(INST_FLEXCAN_CONFIG_1, CAN_HAL_ULTRASONIC3_TX_MB, &txInfo, CAN_HAL_ULTRASONIC3_CANID, txData);
}

void Ultrasonic4_callBack(float val)
{
    flexcan_data_info_t txInfo = {
        .msg_id_type = FLEXCAN_MSG_ID_STD,
        .data_length = 4,
        .fd_enable = false,
        .is_remote = false,
        .enable_brs = false
    };

    union{
    	float value;
		uint8_t arr[4];
    }float_converter;

    float_converter.value = val;

    uint8_t txData[4] = {0};

    for (int i = 0; i < 4; i++) {
        txData[i] = float_converter.arr[i];
    }

    FLEXCAN_DRV_Send(INST_FLEXCAN_CONFIG_1, CAN_HAL_ULTRASONIC4_TX_MB, &txInfo, CAN_HAL_ULTRASONIC4_CANID, txData);
}

void Can_rxCallBack(CAN_HAL_enuCAN_RX_MB_t rx_mb_id)
{
	if(rx_mb_id == CAN_HAL_REQ_SPEED_RX_MB)
	{

	}

	if(rx_mb_id == CAN_HAL_REQ_RPM_RX_MB)
	{

	}

	if(rx_mb_id == CAN_HAL_REQ_ULTRASONIC1_RX_MB)
	{

	}

	if(rx_mb_id == CAN_HAL_REQ_ULTRASONIC2_RX_MB)
	{

	}

	if(rx_mb_id == CAN_HAL_REQ_ULTRASONIC3_RX_MB)
	{

	}

	if(rx_mb_id == CAN_HAL_REQ_ULTRASONIC4_RX_MB)
	{

	}

	if(rx_mb_id ==CAN_HAL_REQ_DOORSTATES_RX_MB)
	{

	}

	if(rx_mb_id == CAN_HAL_REQ_GEARSTATE_RX_MB)
	{

	}
}

