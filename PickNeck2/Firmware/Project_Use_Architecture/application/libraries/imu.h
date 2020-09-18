#ifndef IMU_H
#define IMU_H

#include <stdbool.h>
#include "sdk_errors.h"

#define WHO_AM_I        ((uint8_t)0x0F)
#define INT1_CTRL       ((uint8_t)0x0D)
#define INT2_CTRL       ((uint8_t)0x0E)
#define CTRL1_XL        ((uint8_t)0x10)
#define CTRL2_G         ((uint8_t)0x11)
#define CTRL3_C         ((uint8_t)0x12)

#define OUTX_L_G        ((uint8_t)0x22)
#define OUTX_H_G        ((uint8_t)0x23)
#define OUTY_L_G        ((uint8_t)0x24)
#define OUTY_H_G        ((uint8_t)0x25)
#define OUTZ_L_G        ((uint8_t)0x26)
#define OUTZ_H_G        ((uint8_t)0x27)

#define OUTX_L_XL       ((uint8_t)0x28)
#define OUTX_H_XL       ((uint8_t)0x29)
#define OUTY_L_XL       ((uint8_t)0x2A)
#define OUTY_H_XL       ((uint8_t)0x2B)
#define OUTZ_L_XL       ((uint8_t)0x2C)
#define OUTZ_H_XL       ((uint8_t)0x2D)

#define STEP_COUNTER_L  ((uint8_t)0x4B)
#define STEP_COUNTER_H  ((uint8_t)0x4C)

#define ODR_26_HZ       ((uint8_t)0x40)
#define ODR_52_HZ       ((uint8_t)0x41)

#define INTx_DRDY_XL    ((uint8_t)0x01)
#define INTx_DRDY_G     ((uint8_t)0x02)

#define SW_RESET        ((uint8_t)0x01)

typedef enum
{
    NO_READ = 0,
    ACCEL_READ,
    GYR_READ
} imu_read_data_status;


void imu_is_answer (ret_code_t result, void * p_user_data);
void set_gyr_rate (ret_code_t result, void * p_user_data);
void set_accel_interrupt (ret_code_t result, void * p_user_data);
void set_gyr_interrupt (ret_code_t result, void * p_user_data);

void init_imu(void);

#endif