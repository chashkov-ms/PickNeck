#ifndef SPI_H__
#define SPI_H__

#include <stdint.h>
#include "nrf_drv_spi.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
extern void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context);
void init_spi(void);
void write_spi(void);
uint8_t read_spi(void);
void get_spi_rx(uint8_t * rx_data);
void set_spi_tx(uint8_t * tx_data, uint8_t num_byte);

#endif // SPI_H__

/** @} */