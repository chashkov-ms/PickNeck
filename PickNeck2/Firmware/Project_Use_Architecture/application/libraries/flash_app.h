#ifndef FLASH_APP_H
#define FLASH_APP_H

#include <stdbool.h>
#include "sdk_errors.h"
#include "nrf_spi_mngr.h"

#define FLASH_QUEUE_LENGTH     15
#define FLASH_SPI_INSTANCE_ID  0

#define RDID              ((uint8_t)0x9F)
#define FAST_READ         ((uint8_t)0x0B)
#define PP                ((uint8_t)0x02)
#define DUMMY_BYTE        ((uint8_t)0x00)
#define WREN              ((uint8_t)0x06)

#define CONFIG_ADDRESS  ((uint32_t)0x3FFF00)

typedef struct write_page_command_t {
    uint8_t command;
    uint8_t address[3];
    uint8_t data[256];
} write_page_command;

typedef struct config_data_t {
    uint32_t time;
    uint8_t date;
    uint8_t mounth;
    uint16_t year;
    uint8_t limit_angle;
    uint8_t reaction_time;
    uint8_t vibration_level;
} config_data;

void init_flash(void);
void flash_is_answer (ret_code_t result, void * p_user_data);
void start_configuration (ret_code_t result, void * p_user_data);
static ret_code_t init_spi0_master(void);
void write_flash(uint32_t address, void * p_user_data, uint8_t num_byte);


#endif