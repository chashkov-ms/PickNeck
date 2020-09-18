#ifndef BLE_SERVICES_H
#define BLE_SERVICES_H

NRF_BLE_QWR_DEF(m_qwr);                                                             /**< Context for the Queued Write module.*/

void qwr_init(void);
void nrf_qwr_error_handler(uint32_t nrf_error);

#endif