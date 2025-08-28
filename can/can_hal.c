/*
 * can_hal.c
 *
 *  Created on: Aug 19, 2025
 *      Author: adham
 */

#include "can_hal.h"

static CAN_HAL_rxCallBack_t CAN_HAL_rxCallBack = NULL;

static void CAN_HAL_callBack(uint8_t instance, flexcan_event_type_t eventType, uint32_t buffIdx, flexcan_state_t *driverState);


void CAN_HAL_vInit(void)
{
    FLEXCAN_DRV_GetDefaultConfig(&flexcanInitConfig0);
    flexcanInitConfig0.is_rx_fifo_needed = false;
    flexcanInitConfig0.flexcanMode = FLEXCAN_NORMAL_MODE;
    FLEXCAN_DRV_Init(INST_FLEXCAN_CONFIG_1, &flexcanState0, &flexcanInitConfig0);

    flexcan_data_info_t txInfo;
    txInfo.fd_enable = false;
    txInfo.is_remote = false;
    txInfo.enable_brs = false;
    txInfo.msg_id_type = FLEXCAN_MSG_ID_STD;

    txInfo.data_length = 4;

    FLEXCAN_DRV_ConfigTxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_ULTRASONIC1_TX_MB, &txInfo, CAN_HAL_ULTRASONIC1_CANID);
    FLEXCAN_DRV_ConfigTxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_ULTRASONIC2_TX_MB, &txInfo, CAN_HAL_ULTRASONIC2_CANID);
    FLEXCAN_DRV_ConfigTxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_ULTRASONIC3_TX_MB, &txInfo, CAN_HAL_ULTRASONIC3_CANID);
    FLEXCAN_DRV_ConfigTxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_ULTRASONIC4_TX_MB, &txInfo, CAN_HAL_ULTRASONIC4_CANID);

    txInfo.data_length = 2;
    FLEXCAN_DRV_ConfigTxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_SPEED_TX_MB, &txInfo, CAN_HAL_SPEED_CANID);

    FLEXCAN_DRV_ConfigTxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_RPM_TX_MB, &txInfo, CAN_HAL_RPM_CANID);

    txInfo.data_length = 1;
    FLEXCAN_DRV_ConfigTxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_DOORSTATES_TX_MB, &txInfo, CAN_HAL_DOORSTATES_CANID);

    FLEXCAN_DRV_ConfigTxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_GEAR_SATE_TX_MB, &txInfo, CAN_HAL_GEAR_SATE_CANID);

    txInfo.data_length = 2;
    FLEXCAN_DRV_ConfigTxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_REQ_SPEED_TX_MB, &txInfo, CAN_HAL_REQ_ULTRASONIC1_CANID);
    FLEXCAN_DRV_ConfigTxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_REQ_RPM_TX_MB, &txInfo, CAN_HAL_REQ_RPM_CANID);

    txInfo.data_length = 4;
    FLEXCAN_DRV_ConfigTxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_REQ_ULTRASONIC1_TX_MB, &txInfo, CAN_HAL_REQ_ULTRASONIC1_CANID);
    FLEXCAN_DRV_ConfigTxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_REQ_ULTRASONIC2_TX_MB, &txInfo, CAN_HAL_REQ_ULTRASONIC2_CANID);
    FLEXCAN_DRV_ConfigTxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_REQ_ULTRASONIC3_TX_MB, &txInfo, CAN_HAL_REQ_ULTRASONIC3_CANID);
    FLEXCAN_DRV_ConfigTxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_REQ_ULTRASONIC4_TX_MB, &txInfo, CAN_HAL_REQ_ULTRASONIC4_CANID);

    txInfo.data_length = 1;
    FLEXCAN_DRV_ConfigTxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_REQ_DOORSTATES_TX_MB, &txInfo, CAN_HAL_REQ_DOORSTATES_CANID);
    FLEXCAN_DRV_ConfigTxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_REQ_GEARSTATE_TX_MB, &txInfo, CAN_HAL_REQ_GEARSTATE_CANID);


    flexcan_data_info_t rxInfo = txInfo;
    rxInfo.data_length = 0;
    FLEXCAN_DRV_ConfigRxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_REQ_SPEED_RX_MB, &rxInfo, CAN_HAL_REQ_SPEED_CANID);
    FLEXCAN_DRV_ConfigRxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_REQ_RPM_RX_MB, &rxInfo, CAN_HAL_REQ_RPM_CANID);
    FLEXCAN_DRV_ConfigRxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_REQ_ULTRASONIC1_RX_MB, &rxInfo, CAN_HAL_REQ_ULTRASONIC1_CANID);
    FLEXCAN_DRV_ConfigRxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_REQ_ULTRASONIC2_RX_MB, &rxInfo, CAN_HAL_REQ_ULTRASONIC2_CANID);
    FLEXCAN_DRV_ConfigRxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_REQ_ULTRASONIC3_RX_MB, &rxInfo, CAN_HAL_REQ_ULTRASONIC3_CANID);
    FLEXCAN_DRV_ConfigRxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_REQ_ULTRASONIC4_RX_MB, &rxInfo, CAN_HAL_REQ_ULTRASONIC4_CANID);
    FLEXCAN_DRV_ConfigRxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_REQ_DOORSTATES_RX_MB, &rxInfo, CAN_HAL_REQ_DOORSTATES_CANID);
    FLEXCAN_DRV_ConfigRxMb(INST_FLEXCAN_CONFIG_1, CAN_HAL_REQ_GEARSTATE_RX_MB, &rxInfo, CAN_HAL_REQ_GEARSTATE_CANID);

    FLEXCAN_DRV_InstallEventCallback(INST_FLEXCAN_CONFIG_1, CAN_HAL_callBack, NULL);
}

void CAN_HAL_vInstallCallBack(CAN_HAL_rxCallBack_t callBack)
{
	CAN_HAL_rxCallBack = callBack;
}

static void CAN_HAL_callBack(uint8_t instance, flexcan_event_type_t eventType, uint32_t buffIdx, flexcan_state_t *driverState)
{
	if((CAN_HAL_rxCallBack != NULL) && (eventType == FLEXCAN_EVENT_RX_COMPLETE))
	{
		CAN_HAL_rxCallBack((CAN_HAL_enuCAN_RX_MB_t)buffIdx);
	}
}




