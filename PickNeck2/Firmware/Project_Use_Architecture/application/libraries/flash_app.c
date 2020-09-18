#include <string.h>
#include "flash_app.h"
#include "nrf_log.h"
#include "pin_config.h"


static uint8_t NRF_SPI_MNGR_BUFFER_LOC_IND command[256] = {
    RDID,
    FAST_READ,
    DUMMY_BYTE,
    (CONFIG_ADDRESS>>16)&0xFF,
    (CONFIG_ADDRESS>>8)&0xFF,
    (CONFIG_ADDRESS>>0)&0xFF
};

    static uint8_t flash_read[260] = {1};

    static nrf_spi_mngr_transfer_t transfer_cmd[] =
    {
        NRF_SPI_MNGR_TRANSFER(&command[0], 1, flash_read, 4)
    };
    static nrf_spi_mngr_transaction_t transaction_cmd[] =
    {
        {
            .begin_callback      = NULL,
            .end_callback        = flash_is_answer,
            .p_user_data         = &flash_read[1],
            .p_transfers         = &transfer_cmd[0],
            .number_of_transfers = 1,
            .p_required_spi_cfg  = NULL
        }
    };

NRF_SPI_MNGR_DEF(flash_nrf_spi_mngr, FLASH_QUEUE_LENGTH, FLASH_SPI_INSTANCE_ID);

/**@brief Init FLASH function.
 */
void init_flash(void)
{
    ret_code_t ret_val;

    init_spi0_master();
    ret_val = nrf_spi_mngr_schedule(&flash_nrf_spi_mngr, &transaction_cmd[0]);
    return;
}


void flash_is_answer (ret_code_t result, void * p_user_data)
{
    
    uint8_t answer[3] = {0};
    memcpy(answer, p_user_data, 3);
    if (answer[1] == 0 || answer[2] == 0 || answer[3] == 0 )
    {
        NRF_LOG_ERROR("FLASH is not answer!!");
        return;
    }
    NRF_LOG_INFO("FLASH is answering");
    return;
}


static ret_code_t init_spi0_master(void)
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


void start_configuration (ret_code_t result, void * p_user_data)
{
    uint8_t answer[11] = {0};
    config_data start_config_data;

    memcpy(answer, p_user_data, 11);
    if (answer[10] != 0xFF)
    {
        return;
    }

    start_config_data.time = 0;             // time in 125 msec steps. 1 sec = 8 steps.
    start_config_data.date = 7;
    start_config_data.mounth = 9;
    start_config_data.year = 2020;
    start_config_data.limit_angle = 5;      // 5 deg
    start_config_data.reaction_time = 5;    // 5 sec
    start_config_data.vibration_level = 2;  // сильная

    write_flash(CONFIG_ADDRESS, &start_config_data, 11);
    return;
}


void write_flash(uint32_t address, void * p_user_data, uint8_t num_byte)
{
    ret_code_t ret_val;

    command[0] = WREN;
    command[1] = PP;
    command[2] = (address>>16) & 0xFF;
    command[3] = (address>>8) & 0xFF;
    command[4] = (address>>0) & 0xFF;

    memcpy(&command[5], p_user_data, num_byte);
    
    transfer_cmd[0].p_tx_data = &command[0];
    transfer_cmd[0].tx_length = 5;
    transfer_cmd[0].p_rx_data = NULL;
    transfer_cmd[0].rx_length = 0;

    transaction_cmd[0].begin_callback      = NULL;
    transaction_cmd[0].end_callback        = flash_is_answer;
    transaction_cmd[0].p_user_data         = &flash_read[1];
    transaction_cmd[0].p_transfers         = &transfer_cmd[0];
    transaction_cmd[0].number_of_transfers = 1;
    transaction_cmd[0].p_required_spi_cfg  = NULL;

    ret_val = nrf_spi_mngr_schedule(&flash_nrf_spi_mngr, &transaction_cmd[0]);
    asm("nop");
}

