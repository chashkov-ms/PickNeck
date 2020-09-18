#include "lsm6dsl_example.h"
#include "lsm6dsl_drivers.h"
#include <string.h>
#include <stdio.h>

#include "nrfx_spi.h"
#include "nrfx_spim.h"

#define TX_BUF_DIM          1000

stmdev_ctx_t dev_ctx;

typedef union{
  int16_t i16bit[3];
  uint8_t u8bit[6];
} axis3bit16_t;

typedef union{
  int16_t i16bit;
  uint8_t u8bit[2];
} axis1bit16_t;

static axis3bit16_t data_raw_acceleration;
static axis3bit16_t data_raw_angular_rate;
static axis1bit16_t data_raw_temperature;
static float acceleration_mg[3];
static float angular_rate_mdps[3];
static float temperature_degC;
static uint8_t whoamI, rst;
static uint8_t tx_buffer[TX_BUF_DIM];



void lsm6dsl_handle(void)
{}

void lsm6dsl_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
    // CS_LOW
    // SPI_TRANSMIT
    // SPI_TRANSMIT
    // CS_HIGH
}

void lsm6dsl_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
    // CS_LOW
    // SPI_TRANSMIT
    // SPI_RECEIVE
    // CS_HIGH
}

void lsm6dsl_init(void)
{
    dev_ctx.write_reg = lsm6dsl_write;
    dev_ctx.read_reg = lsm6dsl_read;
    dev_ctx.handle = &lsm6dsl_handle;
      /*
    *  Check device ID
    */
    whoamI = 0;
    lsm6dsl_device_id_get(&dev_ctx, &whoamI);
    if ( whoamI != LSM6DSL_ID )
    {
        while(1); /*manage here device not found */
    }
    /*
    *  Restore default configuration
    */
    lsm6dsl_reset_set(&dev_ctx, PROPERTY_ENABLE);
    do {
        lsm6dsl_reset_get(&dev_ctx, &rst);
    } while (rst);
    /*
    *  Enable Block Data Update
    */
    lsm6dsl_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
    /*
    * Set Output Data Rate
    */
    lsm6dsl_xl_data_rate_set(&dev_ctx, LSM6DSL_XL_ODR_12Hz5);
    lsm6dsl_gy_data_rate_set(&dev_ctx, LSM6DSL_GY_ODR_12Hz5);
    /*
    * Set full scale
    */ 
    lsm6dsl_xl_full_scale_set(&dev_ctx, LSM6DSL_2g);
    lsm6dsl_gy_full_scale_set(&dev_ctx, LSM6DSL_2000dps);

    /*
    * Configure filtering chain(No aux interface)
    */ 
    /* Accelerometer - analog filter */
    lsm6dsl_xl_filter_analog_set(&dev_ctx, LSM6DSL_XL_ANA_BW_400Hz);

    /* Accelerometer - LPF1 path ( LPF2 not used )*/
    //lsm6dsl_xl_lp1_bandwidth_set(&dev_ctx, LSM6DSL_XL_LP1_ODR_DIV_4);

    /* Accelerometer - LPF1 + LPF2 path */  
    lsm6dsl_xl_lp2_bandwidth_set(&dev_ctx, LSM6DSL_XL_LOW_NOISE_LP_ODR_DIV_100);

    /* Accelerometer - High Pass / Slope path */
    //lsm6dsl_xl_reference_mode_set(&dev_ctx, PROPERTY_DISABLE);
    //lsm6dsl_xl_hp_bandwidth_set(&dev_ctx, LSM6DSL_XL_HP_ODR_DIV_100);

    /* Gyroscope - filtering chain */
    lsm6dsl_gy_band_pass_set(&dev_ctx, LSM6DSL_HP_260mHz_LP1_STRONG);
}


void lsm6dsl_example(void)
{
    /*
    * Read output only if new value is available
    */
    lsm6dsl_reg_t reg;
    lsm6dsl_status_reg_get(&dev_ctx, &reg.status_reg);

    if (reg.status_reg.xlda)
    {
        /* Read magnetic field data */
        memset(data_raw_acceleration.u8bit, 0x00, 3*sizeof(int16_t));
        lsm6dsl_acceleration_raw_get(&dev_ctx, data_raw_acceleration.u8bit);
        acceleration_mg[0] = lsm6dsl_from_fs2g_to_mg( data_raw_acceleration.i16bit[0]);
        acceleration_mg[1] = lsm6dsl_from_fs2g_to_mg( data_raw_acceleration.i16bit[1]);
        acceleration_mg[2] = lsm6dsl_from_fs2g_to_mg( data_raw_acceleration.i16bit[2]);

        sprintf((char*)tx_buffer, "Acceleration [mg]:%4.2f\t%4.2f\t%4.2f\r\n",
        acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);
        tx_com( tx_buffer, strlen( (char const*)tx_buffer ) );
    }
    if (reg.status_reg.gda)
    {
        /* Read magnetic field data */
        memset(data_raw_angular_rate.u8bit, 0x00, 3*sizeof(int16_t));
        lsm6dsl_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate.u8bit);
        angular_rate_mdps[0] = lsm6dsl_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[0]);
        angular_rate_mdps[1] = lsm6dsl_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[1]);
        angular_rate_mdps[2] = lsm6dsl_from_fs2000dps_to_mdps(data_raw_angular_rate.i16bit[2]);

        sprintf((char*)tx_buffer, "Angular rate [mdps]:%4.2f\t%4.2f\t%4.2f\r\n",
        angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
        tx_com( tx_buffer, strlen( (char const*)tx_buffer ) );
    }
    if (reg.status_reg.tda)
    {
        /* Read temperature data */
        memset(data_raw_temperature.u8bit, 0x00, sizeof(int16_t));
        lsm6dsl_temperature_raw_get(&dev_ctx, data_raw_temperature.u8bit);
        temperature_degC = lsm6dsl_from_lsb_to_celsius( data_raw_temperature.i16bit );

        sprintf((char*)tx_buffer, "Temperature [degC]:%6.2f\r\n", temperature_degC );
        tx_com( tx_buffer, strlen( (char const*)tx_buffer ) );
    }
}


