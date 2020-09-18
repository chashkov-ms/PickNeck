#include <string.h>
#include "imu.h"
#include "spi.h"
#include "nrf_log.h"

/**@brief Init IMU function.
 */
void init_imu(void)
{
    if (IMU_is_not_answer())
    {
        return;
    }


}


bool IMU_is_not_answer(void)
{
    uint8_t imu_read_addr = 0x0F | 0x80;
    set_spi_tx(&imu_read_addr, 1);
    if (read_spi() != 106)
    {
        NRF_LOG_ERROR("IMU not aswering!!!");
        return true;
    }
    NRF_LOG_INFO("IMU answered");
    return false;

}
