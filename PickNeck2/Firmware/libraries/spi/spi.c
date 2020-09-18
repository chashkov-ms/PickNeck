#include <string.h>
#include "spi.h"
#include "pin_config.h"
#include "nrf_log.h"


#define SPI_INSTANCE  1 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /**< SPI instance. */
static uint8_t       spi_tx_buf[1] = {0};           /**< TX buffer. */
static uint8_t       spi_rx_buf[2];    /**< RX buffer. */
static const uint8_t m_length = sizeof(spi_tx_buf);        /**< Transfer length. */
static const uint8_t mr_length = sizeof(spi_rx_buf);        /**< Transfer length. */

/**@brief Init SPI function.
 */
void init_spi(void)
{

    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = IMU_CS_PIN;
    spi_config.miso_pin = IMU_MISO_PIN;
    spi_config.mosi_pin = IMU_MOSI_PIN;
    spi_config.sck_pin  = IMU_SCK_PIN;
    spi_config.frequency = NRF_SPI_FREQ_1M;
    memset(spi_rx_buf, 1, m_length+1);

    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));

    NRF_LOG_INFO("SPI example started.");

}

/**@brief Write SPI function.
 */
void write_spi(void)
{
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, spi_tx_buf, m_length, NULL, NULL));
}

/**@brief Read SPI function.
 */
uint8_t read_spi(void)
{
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, spi_tx_buf, m_length, spi_rx_buf, mr_length));
    return (uint8_t)spi_rx_buf[1];
}

/**@brief 
 */
void get_spi_rx(uint8_t * rx_data)
{
    memcpy(rx_data, spi_rx_buf, mr_length);
}

/**@brief 
 */
void set_spi_tx(uint8_t * tx_data, uint8_t num_byte)
{
    memcpy(spi_tx_buf, tx_data, num_byte);
}


