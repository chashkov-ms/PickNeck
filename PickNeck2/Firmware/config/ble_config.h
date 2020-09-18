#ifndef BLE_CONFIG_H
#define BLE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define OPCODE_LENGTH        1
#define HANDLE_LENGTH        2

/**@brief   Maximum length of data (in bytes) that can be transmitted to the peer by the other service module. */
#if defined(NRF_SDH_BLE_GATT_MAX_MTU_SIZE) && (NRF_SDH_BLE_GATT_MAX_MTU_SIZE != 0)
	#define BLE_MAX_DATA_LEN (NRF_SDH_BLE_GATT_MAX_MTU_SIZE - OPCODE_LENGTH - HANDLE_LENGTH)
#else
	#define BLE_MAX_DATA_LEN (BLE_GATT_MTU_SIZE_DEFAULT - OPCODE_LENGTH - HANDLE_LENGTH)
	#warning NRF_SDH_BLE_GATT_MAX_MTU_SIZE is not defined.
#endif



/**< Used vendor specific UUID - ADFB0001-A602-11EA-BB37-0242AC130002. */
#define CMD_BASE_UUID { {0x02, 0x00, 0x13, 0xAC, 0x42, 0x02, 0x37, 0xBB, 0xEA, 0x11, 0x02, 0xA6, 0x00, 0x00, 0xFB, 0xAD} }

#define BLE_UUID_CMD_SERVICE 0x0001 /**< The UUID of the Command Service. */

#define BLE_UUID_CMD_TX_CHARACTERISTIC 0x0003               /**< The UUID of the TX Characteristic. */
#define BLE_UUID_CMD_RX_CHARACTERISTIC 0x0002               /**< The UUID of the RX Characteristic. */



 /**< Used vendor specific UUID - ADFB0004-A602-11EA-BB37-0242AC130002. */
#define IMU_BASE_UUID { {0x02, 0x00, 0x13, 0xAC, 0x42, 0x02, 0x37, 0xBB, 0xEA, 0x11, 0x02, 0xA6, 0x00, 0x00, 0xFC, 0xAD} }

#define BLE_UUID_IMU_SERVICE 0x0004 /**< The UUID of the IMU Service. */

#define BLE_UUID_IMU_ACC_CHARACTERISTIC 0x0005
#define BLE_UUID_IMU_GYR_CHARACTERISTIC 0x0006
#define BLE_UUID_IMU_ANG_CHARACTERISTIC 0x0007
#define BLE_UUID_IMU_PED_CHARACTERISTIC 0x0008


#ifdef __cplusplus
}
#endif

#endif // BLE_CONFIG_H
