#include <string.h>
#include "spi_application.h"
#include "pin_config.h"
#include "nrf_log.h"
#include "nrf_spi_mngr.h"

NRF_SPI_MNGR_DEF(imu_nrf_spi_mngr, IMU_QUEUE_LENGTH, IMU_SPI_INSTANCE_ID);
NRF_SPI_MNGR_DEF(flash_nrf_spi_mngr, FLASH_QUEUE_LENGTH, FLASH_SPI_INSTANCE_ID);

static uint16_t flash_read[9] = {1, 1, 1};
static nrf_spi_mngr_transfer_t flash_transfer_cmd;
static nrf_spi_mngr_transaction_t flash_transaction_cmd;


static uint8_t imu_read[(3+3)*2+1] = {1, 1, 1};
static nrf_spi_mngr_transfer_t imu_transfer_cmd;
static nrf_spi_mngr_transaction_t imu_transaction_cmd;


ret_code_t init_spi0_master(void)
{
    // SPI0 (with transaction manager) initialization.
    nrf_drv_spi_config_t const m_master0_config =
    {
        .sck_pin        = FLASH_SCK_PIN,
        .mosi_pin       = FLASH_MOSI_PIN,
        .miso_pin       = FLASH_MISO_PIN,
        .ss_pin         = FLASH_CS_PIN,
        .irq_priority   = APP_IRQ_PRIORITY_LOWEST,
        .orc            = 0xFF,
        .frequency      = NRF_DRV_SPI_FREQ_1M,
        .mode           = NRF_DRV_SPI_MODE_0,
        .bit_order      = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST
    };
    return nrf_spi_mngr_init(&flash_nrf_spi_mngr, &m_master0_config);
}


void send_flash_spi_message(const uint8_t * tx_data, uint8_t num_tx_byte, uint8_t num_rx_byte, nrf_spi_mngr_callback_end_t callback)
{

    flash_transfer_cmd.p_tx_data = tx_data;
    flash_transfer_cmd.tx_length = num_tx_byte;
    flash_transfer_cmd.p_rx_data = (uint8_t *)imu_read;
    flash_transfer_cmd.rx_length = num_rx_byte+1;

    flash_transaction_cmd.begin_callback      = NULL;
    flash_transaction_cmd.end_callback        = callback;
    flash_transaction_cmd.p_user_data         = &flash_read[1];
    flash_transaction_cmd.p_transfers         = &flash_transfer_cmd;
    flash_transaction_cmd.number_of_transfers = 1;
    flash_transaction_cmd.p_required_spi_cfg  = NULL;

    APP_ERROR_CHECK(nrf_spi_mngr_schedule(&flash_nrf_spi_mngr, &flash_transaction_cmd));
}


ret_code_t init_spi1_master(void)
{
    // SPI1 (with transaction manager) initialization.
    nrf_drv_spi_config_t const m_master1_config =
    {
        .sck_pin        = IMU_SCK_PIN,
        .mosi_pin       = IMU_MOSI_PIN,
        .miso_pin       = IMU_MISO_PIN,
        .ss_pin         = IMU_CS_PIN,
        .irq_priority   = APP_IRQ_PRIORITY_LOWEST,
        .orc            = 0xFF,
        .frequency      = NRF_DRV_SPI_FREQ_1M,
        .mode           = NRF_DRV_SPI_MODE_3,
        .bit_order      = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST
    };
    return nrf_spi_mngr_init(&imu_nrf_spi_mngr, &m_master1_config);
}


void send_imu_spi_message(const uint8_t * tx_data, uint8_t num_tx_byte, uint8_t num_rx_byte, nrf_spi_mngr_callback_end_t callback)
{

    imu_transfer_cmd.p_tx_data = tx_data;
    imu_transfer_cmd.tx_length = num_tx_byte;
    imu_transfer_cmd.p_rx_data = (uint8_t *)imu_read;
    imu_transfer_cmd.rx_length = num_rx_byte+1;

    imu_transaction_cmd.begin_callback      = NULL;
    imu_transaction_cmd.end_callback        = callback;
    imu_transaction_cmd.p_user_data         = &imu_read[1];
    imu_transaction_cmd.p_transfers         = &imu_transfer_cmd;
    imu_transaction_cmd.number_of_transfers = 1;
    imu_transaction_cmd.p_required_spi_cfg  = NULL;

    APP_ERROR_CHECK(nrf_spi_mngr_schedule(&imu_nrf_spi_mngr, &imu_transaction_cmd));
}

