/** @file
 *
 * @defgroup ble_imu IMU Service
 * @{
 * @ingroup ble_sdk_srv
 * @brief IMU Service module.
 *
 * @details This module implements the IMU Service.
 *
 *          If an event handler is supplied by the application, the Health Thermometer
 *          Service will generate IMU Service events to the application.
 *
 * @note    The application must register this module as BLE event observer using the
 *          NRF_SDH_BLE_OBSERVER macro. Example:
 *          @code
 *              ble_imu_t instance;
 *              NRF_SDH_BLE_OBSERVER(anything, BLE_IMU_BLE_OBSERVER_PRIO,
 *                                   ble_imu_on_ble_evt, &instance);
 *          @endcode
 *
 * @note Attention!
 *  To maintain compliance with Nordic Semiconductor ASA Bluetooth profile
 *  qualification listings, this section of source code must not be modified.
 */

#ifndef BLE_IMU_H__
#define BLE_IMU_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "ble_date_time.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gq.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifndef BLE_IMU_BLE_OBSERVER_PRIO
#define BLE_IMU_BLE_OBSERVER_PRIO 2
#endif
/**@brief   Macro for defining a ble_imu instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_IMU_DEF(_name)                                                                          \
static ble_imu_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
					 BLE_IMU_BLE_OBSERVER_PRIO,                                                     \
					 ble_imu_on_ble_evt, &_name)


/**@brief IMU Service event type. */
typedef enum
{
	BLE_IMU_ACC_EVT_INDICATION_ENABLED,                                     /**< IMU value indication enabled event. */
	BLE_IMU_ACC_EVT_INDICATION_DISABLED,                                    /**< IMU value indication disabled event. */
	BLE_IMU_GYR_EVT_INDICATION_ENABLED,                                     /**< IMU value indication enabled event. */
	BLE_IMU_GYR_EVT_INDICATION_DISABLED,                                    /**< IMU value indication disabled event. */
	BLE_IMU_ANG_EVT_INDICATION_ENABLED,                                     /**< IMU value indication enabled event. */
	BLE_IMU_ANG_EVT_INDICATION_DISABLED,                                    /**< IMU value indication disabled event. */
	BLE_IMU_PED_EVT_INDICATION_ENABLED,                                     /**< IMU value indication enabled event. */
	BLE_IMU_PED_EVT_INDICATION_DISABLED,                                    /**< IMU value indication disabled event. */
	BLE_IMU_EVT_INDICATION_CONFIRMED,                                       /**< Confirmation of a IMU measurement indication has been received. */

} ble_imu_evt_type_t;

/**@brief IMU Service event. */
typedef struct
{
	ble_imu_evt_type_t evt_type;                                            /**< Type of event. */
} ble_imu_evt_t;

// Forward declaration of the ble_imu_t type.
typedef struct ble_imu_s ble_imu_t;

/**@brief Health Thermometer Service event handler type. */
typedef void (*ble_imu_evt_handler_t) (ble_imu_t * p_imu, ble_imu_evt_t * p_evt);

/**@brief FLOAT format (IEEE-11073 32-bit FLOAT, defined as a 32-bit value with a 24-bit mantissa
 *        and an 8-bit exponent. */
typedef struct
{
  float  x_value;                                                         /**< */
  float  y_value;                                                         /**< */
  float  z_value;                                                         /**< */
} xyz_value_t;

/**@brief Health Thermometer Service init structure. This contains all options and data
 *        needed for initialization of the service. */
typedef struct
{
	ble_imu_evt_handler_t        evt_handler;                               /**< Event handler to be called for handling events in the Health Thermometer Service. */
	ble_srv_error_handler_t      error_handler;                             /**< Function to be called in case of an error. */
	uint8_t                      temp_type_as_characteristic;               /**< Set non-zero if temp type given as characteristic */
	uint8_t                      temp_type;                                 /**< Temperature type if temperature characteristic is used */
	nrf_ble_gq_t               * p_gatt_queue;                              /**< Pointer to BLE GATT Queue instance. */
} ble_imu_init_t;

/**@brief Health Thermometer Service structure. This contains various status information for
 *        the service. */
struct ble_imu_s
{
	uint8_t                      uuid_type;                                 /**< UUID type for IMU Service Base UUID. */
	ble_imu_evt_handler_t        evt_handler;                               /**< Event handler to be called for handling events in the Health Thermometer Service. */
	ble_srv_error_handler_t      error_handler;                             /**< Function to be called in case of an error. */
	uint16_t                     service_handle;                            /**< Handle of Health Thermometer Service (as provided by the BLE stack). */
	ble_gatts_char_handles_t     meas_acc_handles;                          /**< Handles related to the Accelerometer Measurement characteristic. */
	ble_gatts_char_handles_t     meas_gyr_handles;                          /**< Handles related to the Gyroscope Measurement characteristic. */
	ble_gatts_char_handles_t     meas_ang_handles;                          /**< Handles related to the Angle Measurement characteristic. */
	ble_gatts_char_handles_t     meas_ped_handles;                          /**< Handles related to the Pedometer Measurement characteristic. */
	uint16_t                     conn_handle;                               /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
	uint8_t                      temp_type;                                 /**< Temperature type indicates where the measurement was taken. */
	nrf_ble_gq_t               * p_gatt_queue;                              /**< Pointer to BLE GATT Queue instance. */
};

/**@brief Health Thermometer Service measurement structure. This contains a Health Thermometer
 *        measurement. */
typedef struct ble_imu_meas_s
{
	xyz_value_t                  acc_meas;                                  /**<  */
	xyz_value_t                  gyr_meas;                                  /**<  */
	xyz_value_t                  ang_meas;                                  /**<  */
	uint32_t                     ped_meas;                                  /**<  */
	uint8_t                      temp_type;                                 /**<  */
} ble_imu_meas_t;


/**@brief Function for initializing the IMU Service.
 *
 * @param[out]  p_imu       Health Thermometer Service structure. This structure will have to
 *                          be supplied by the application. It will be initialized by this function,
 *                          and will later be used to identify this particular service instance.
 * @param[in]   p_imu_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_imu_init(ble_imu_t * p_imu, const ble_imu_init_t * p_imu_init);


/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the IMU Service.
 *
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 * @param[in]   p_context   IMU Service structure.
 */
void ble_imu_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);


/**@brief Function for sending health thermometer measurement if indication has been enabled.
 *
 * @details The application calls this function after having performed a IMU
 *          measurement. If indication has been enabled, the measurement data is encoded and
 *          sent to the client.
 *
 * @param[in]   p_imu       IMU Service structure.
 * @param[in]   p_imu_meas  Pointer to new health thermometer measurement.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
// uint32_t ble_imu_measurement_send(ble_imu_t * p_imu, ble_imu_meas_t * p_imu_meas);
uint32_t ble_imu_measurement_send(ble_imu_t * p_imu, ble_imu_meas_t * p_imu_meas, uint16_t value_handle);

/**@brief Function for checking if indication of IMU Measurement is currently enabled.
 *
 * @param[in]   p_imu                  IMU Service structure.
 * @param[out]  p_indication_enabled   TRUE if indication is enabled, FALSE otherwise.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
// uint32_t ble_imu_is_indication_enabled(ble_imu_t * p_imu, bool * p_indication_enabled);


#ifdef __cplusplus
}
#endif

#endif // BLE_IMU_H__

/** @} */
