/**@file
 *
 * @defgroup ble_cmd Command Service
 * @{
 * @ingroup  ble_sdk_srv
 * @brief    Comand Service implementation.
 *
 * @details The Command Service is a simple GATT-based service with TX and RX characteristics.
 *          Data received from the peer is passed to the application, and the data received
 *          from the application of this service is sent to the peer as Handle Value
 *          Notifications. This module demonstrates how to implement a custom GATT-based
 *          service and characteristics using the SoftDevice. The service
 *          is used by the application to send and receive ASCII text strings to and from the
 *          peer.
 *
 * @note    The application must register this module as BLE event observer using the
 *          NRF_SDH_BLE_OBSERVER macro. Example:
 *          @code
 *              ble_cmd_t instance;
 *              NRF_SDH_BLE_OBSERVER(anything, BLE_CMD_BLE_OBSERVER_PRIO,
 *                                   ble_cmd_on_ble_evt, &instance);
 *          @endcode
 */
#ifndef BLE_CMD_H__
#define BLE_CMD_H__

#include <stdint.h>
#include <stdbool.h>
#include "sdk_config.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "ble_link_ctx_manager.h"

#ifdef __cplusplus
extern "C" {
#endif
#define BLE_CMD_BLE_OBSERVER_PRIO 3
/**@brief   Macro for defining a ble_cmd instance.
 *
 * @param     _name            Name of the instance.
 * @param[in] _cmd_max_clients Maximum number of CMD clients connected at a time.
 * @hideinitializer
 */
#define BLE_CMD_DEF(_name, _cmd_max_clients)                      \
	BLE_LINK_CTX_MANAGER_DEF(CONCAT_2(_name, _link_ctx_storage),  \
							 (_cmd_max_clients),                  \
							 sizeof(ble_cmd_client_context_t));   \
	static ble_cmd_t _name =                                      \
	{                                                             \
		.p_link_ctx_storage = &CONCAT_2(_name, _link_ctx_storage) \
	};                                                            \
	NRF_SDH_BLE_OBSERVER(_name ## _obs,                           \
						 BLE_CMD_BLE_OBSERVER_PRIO,               \
						 ble_cmd_on_ble_evt,                      \
						 &_name)


/**@brief   Command Service event types. */
typedef enum
{
	BLE_CMD_EVT_RX_DATA,     /**< Data received. */
	BLE_CMD_EVT_TX_READY,    /**< Service is ready to accept new data to be transmitted. */
	BLE_CMD_EVT_NOT_STARTED, /**< Notification has been enabled. */
	BLE_CMD_EVT_NOT_STOPPED, /**< Notification has been disabled. */
} ble_cmd_evt_type_t;


/* Forward declaration of the ble_cmd_t type. */
typedef struct ble_cmd_s ble_cmd_t;


/**@brief   Command Service @ref BLE_CMD_EVT_RX_DATA event data.
 *
 * @details This structure is passed to an event when @ref BLE_CMD_EVT_RX_DATA occurs.
 */
typedef struct
{
	uint8_t const * p_data; /**< A pointer to the buffer with received data. */
	uint16_t        length; /**< Length of received data. */
} ble_cmd_evt_rx_data_t;


/**@brief Command Service client context structure.
 *
 * @details This structure contains state context related to hosts.
 */
typedef struct
{
	bool is_notification_enabled; /**< Variable to indicate if the peer has enabled notification of the RX characteristic.*/
} ble_cmd_client_context_t;


/**@brief   Command Service event structure.
 *
 * @details This structure is passed to an event coming from service.
 */
typedef struct
{
	ble_cmd_evt_type_t         type;        /**< Event type. */
	ble_cmd_t                * p_cmd;       /**< A pointer to the instance. */
	uint16_t                   conn_handle; /**< Connection handle. */
	ble_cmd_client_context_t * p_link_ctx;  /**< A pointer to the link context. */
	union
	{
		ble_cmd_evt_rx_data_t rx_data; /**< @ref BLE_CMD_EVT_RX_DATA event data. */
	} params;
} ble_cmd_evt_t;


/**@brief Command Service event handler type. */
typedef void (* ble_cmd_data_handler_t) (ble_cmd_evt_t * p_evt);


/**@brief   Command Service initialization structure.
 *
 * @details This structure contains the initialization information for the service. The application
 * must fill this structure and pass it to the service using the @ref ble_cms_init
 *          function.
 */
typedef struct
{
	ble_cmd_data_handler_t data_handler; /**< Event handler to be called for handling received data. */
} ble_cmd_init_t;


/**@brief   Command Service structure.
 *
 * @details This structure contains status information related to the service.
 */
struct ble_cmd_s
{
	uint8_t                         uuid_type;          /**< UUID type for Command Service Base UUID. */
	uint16_t                        service_handle;     /**< Handle of Command Service (as provided by the SoftDevice). */
	ble_gatts_char_handles_t        tx_handles;         /**< Handles related to the TX characteristic (as provided by the SoftDevice). */
	ble_gatts_char_handles_t        rx_handles;         /**< Handles related to the RX characteristic (as provided by the SoftDevice). */
	blcm_link_ctx_storage_t * const p_link_ctx_storage; /**< Pointer to link context storage with handles of all current connections and its context. */
	ble_cmd_data_handler_t          data_handler;       /**< Event handler to be called for handling received data. */
};


/**@brief   Function for initializing the Command Service.
 *
 * @param[out] p_cmd      Command Service structure. This structure must be supplied
 *                        by the application. It is initialized by this function and will
 *                        later be used to identify this particular service instance.
 * @param[in] p_cmd_init  Information needed to initialize the service.
 *
 * @retval NRF_SUCCESS If the service was successfully initialized. Otherwise, an error code is returned.
 * @retval NRF_ERROR_NULL If either of the pointers p_cmd or p_cmd_init is NULL.
 */
uint32_t ble_cmd_init(ble_cmd_t * p_cmd, ble_cmd_init_t const * p_cmd_init);


/**@brief   Function for handling the Command Service's BLE events.
 *
 * @details The Command Service expects the application to call this function each time an
 * event is received from the SoftDevice. This function processes the event if it
 * is relevant and calls the Command Service event handler of the
 * application if necessary.
 *
 * @param[in] p_ble_evt     Event received from the SoftDevice.
 * @param[in] p_context     Command Service structure.
 */
void ble_cmd_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);


/**@brief   Function for sending a data to the peer.
 *
 * @details This function sends the input string as an RX characteristic notification to the
 *          peer.
 *
 * @param[in]     p_cmd       Pointer to the Command Service structure.
 * @param[in]     p_data      String to be sent.
 * @param[in,out] p_length    Pointer Length of the string. Amount of sent bytes.
 * @param[in]     conn_handle Connection Handle of the destination client.
 *
 * @retval NRF_SUCCESS If the string was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_cmd_data_send(ble_cmd_t * p_cmd,
						   uint8_t   * p_data,
						   uint16_t  * p_length,
						   uint16_t    conn_handle);


#ifdef __cplusplus
}
#endif

#endif // BLE_CMD_H__

/** @} */
