#include <string.h>
#include "imu.h"
#include "nrf_log.h"
#include "pin_config.h"
#include "app_scheduler.h"
#include "spi_application.h"
#include "imu_filter.h"


imu_read_data_status data_read_flag = NO_READ;

extern void imu_filtering(ret_code_t result, void * p_user_data);


void imu_is_answer (ret_code_t result, void * p_user_data)
{
    const uint8_t NRF_SPI_MNGR_BUFFER_LOC_IND command[] = {
        CTRL1_XL,            
          ODR_26_HZ
    };

    uint8_t answer = *(uint8_t*)p_user_data;
    if (answer != 106)
    {
        NRF_LOG_ERROR("IMU is not answer!!");
        return;
    }
    NRF_LOG_INFO("IMU is answering");

    send_imu_spi_message(&command[0], 2, 0, set_gyr_rate); // set 26 Hz rate accelerometer
    return;
}


void set_gyr_rate (ret_code_t result, void * p_user_data)
{
    const uint8_t NRF_SPI_MNGR_BUFFER_LOC_IND command[] = {
        CTRL2_G,             // set 26 Hz rate gyroscope
          ODR_26_HZ
    };
    
    send_imu_spi_message(&command[0], 2, 0, set_accel_interrupt);
}


void set_accel_interrupt (ret_code_t result, void * p_user_data)
{
    const uint8_t NRF_SPI_MNGR_BUFFER_LOC_IND command[] = {
        INT1_CTRL,           
          INTx_DRDY_XL
    };

    send_imu_spi_message(&command[0], 2, 0, set_gyr_interrupt); 
}


void set_gyr_interrupt (ret_code_t result, void * p_user_data)
{
    const uint8_t NRF_SPI_MNGR_BUFFER_LOC_IND command[] = {
        INT2_CTRL,
          INTx_DRDY_G
    };

    send_imu_spi_message(&command[0], 2, 0, NULL); 
}


void read_imu_steps (ret_code_t result, void * p_user_data)
{
    
}


static void accelerometer_ready_evt(void * p_event_data, uint16_t event_size)
{
    const uint8_t NRF_SPI_MNGR_BUFFER_LOC_IND command[] = {
        OUTX_L_XL|0x80     // read accelerometer 6 bytes data
    };

    if (data_read_flag == NO_READ)
    {
        data_read_flag = ACCEL_READ;
    }
    else
    {
        send_imu_spi_message(&command[0], 1, (3+3)*2, imu_filtering);
        data_read_flag = NO_READ;
    }

    return;
}


static void gyroscope_ready_evt(void * p_event_data, uint16_t event_size)
{
    const uint8_t NRF_SPI_MNGR_BUFFER_LOC_IND command[] = {
        OUTX_L_XL|0x80     // read accelerometer 6 bytes data
    };

    if (data_read_flag == NO_READ)
    {
        data_read_flag = GYR_READ;
    }
    else
    {
        send_imu_spi_message(&command[0], 1, (3+3)*2, imu_filtering);
        data_read_flag = NO_READ;
    }
    return;
}


/**@brief Init IMU function.
 */
void init_imu(void)
{
    const uint8_t NRF_SPI_MNGR_BUFFER_LOC_IND command[] = {
        WHO_AM_I|0x80,       // read WHO_AM_I
    };

    init_spi1_master();

    send_imu_spi_message(&command[0], 1, 1, imu_is_answer);
    return;
}
