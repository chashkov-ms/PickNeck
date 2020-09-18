#include "sdk_common.h"
#if NRF_MODULE_ENABLED(BLE_IMU)
#include "ble_err.h"
#include "ble_imu.h"
#include "ble_config.h"
#include <string.h>
#include "ble_srv_common.h"


/**@brief Function for interception of GATT errors and @ref nrf_ble_gq errors.
 *
 * @param[in] nrf_error   Error code.
 * @param[in] p_ctx       Parameter from the event handler.
 * @param[in] conn_handle Connection handle.
 */
static void gatt_error_handler(uint32_t nrf_error, void * p_ctx, uint16_t conn_handle)
{
	ble_imu_t * p_imu = (ble_imu_t *)p_ctx;

	if (p_imu->error_handler != NULL)
	{
		p_imu->error_handler(nrf_error);
	}
}


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_imu       IMU Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_imu_t * p_imu, ble_evt_t const * p_ble_evt)
{
	ret_code_t err_code;

	p_imu->conn_handle = p_ble_evt->evt.gatts_evt.conn_handle;

	err_code = nrf_ble_gq_conn_handle_register(p_imu->p_gatt_queue, p_imu->conn_handle);

	if ((p_imu->error_handler != NULL) &&
		(err_code != NRF_SUCCESS))
	{
		p_imu->error_handler(err_code);
	}
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_imu       IMU Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_imu_t * p_imu, ble_evt_t const * p_ble_evt)
{
	UNUSED_PARAMETER(p_ble_evt);
	p_imu->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling the Write event.
 *
 * @param[in]   p_imu       IMU Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_imu_t * p_imu, ble_evt_t const * p_ble_evt)
{
	ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

	if (p_evt_write->handle == p_imu->meas_acc_handles.cccd_handle)
	{
		if (p_evt_write->len == 2)
		{
			if (p_imu->evt_handler != NULL)
			{
				ble_imu_evt_t evt;

				if (ble_srv_is_indication_enabled(p_evt_write->data))
				{
					evt.evt_type = BLE_IMU_ACC_EVT_INDICATION_ENABLED;
				} else {
					evt.evt_type = BLE_IMU_ACC_EVT_INDICATION_DISABLED;
				}

				p_imu->evt_handler(p_imu, &evt);
			}
		}
	}
	else if (p_evt_write->handle == p_imu->meas_gyr_handles.cccd_handle)
	{
		if (p_evt_write->len == 2)
		{
			if (p_imu->evt_handler != NULL)
			{
				ble_imu_evt_t evt;

				if (ble_srv_is_indication_enabled(p_evt_write->data))
				{
					evt.evt_type = BLE_IMU_GYR_EVT_INDICATION_ENABLED;
				} else {
					evt.evt_type = BLE_IMU_GYR_EVT_INDICATION_DISABLED;
				}

				p_imu->evt_handler(p_imu, &evt);
			}
		}
	}
	else if (p_evt_write->handle == p_imu->meas_ang_handles.cccd_handle)
	{
		if (p_evt_write->len == 2)
		{
			// CCCD written, update indication state
			if (p_imu->evt_handler != NULL)
			{
				ble_imu_evt_t evt;

				if (ble_srv_is_indication_enabled(p_evt_write->data))
				{
					evt.evt_type = BLE_IMU_ANG_EVT_INDICATION_ENABLED;
				} else {
					evt.evt_type = BLE_IMU_ANG_EVT_INDICATION_DISABLED;
				}

				p_imu->evt_handler(p_imu, &evt);
			}
		}
	}
	else if (p_evt_write->handle == p_imu->meas_ped_handles.cccd_handle)
	{
		if (p_evt_write->len == 2)
		{
			if (p_imu->evt_handler != NULL)
			{
				ble_imu_evt_t evt;

				if (ble_srv_is_indication_enabled(p_evt_write->data))
				{
					evt.evt_type = BLE_IMU_PED_EVT_INDICATION_ENABLED;
				} else {
					evt.evt_type = BLE_IMU_PED_EVT_INDICATION_DISABLED;
				}

				p_imu->evt_handler(p_imu, &evt);
			}
		}
	}
}

