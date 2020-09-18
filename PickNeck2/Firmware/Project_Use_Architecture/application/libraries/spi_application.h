#ifndef SPI_APPLICATION_H__
#define SPI_APPLICATION_H__

#include <stdint.h>
#include "sdk_errors.h"
#include "nrf_spi_mngr.h"

#define FLASH_SPI_INSTANCE_ID 0
#define IMU_SPI_INSTANCE_ID   1

#define FLASH_QUEUE_LENGTH    15
#define IMU_QUEUE_LENGTH      15

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

ret_code_t init_spi0_master(void);
ret_code_t init_spi1_master(void);
void send_imu_spi_message(const uint8_t * tx_data, uint8_t num_tx_byte, uint8_t num_rx_byte, nrf_spi_mngr_callback_end_t end_callback);


#endif // SPI_H__

/** @} */