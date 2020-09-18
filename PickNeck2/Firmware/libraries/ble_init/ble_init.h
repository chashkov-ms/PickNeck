#ifndef BLE_INIT_H__
#define BLE_INIT_H__


#include "ble_cts_c.h"


#ifdef __cplusplus
extern "C" {
#endif


void get_time(uint32_t err_code);
void current_time_print(ble_cts_c_evt_t * p_evt);

void ble_init(void);
void start_ble_advertising(bool erase_bonds);

void battery_level_update(void);

void delete_bonds(void);

void imu_measurement_send(void);

void ble_restart_without_whitelist(void);

#ifdef __cplusplus
}
#endif

#endif // BLE_INIT_H__

/** @} */