/**@brief Function for handling the HVC event.
 *
 * @details Handles HVC events from the BLE stack.
 *
 * @param[in]   p_imu       IMU Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_hvc(ble_imu_t * p_imu, ble_evt_t const * p_ble_evt)
{
	ble_gatts_evt_hvc_t const * p_hvc = &p_ble_evt->evt.gatts_evt.params.hvc;

	if ((p_hvc->handle == p_imu->meas_acc_handles.value_handle) ||
		(p_hvc->handle == p_imu->meas_gyr_handles.value_handle) ||
		(p_hvc->handle == p_imu->meas_ang_handles.value_handle) ||
		(p_hvc->handle == p_imu->meas_ped_handles.value_handle))
	{
		ble_imu_evt_t evt;

		evt.evt_type = BLE_IMU_EVT_INDICATION_CONFIRMED;
		p_imu->evt_handler(p_imu, &evt);
	}
}


void ble_imu_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
	ble_imu_t * p_imu = (ble_imu_t *)p_context;

	switch (p_ble_evt->header.evt_id)
	{
		case BLE_GAP_EVT_CONNECTED:
			on_connect(p_imu, p_ble_evt);
			break;

		case BLE_GAP_EVT_DISCONNECTED:
			on_disconnect(p_imu, p_ble_evt);
			break;

		case BLE_GATTS_EVT_WRITE:
			on_write(p_imu, p_ble_evt);
			break;

		case BLE_GATTS_EVT_HVC:
			on_hvc(p_imu, p_ble_evt);
			break;

		default:
			// No implementation needed.
			break;
	}
}


uint32_t ble_imu_init(ble_imu_t * p_imu, ble_imu_init_t const * p_imu_init)
{
	VERIFY_PARAM_NOT_NULL(p_imu);
	VERIFY_PARAM_NOT_NULL(p_imu_init);
	VERIFY_PARAM_NOT_NULL(p_imu_init->p_gatt_queue);

	ret_code_t            err_code;
	uint8_t               init_value[BLE_MAX_DATA_LEN];
	ble_imu_meas_t        initial_imu;
	ble_uuid_t            ble_uuid;
	ble_uuid128_t         imu_base_uuid = IMU_BASE_UUID;
	ble_add_char_params_t add_char_params;

	// Initialize service structure
	p_imu->evt_handler   = p_imu_init->evt_handler;
	p_imu->p_gatt_queue  = p_imu_init->p_gatt_queue;
	p_imu->error_handler = p_imu_init->error_handler;
	p_imu->conn_handle   = BLE_CONN_HANDLE_INVALID;
	p_imu->temp_type     = p_imu_init->temp_type;

	/**@snippet [Adding proprietary Service to the SoftDevice] */
	// Add a custom base UUID.
	err_code = sd_ble_uuid_vs_add(&imu_base_uuid, &p_imu->uuid_type);
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}

	ble_uuid.type = p_imu->uuid_type;
	ble_uuid.uuid = BLE_UUID_IMU_SERVICE;

	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_imu->service_handle);
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}

	// Add accelerometer measurement characteristic
	memset(&add_char_params, 0, sizeof(add_char_params));
	memset(&initial_imu, 0, sizeof(initial_imu));

	add_char_params.uuid                = BLE_UUID_IMU_ACC_CHARACTERISTIC;
	add_char_params.uuid_type           = p_imu->uuid_type;
	add_char_params.init_len            = sizeof(initial_imu.acc_meas);
	add_char_params.max_len             = sizeof(initial_imu.acc_meas); // BLE_MAX_DATA_LEN;
	add_char_params.p_init_value        = init_value;
	add_char_params.is_var_len          = true;
	add_char_params.char_props.indicate = 1;

	add_char_params.cccd_write_access   = SEC_JUST_WORKS;   // SEC_OPEN

	err_code = characteristic_add(p_imu->service_handle, &add_char_params, &p_imu->meas_acc_handles);
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}

	// Add gyroscope measurement characteristic

	add_char_params.uuid                = BLE_UUID_IMU_GYR_CHARACTERISTIC;
	add_char_params.uuid_type           = p_imu->uuid_type;
	add_char_params.init_len            = sizeof(initial_imu.gyr_meas);
	add_char_params.max_len             = sizeof(initial_imu.gyr_meas); // BLE_MAX_DATA_LEN;
	add_char_params.p_init_value        = init_value;
	add_char_params.is_var_len          = true;
	add_char_params.char_props.indicate = 1;

	add_char_params.cccd_write_access   = SEC_JUST_WORKS;

	err_code = characteristic_add(p_imu->service_handle, &add_char_params, &p_imu->meas_gyr_handles);
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}

	// Add angle measurement characteristic

	add_char_params.uuid                = BLE_UUID_IMU_ANG_CHARACTERISTIC;
	add_char_params.uuid_type           = p_imu->uuid_type;
	add_char_params.init_len            = sizeof(initial_imu.ang_meas);
	add_char_params.max_len             = sizeof(initial_imu.ang_meas); // BLE_MAX_DATA_LEN;
	add_char_params.p_init_value        = init_value;
	add_char_params.is_var_len          = true;
	add_char_params.char_props.indicate = 1;

	add_char_params.cccd_write_access   = SEC_JUST_WORKS;

	err_code = characteristic_add(p_imu->service_handle, &add_char_params, &p_imu->meas_ang_handles);
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}

	// Add pedometer measurement characteristic

	add_char_params.uuid                = BLE_UUID_IMU_PED_CHARACTERISTIC;
	add_char_params.uuid_type           = p_imu->uuid_type;
	add_char_params.init_len            = sizeof(initial_imu.ped_meas);
	add_char_params.max_len             = sizeof(initial_imu.ped_meas); // BLE_MAX_DATA_LEN;
	add_char_params.p_init_value        = init_value;
	add_char_params.is_var_len          = true;
	add_char_params.char_props.indicate = 1;

	add_char_params.cccd_write_access   = SEC_JUST_WORKS;

	err_code = characteristic_add(p_imu->service_handle, &add_char_params, &p_imu->meas_ped_handles);
	if (err_code != NRF_SUCCESS)
	{
		return err_code;
	}
	return NRF_SUCCESS;
}


/**@brief Function for encoding a float value.
 *
 * @param[in]   value            Value to be encoded.
 * @param[out]  p_encoded_data   Buffer where the encoded data is to be written.
 *
 * @return      Number of bytes written.
 */
static uint8_t float_encode(float value, uint8_t * p_encoded_data)
{
	uint8_t *p = (uint8_t *) &value;
	p_encoded_data[0] = (uint8_t) p[0];
	p_encoded_data[1] = (uint8_t) p[1];
	p_encoded_data[2] = (uint8_t) p[2];
	p_encoded_data[3] = (uint8_t) p[3];
	return sizeof(float);
}


/**@brief Function for encoding a IMU Measurement.
 *
 * @param[in]   p_imu              IMU Service structure.
 * @param[in]   p_imu_meas         Measurement to be encoded.
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint16_t imu_measurement_encode(ble_imu_t * p_imu, ble_imu_meas_t * p_imu_meas,
									  uint8_t * p_encoded_buffer, uint16_t value_handle)
{
	uint16_t len = 0;

	if(value_handle == p_imu->meas_acc_handles.value_handle)
	{
		len += float_encode(p_imu_meas->acc_meas.x_value, &p_encoded_buffer[len]);
		len += float_encode(p_imu_meas->acc_meas.y_value, &p_encoded_buffer[len]);
		len += float_encode(p_imu_meas->acc_meas.z_value, &p_encoded_buffer[len]);
	}
	else if(value_handle == p_imu->meas_gyr_handles.value_handle)
	{
		len += float_encode(p_imu_meas->gyr_meas.x_value, &p_encoded_buffer[len]);
		len += float_encode(p_imu_meas->gyr_meas.y_value, &p_encoded_buffer[len]);
		len += float_encode(p_imu_meas->gyr_meas.z_value, &p_encoded_buffer[len]);
	}
	else if(value_handle == p_imu->meas_ang_handles.value_handle)
	{
		len += float_encode(p_imu_meas->ang_meas.x_value, &p_encoded_buffer[len]);
		len += float_encode(p_imu_meas->ang_meas.y_value, &p_encoded_buffer[len]);
		len += float_encode(p_imu_meas->ang_meas.z_value, &p_encoded_buffer[len]);
	}
	else if(value_handle == p_imu->meas_ped_handles.value_handle)
	{
		len += uint32_encode(p_imu_meas->ped_meas, &p_encoded_buffer[len]);
	}
	return len;
}


uint32_t ble_imu_measurement_send(ble_imu_t * p_imu, ble_imu_meas_t * p_imu_meas, uint16_t value_handle)
{
	ret_code_t err_code;

	// Send value if connected
	if (p_imu->conn_handle != BLE_CONN_HANDLE_INVALID)
	{
		uint8_t          encoded_imu_meas[BLE_MAX_DATA_LEN];
		uint16_t         len;
		nrf_ble_gq_req_t imu_req;

		len = imu_measurement_encode(p_imu, p_imu_meas, encoded_imu_meas, value_handle);

		memset(&imu_req, 0, sizeof(nrf_ble_gq_req_t));

		imu_req.type                      = NRF_BLE_GQ_REQ_GATTS_HVX;
		imu_req.error_handler.cb          = gatt_error_handler;
		imu_req.error_handler.p_ctx       = p_imu;
		imu_req.params.gatts_hvx.handle   = value_handle;
		imu_req.params.gatts_hvx.offset   = 0;
		imu_req.params.gatts_hvx.p_data   = encoded_imu_meas;
		imu_req.params.gatts_hvx.p_len    = &len;
		imu_req.params.gatts_hvx.type     = BLE_GATT_HVX_INDICATION;

		err_code = nrf_ble_gq_item_add(p_imu->p_gatt_queue, &imu_req, p_imu->conn_handle);
	} else {
		err_code = NRF_ERROR_INVALID_STATE;
	}

	return err_code;
}


// uint32_t ble_imu_is_indication_enabled(ble_imu_t * p_imu, bool * p_indication_enabled)
// {
// 	ret_code_t err_code;
// 	uint8_t  cccd_value_buf[BLE_CCCD_VALUE_LEN];
// 	ble_gatts_value_t gatts_value;

// 	// Initialize value struct.
// 	memset(&gatts_value, 0, sizeof(gatts_value));

// 	gatts_value.len     = BLE_CCCD_VALUE_LEN;
// 	gatts_value.offset  = 0;
// 	gatts_value.p_value = cccd_value_buf;

// 	err_code = sd_ble_gatts_value_get(p_imu->conn_handle,
// 									  p_imu->meas_acc_handles.cccd_handle,
// 									  &gatts_value);
// 	if (err_code == NRF_SUCCESS)
// 	{
// 		*p_indication_enabled = ble_srv_is_indication_enabled(cccd_value_buf);
// 	}
// 	if (err_code == BLE_ERROR_GATTS_SYS_ATTR_MISSING)
// 	{
// 		*p_indication_enabled = false;
// 		return NRF_SUCCESS;
// 	}
// 	return err_code;
// }


#endif // NRF_MODULE_ENABLED(BLE_IMU)
