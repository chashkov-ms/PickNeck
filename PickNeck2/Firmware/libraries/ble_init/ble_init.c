#include "app_timer.h"
#include "app_scheduler.h"

#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_bas.h"
#include "ble_cts_c.h"
#include "ble_conn_params.h"
#include "ble_db_discovery.h"
#include "ble_dis.h"
#include "ble_cmd.h"
#include "ble_imu.h"
#include "ble_hci.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "fds.h"
#include "rtc.h"

// #include "ble_gap.h"

#include "peer_manager.h"
#include "peer_manager_handler.h"

#include "led_action.h"

#include "nrf_log.h"

#include "pin_config.h"
#include "ble_config.h"

#define DEVICE_NAME                     "PicNeck"                                   /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "FabLiteElectronics"                        /**< Manufacturer. Will be passed to Device Information Service. */
#define MODEL_NUM                       "PicNeck-EXAMPLE_v1"                        /**< Model number. Will be passed to Device Information Service. */
#define SERIAL_NUM                      "0xFFZERO-ONE"
#define HARDWARE_REV                    "2.0"
#define FIRMWARE_REV                    "2.0"
#define SOFTWARE_REV                    "2.0"
#define MANUFACTURER_ID                 0x1122334455                                /**< Manufacturer ID, part of System ID. Will be passed to Device Information Service. */
#define ORG_UNIQUE_ID                   0x667788                                    /**< Organizational Unique ID, part of System ID. Will be passed to Device Information Service. */


#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */
#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */


#define APP_ADV_INTERVAL                64                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */

#define APP_ADV_DURATION                18000                                       /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define APP_ADV_FAST_INTERVAL           0x0028                                      /**< Fast advertising interval (in units of 0.625 ms). The default value corresponds to 25 ms. */
#define APP_ADV_SLOW_INTERVAL           0x0C80                                      /**< Slow advertising interval (in units of 0.625 ms). The default value corresponds to 2 seconds. */

#define APP_ADV_FAST_DURATION           3000                                        /**< The advertising duration of fast advertising in units of 10 milliseconds. */
#define APP_ADV_SLOW_DURATION           18000                                       /**< The advertising duration of slow advertising in units of 10 milliseconds. */


#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(500, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(1000, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_TIMEOUT               30                                          /**< Time-out for pairing request or security request (in seconds). */
#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection requirement. */
#define SEC_PARAM_LESC                  0                                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data availability. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define SCHED_MAX_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE                   /**< Maximum size of scheduler events. Note that scheduler BLE stack events do not contain any data, as the events are being pulled from the stack in the event handler. */
#ifdef SVCALL_AS_NORMAL_FUNCTION
#define SCHED_QUEUE_SIZE                20                                          /**< Maximum number of events in the scheduler queue. More is needed in case of Serialization. */
#else
#define SCHED_QUEUE_SIZE                10                                          /**< Maximum number of events in the scheduler queue. */
#endif

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

BLE_CTS_C_DEF(m_cts_c);                                                             /**< Current Time service instance. */
BLE_CMD_DEF(m_cmd, NRF_SDH_BLE_TOTAL_LINK_COUNT);                                   /**< BLE NUS service instance. */
BLE_IMU_DEF(m_imu);                                                                 /**< Structure used to identify the IMU service. */
NRF_BLE_GATT_DEF(m_gatt);                                                           /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                             /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                                                 /**< Advertising module instance. */
BLE_DB_DISCOVERY_DEF(m_ble_db_discovery);                                           /**< DB discovery module instance. */
NRF_BLE_GQ_DEF(m_ble_gatt_queue,                                                    /**< BLE GATT Queue instance. */
			   NRF_SDH_BLE_PERIPHERAL_LINK_COUNT,
			   NRF_BLE_GQ_QUEUE_SIZE);

uint16_t            m_conn_handle          = BLE_CONN_HANDLE_INVALID;                 /**< Handle of the current connection. */

static pm_peer_id_t m_peer_id;                                                      /**< Device reference handle to the current bonded central. */
static uint16_t     m_cur_conn_handle = BLE_CONN_HANDLE_INVALID;                    /**< Handle of the current connection. */

static pm_peer_id_t m_whitelist_peers[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];            /**< List of peers currently in the whitelist. */
static uint32_t     m_whitelist_peer_cnt;                                           /**< Number of peers currently in the whitelist. */

static bool         m_imu_meas_ind_conf_pending = false;                       /**< Flag to keep track of when an indication confirmation is pending. */

#define FREE_BLE_UUID_SERVICE 0xFD64
// Universally unique service identifier.
static ble_uuid_t m_adv_uuids[] = { {FREE_BLE_UUID_SERVICE, BLE_UUID_TYPE_BLE} };

BLE_BAS_DEF(m_bas);                                                                 /**< Structure used to identify the battery service. */

static char const * day_of_week[] =
{
	"Unknown",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday",
	"Sunday"
};

static char const * month_of_year[] =
{
	"Unknown",
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};

/**@brief Function for handling Service errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void service_error_handler(uint32_t nrf_error)
{
	APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for handling the Current Time Service errors.
 *
 * @param[in] p_evt  Event received from the Current Time Service client.
 */
void current_time_print(ble_cts_c_evt_t * p_evt)
{
	NRF_LOG_INFO("\r\nCurrent Time:");
	NRF_LOG_INFO("\r\nDate:");

	NRF_LOG_INFO("\tDay of week   %s", (uint32_t)day_of_week[p_evt->
														 params.
														 current_time.
														 exact_time_256.
														 day_date_time.
														 day_of_week]);

	if (p_evt->params.current_time.exact_time_256.day_date_time.date_time.day == 0)
	{
		NRF_LOG_INFO("\tDay of month  Unknown");
	}
	else
	{
		NRF_LOG_INFO("\tDay of month  %i",
						p_evt->params.current_time.exact_time_256.day_date_time.date_time.day);
	}

	NRF_LOG_INFO("\tMonth of year %s",
	(uint32_t)month_of_year[p_evt->params.current_time.exact_time_256.day_date_time.date_time.month]);
	if (p_evt->params.current_time.exact_time_256.day_date_time.date_time.year == 0)
	{
		NRF_LOG_INFO("\tYear          Unknown");
	}
	else
	{
		NRF_LOG_INFO("\tYear          %i",
						p_evt->params.current_time.exact_time_256.day_date_time.date_time.year);
	}
	NRF_LOG_INFO("\r\nTime:");
	NRF_LOG_INFO("\tHours     %i",
					p_evt->params.current_time.exact_time_256.day_date_time.date_time.hours);
	NRF_LOG_INFO("\tMinutes   %i",
					p_evt->params.current_time.exact_time_256.day_date_time.date_time.minutes);
	NRF_LOG_INFO("\tSeconds   %i",
					p_evt->params.current_time.exact_time_256.day_date_time.date_time.seconds);
	NRF_LOG_INFO("\tFractions %i/256 of a second",
					p_evt->params.current_time.exact_time_256.fractions256);

	NRF_LOG_INFO("\r\nAdjust reason:\r");
	NRF_LOG_INFO("\tDaylight savings %x",
					p_evt->params.current_time.adjust_reason.change_of_daylight_savings_time);
	NRF_LOG_INFO("\tTime zone        %x",
					p_evt->params.current_time.adjust_reason.change_of_time_zone);
	NRF_LOG_INFO("\tExternal update  %x",
					p_evt->params.current_time.adjust_reason.external_reference_time_update);
	NRF_LOG_INFO("\tManual update    %x",
					p_evt->params.current_time.adjust_reason.manual_time_update);
}


/**@brief Function for handling the Current Time Service errors.
 *
 * @param[in]  nrf_error  Error code containing information about what went wrong.
 */
static void current_time_error_handler(uint32_t nrf_error)
{
	APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for handling the Current Time Service client events.
 *
 * @details This function will be called for all events in the Current Time Service client that
 *          are passed to the application.
 *
 * @param[in] p_evt Event received from the Current Time Service client.
 */
static void on_cts_c_evt(ble_cts_c_t * p_cts, ble_cts_c_evt_t * p_evt)
{
	ret_code_t err_code;

	switch (p_evt->evt_type)
	{
		case BLE_CTS_C_EVT_DISCOVERY_COMPLETE:
			{
				NRF_LOG_INFO("Current Time Service discovered on server.");
				err_code = ble_cts_c_handles_assign(&m_cts_c,
													p_evt->conn_handle,
													&p_evt->params.char_handles);
				APP_ERROR_CHECK(err_code);
			} break;

		case BLE_CTS_C_EVT_DISCOVERY_FAILED:
			{
				NRF_LOG_INFO("Current Time Service not found on server. ");
				// CTS not found in this case we just disconnect. There is no reason to stay
				// in the connection for this simple app since it all wants is to interact with CT
				if (p_evt->conn_handle != BLE_CONN_HANDLE_INVALID)
				{
					err_code = sd_ble_gap_disconnect(p_evt->conn_handle,
													 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
					APP_ERROR_CHECK(err_code);
				}
			} break;

		case BLE_CTS_C_EVT_DISCONN_COMPLETE:
			{
				NRF_LOG_INFO("Disconnect Complete.");
			} break;

		case BLE_CTS_C_EVT_CURRENT_TIME:
			{
				NRF_LOG_INFO("Current Time received.");
				// current_time_print(p_evt);
				set_calendar(p_evt);
			} break;

		case BLE_CTS_C_EVT_INVALID_TIME:
			{
				NRF_LOG_INFO("Invalid Time received.");
			} break;

		default:
			{
			} break;
	}
}



/**@brief Function for performing a battery measurement, and update the Battery Level characteristic in the Battery Service.
 */
void battery_level_update(void)
{
	ret_code_t err_code;
	uint8_t  battery_level;
		static uint8_t x = 100;

	battery_level = (uint8_t)x;
		x = x - 1;

	err_code = ble_bas_battery_level_update(&m_bas, battery_level, BLE_CONN_HANDLE_ALL);
	if  ((err_code != NRF_SUCCESS) &&
		(err_code != NRF_ERROR_INVALID_STATE) &&
		(err_code != NRF_ERROR_RESOURCES) &&
		(err_code != NRF_ERROR_BUSY) &&
		(err_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING))
	{
		APP_ERROR_HANDLER(err_code);
	}
}


/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
 *          the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
	ret_code_t              err_code;
	ble_gap_conn_params_t   gap_conn_params;
	ble_gap_conn_sec_mode_t sec_mode;

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

	err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *) DEVICE_NAME, strlen(DEVICE_NAME));
	APP_ERROR_CHECK(err_code);

	memset(&gap_conn_params, 0, sizeof(gap_conn_params));

	gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
	gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
	gap_conn_params.slave_latency     = SLAVE_LATENCY;
	gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

	err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
	APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the data from the Command Service.
 *
 * @details This function will process the data received from the Command BLE Service
 *
 * @param[in] p_evt       Command Service event.
 */
/**@snippet [Handling the data received over BLE] */
static void cmd_data_handler(ble_cmd_evt_t * p_evt)
{
	uint16_t length = (uint16_t)p_evt->params.rx_data.length;
	// uint16_t length = 3;
	static uint8_t data_array[BLE_MAX_DATA_LEN];
	switch (p_evt->type)
	{
		case BLE_CMD_EVT_RX_DATA:
		{
			/* TODO */
			// parse_command(p_evt->params.rx_data.p_data, p_evt->params.rx_data.length)
			// send answer

			for(uint8_t i = 0; i < p_evt->params.rx_data.length; i++)
			{
				data_array[i] = p_evt->params.rx_data.p_data[i];
			}
			ret_code_t err_code;

			NRF_LOG_INFO("Received data from BLE CMD.");
			do
			{
				err_code = ble_cmd_data_send(&m_cmd, data_array, &length, m_conn_handle);
				if ((err_code != NRF_ERROR_INVALID_STATE) &&
					(err_code != NRF_ERROR_RESOURCES) &&
					(err_code != NRF_ERROR_NOT_FOUND))
				{
					APP_ERROR_CHECK(err_code);
				}
			} while (err_code == NRF_ERROR_RESOURCES);
		} break;

		case BLE_CMD_EVT_TX_READY:
		{
		} break;

		case BLE_CMD_EVT_NOT_STARTED:
		{
		} break;

		case BLE_CMD_EVT_NOT_STOPPED:
		{
		} break;

		default:
		{
		} break;
	}
}


/**@brief Function for simulating and sending one Temperature Measurement.
Handle Value operations
 */
static void imu_measurement_send(uint16_t value_handle)
{
	ble_imu_meas_t measuring;
	ret_code_t     err_code;

	if (!m_imu_meas_ind_conf_pending)
	{
		measuring.acc_meas.x_value = 9.1f;
		measuring.acc_meas.y_value = 9.2f;
		measuring.acc_meas.z_value = 9.3f;
		measuring.gyr_meas.x_value = 9.4f;
		measuring.gyr_meas.y_value = 9.5f;
		measuring.gyr_meas.z_value = 9.6f;
		measuring.ang_meas.x_value = 9.7f;
		measuring.ang_meas.y_value = 9.8f;
		measuring.ang_meas.z_value = 9.9f;
		measuring.ped_meas = 300;
		err_code = ble_imu_measurement_send(&m_imu, &measuring, value_handle);

		switch (err_code)
		{
			case NRF_SUCCESS:
				{
					// Measurement was successfully sent, wait for confirmation.
					m_imu_meas_ind_conf_pending = true;
				} break;

			case NRF_ERROR_INVALID_STATE:
				{
					// Ignore error.
				} break;

			default:
				{
					APP_ERROR_HANDLER(err_code);
				} break;
		}
	}
}


/**@brief Function for handling the IMU Service events.
 *
 * @details This function will be called for all IMU Service events which are passed
 *          to the application.
 *
 * @param[in] p_imu  IMU Service structure.
 * @param[in] p_evt  Event received from the IMU Service.
 */
static void on_imu_evt(ble_imu_t * p_imu, ble_imu_evt_t * p_evt)
{
	switch (p_evt->evt_type)
	{
		case BLE_IMU_ACC_EVT_INDICATION_ENABLED:
			{
				// Indication has been enabled, send a single IMU measurement
				imu_measurement_send(p_imu->meas_acc_handles.value_handle);
			} break;
		case BLE_IMU_GYR_EVT_INDICATION_ENABLED:
			{
				// Indication has been enabled, send a single IMU measurement
				imu_measurement_send(p_imu->meas_gyr_handles.value_handle);
			} break;
		case BLE_IMU_ANG_EVT_INDICATION_ENABLED:
			{
				// Indication has been enabled, send a single IMU measurement
				imu_measurement_send(p_imu->meas_ang_handles.value_handle);
			} break;
		case BLE_IMU_PED_EVT_INDICATION_ENABLED:
			{
				// Notification has been enabled, send a single IMU measurement
				imu_measurement_send(p_imu->meas_ped_handles.value_handle);
			} break;

		case BLE_IMU_EVT_INDICATION_CONFIRMED:
			{
				m_imu_meas_ind_conf_pending = false;
			} break;

		default:
			{
				// No implementation needed.
			} break;
	}
}


/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
	APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
	ret_code_t         err_code;
	ble_cmd_init_t     cmd_init;
	ble_imu_init_t     imu_init;
	ble_bas_init_t     bas_init;
	ble_dis_init_t     dis_init;
	ble_dis_sys_id_t   sys_id;
	ble_cts_c_init_t   cts_init = {0};
	nrf_ble_qwr_init_t qwr_init = {0};

	// Initialize Queued Write Module.
	qwr_init.error_handler = nrf_qwr_error_handler;

	err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
	APP_ERROR_CHECK(err_code);

	// Initialize CTS.
	cts_init.evt_handler   = on_cts_c_evt;
	cts_init.error_handler = current_time_error_handler;
	cts_init.p_gatt_queue  = &m_ble_gatt_queue;
	err_code               = ble_cts_c_init(&m_cts_c, &cts_init);
	APP_ERROR_CHECK(err_code);

	// Initialize CMD service.
	memset(&cmd_init, 0, sizeof(cmd_init));

	cmd_init.data_handler = cmd_data_handler;

	err_code = ble_cmd_init(&m_cmd, &cmd_init);
	APP_ERROR_CHECK(err_code);


	// Here the sec level for the Battery Service can be changed/increased.
	bas_init.bl_rd_sec        = SEC_OPEN;
	bas_init.bl_cccd_wr_sec   = SEC_OPEN;
	bas_init.bl_report_rd_sec = SEC_OPEN;

	bas_init.evt_handler          = NULL;
	bas_init.support_notification = true;
	bas_init.p_report_ref         = NULL;
	bas_init.initial_batt_level   = 100;

	err_code = ble_bas_init(&m_bas, &bas_init);
	APP_ERROR_CHECK(err_code);

	// Initialize IMU Service
	memset(&imu_init, 0, sizeof(imu_init));

	imu_init.evt_handler                 = on_imu_evt;
	imu_init.p_gatt_queue                = &m_ble_gatt_queue;
	imu_init.error_handler               = service_error_handler;
	imu_init.temp_type_as_characteristic = 0;
	imu_init.temp_type                   = 2;

	// Here the sec level for the IMU Service can be changed/increased.
	// imu_init.ht_meas_cccd_wr_sec = SEC_JUST_WORKS;

	err_code = ble_imu_init(&m_imu, &imu_init);
	APP_ERROR_CHECK(err_code);

	// Initialize Device Information Service.
	memset(&dis_init, 0, sizeof(dis_init));

	ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, MANUFACTURER_NAME);
	ble_srv_ascii_to_utf8(&dis_init.model_num_str, MODEL_NUM);
	ble_srv_ascii_to_utf8(&dis_init.serial_num_str, SERIAL_NUM);
	ble_srv_ascii_to_utf8(&dis_init.hw_rev_str, HARDWARE_REV);
	ble_srv_ascii_to_utf8(&dis_init.fw_rev_str, FIRMWARE_REV);
	ble_srv_ascii_to_utf8(&dis_init.sw_rev_str, SOFTWARE_REV);

	sys_id.manufacturer_id            = MANUFACTURER_ID;
	sys_id.organizationally_unique_id = ORG_UNIQUE_ID;
	dis_init.p_sys_id                 = &sys_id;

	dis_init.dis_char_rd_sec = SEC_OPEN;

	err_code = ble_dis_init(&dis_init);
	APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling an event from the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module
 *          which are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply setting
 *       the disconnect_on_fail config parameter, but instead we use the event handler
 *       mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
	ret_code_t err_code;

	if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
	{
		err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
		APP_ERROR_CHECK(err_code);
	}
}


/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
	APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
	ret_code_t               err_code;
	ble_conn_params_init_t   cp_init;

	memset(&cp_init, 0, sizeof(cp_init));

	cp_init.p_conn_params                  = NULL;
	cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
	cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
	cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
	cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
	cp_init.disconnect_on_fail             = false;
	cp_init.evt_handler                    = on_conn_params_evt;
	cp_init.error_handler                  = conn_params_error_handler;

	err_code = ble_conn_params_init(&cp_init);
	APP_ERROR_CHECK(err_code);
}


void get_time(uint32_t err_code)
{
	if (m_cts_c.conn_handle != BLE_CONN_HANDLE_INVALID)
	{
		err_code = ble_cts_c_current_time_read(&m_cts_c);
		if (err_code == NRF_ERROR_NOT_FOUND)
		{
			NRF_LOG_INFO("Current Time Service is not discovered.");
		}
	}
}


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
	uint32_t err_code;

	switch (p_ble_evt->header.evt_id)
	{
		case BLE_GAP_EVT_CONNECTED:
			{
				NRF_LOG_INFO("Connected");
				err_code = indication_set(INDICATE_CONNECTED);
				APP_ERROR_CHECK(err_code);
				m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
				err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
				APP_ERROR_CHECK(err_code);
				// get_time(err_code);

			} break;

		case BLE_GAP_EVT_DISCONNECTED:
			{
				NRF_LOG_INFO("Disconnected");
				// LED indication will be changed when advertising starts.
				m_conn_handle = BLE_CONN_HANDLE_INVALID;
				if (p_ble_evt->evt.gap_evt.conn_handle == m_cts_c.conn_handle)
				{
					m_cts_c.conn_handle = BLE_CONN_HANDLE_INVALID;
				}
			} break;

		case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
			{
				NRF_LOG_INFO("PHY update request.");
				ble_gap_phys_t const phys =
				{
					.rx_phys = BLE_GAP_PHY_AUTO,
					.tx_phys = BLE_GAP_PHY_AUTO,
				};
				err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
				APP_ERROR_CHECK(err_code);
			} break;

		// case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
		// 	{
		// 		// Pairing not supported
		// 		err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
		// 		APP_ERROR_CHECK(err_code);
		// 	} break;

		// case BLE_GATTS_EVT_SYS_ATTR_MISSING:
		// 	{
		// 		// No system attributes have been stored.
		// 		err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
		// 		APP_ERROR_CHECK(err_code);
		// 	} break;

		case BLE_GATTC_EVT_TIMEOUT:
			{
				// Disconnect on GATT Client timeout event.
				NRF_LOG_INFO("GATT Client Timeout.");
				err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
												 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
				APP_ERROR_CHECK(err_code);
			} break;

		case BLE_GATTS_EVT_TIMEOUT:
			{
				// Disconnect on GATT Server timeout event.
				NRF_LOG_INFO("GATT Server Timeout.");
				err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
												 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
				APP_ERROR_CHECK(err_code);
			} break;

		default:
			{
				// No implementation needed.
			} break;
	}
}


/**@brief Function for the SoftDevice initialization.
 *
 * @details This function initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
	ret_code_t err_code;

	err_code = nrf_sdh_enable_request();
	APP_ERROR_CHECK(err_code);

	// Configure the BLE stack using the default settings.
	// Fetch the start address of the application RAM.
	uint32_t ram_start = 0;
	err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
	APP_ERROR_CHECK(err_code);

	// Enable BLE stack.
	err_code = nrf_sdh_ble_enable(&ram_start);
	APP_ERROR_CHECK(err_code);

	// Register a handler for BLE events.
	NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}


// /**@brief Function for handling events from the GATT library. */
// static void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
// {
// 	if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
// 	{
// 		m_ble_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
// 		NRF_LOG_INFO("Data len is set to 0x%X(%d)", m_ble_max_data_len, m_ble_max_data_len);
// 	}
// 	NRF_LOG_INFO("ATT MTU exchange completed. central 0x%x peripheral 0x%x",
// 				  p_gatt->att_mtu_desired_central,
// 				  p_gatt->att_mtu_desired_periph);
// }


/**@brief Function for initializing the GATT library. */
static void gatt_init(void)
{
	ret_code_t err_code;

	err_code = nrf_ble_gatt_init(&m_gatt, NULL); // gatt_evt_handler);
	APP_ERROR_CHECK(err_code);

	err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
	APP_ERROR_CHECK(err_code);
}


/**@brief Function for the Event Scheduler initialization.
 */
static void scheduler_init(void)
{
	APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}


/**@brief Fetch the list of peer manager peer IDs.
 *
 * @param[inout] p_peers   The buffer where to store the list of peer IDs.
 * @param[inout] p_size    In: The size of the @p p_peers buffer.
 *                         Out: The number of peers copied in the buffer.
 */
static void peer_list_get(pm_peer_id_t * p_peers, uint32_t * p_size)
{
	pm_peer_id_t   peer_id;
	ret_code_t     peers_to_copy;
	NRF_LOG_INFO("Get peer list");

	peers_to_copy = (*p_size < BLE_GAP_WHITELIST_ADDR_MAX_COUNT) ?
					 *p_size : BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

	peer_id = pm_next_peer_id_get(PM_PEER_ID_INVALID);
	*p_size = 0;

	while ((peer_id != PM_PEER_ID_INVALID) && (peers_to_copy--))
	{
		p_peers[(*p_size)++] = peer_id;
		peer_id = pm_next_peer_id_get(peer_id);
	}
}


/**@brief Clear bond information from persistent storage.
 */
void delete_bonds(void)
{
	ret_code_t err_code;
	NRF_LOG_INFO("Erase bonds!");
	sd_ble_gap_adv_stop(m_advertising.adv_handle);

	err_code = pm_peers_delete();
	APP_ERROR_CHECK(err_code);
	// sd_ble_gap_adv_start(m_advertising.adv_handle, m_advertising.conn_cfg_tag);
}


/**@brief Function for starting advertising.
 */
void start_ble_advertising(bool erase_bonds)
{
	if (erase_bonds == true)
	{
		delete_bonds();
		// Advertising is started by PM_EVT_PEERS_DELETE_SUCCEEDED event.
	} else {
		ret_code_t ret;

		memset(m_whitelist_peers, PM_PEER_ID_INVALID, sizeof(m_whitelist_peers));
		m_whitelist_peer_cnt = (sizeof(m_whitelist_peers) / sizeof(pm_peer_id_t));

		peer_list_get(m_whitelist_peers, &m_whitelist_peer_cnt);

		ret = pm_whitelist_set(m_whitelist_peers, m_whitelist_peer_cnt);
		APP_ERROR_CHECK(ret);

		// Setup the device identies list.
		// Some SoftDevices do not support this feature.
		ret = pm_device_identities_list_set(m_whitelist_peers, m_whitelist_peer_cnt);
		if (ret != NRF_ERROR_NOT_SUPPORTED)
		{
			APP_ERROR_CHECK(ret);
		}

		ret = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
		APP_ERROR_CHECK(ret);
	}
}


void ble_restart_without_whitelist(void)
{
	uint32_t err_code;
	if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
	{
		err_code = ble_advertising_restart_without_whitelist(&m_advertising);
		if (err_code != NRF_ERROR_INVALID_STATE)
		{
			APP_ERROR_CHECK(err_code);
		}
	}
}


/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
	ret_code_t err_code;

	pm_handler_on_pm_evt(p_evt);
	pm_handler_flash_clean(p_evt);

	switch (p_evt->evt_id)
	{
		case PM_EVT_CONN_SEC_SUCCEEDED:
			{
				m_peer_id = p_evt->peer_id;

				// Discover peer's services.
				err_code  = ble_db_discovery_start(&m_ble_db_discovery, p_evt->conn_handle);
				APP_ERROR_CHECK(err_code);
			} break;

		case PM_EVT_PEERS_DELETE_SUCCEEDED:
			{
				start_ble_advertising(false);
			} break;

		case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
			{
				// Note: You should check on what kind of white list policy your application should use.
				if (     p_evt->params.peer_data_update_succeeded.flash_changed
					 && (p_evt->params.peer_data_update_succeeded.data_id == PM_PEER_DATA_ID_BONDING))
				{
					NRF_LOG_INFO("New Bond, add the peer to the whitelist if possible");
					NRF_LOG_INFO("\tm_whitelist_peer_cnt %d, MAX_PEERS_WLIST %d",
								  m_whitelist_peer_cnt + 1,
								  BLE_GAP_WHITELIST_ADDR_MAX_COUNT);

					if (m_whitelist_peer_cnt < BLE_GAP_WHITELIST_ADDR_MAX_COUNT)
					{
						// Bonded to a new peer, add it to the whitelist.
						m_whitelist_peers[m_whitelist_peer_cnt++] = m_peer_id;

						// The whitelist has been modified, update it in the Peer Manager.
						err_code = pm_device_identities_list_set(m_whitelist_peers, m_whitelist_peer_cnt);

						if (err_code == BLE_ERROR_GAP_DEVICE_IDENTITIES_DUPLICATE)
						{
							m_whitelist_peer_cnt--;
						}

						if ((err_code != NRF_ERROR_NOT_SUPPORTED) && 
							(err_code != BLE_ERROR_GAP_DEVICE_IDENTITIES_DUPLICATE))
						{
							APP_ERROR_CHECK(err_code);
						}						err_code = pm_whitelist_set(m_whitelist_peers, m_whitelist_peer_cnt);
						APP_ERROR_CHECK(err_code);
					}
				}
			} break;
		case PM_EVT_CONN_SEC_CONFIG_REQ:
			{
				// Reject pairing request from an already bonded peer.
				pm_conn_sec_config_t conn_sec_config = {.allow_repairing = true};
				pm_conn_sec_config_reply(m_conn_handle, &conn_sec_config);
			} break;
		case PM_EVT_CONN_SEC_FAILED:
			{
			} break;

		case PM_EVT_PEER_DELETE_SUCCEEDED:
			{
			} break;

		case PM_EVT_PEER_DELETE_FAILED:
			{
			} break;

		default:
			{
			} break;
	}
}


/**@brief Function for the Peer Manager initialization.
 */
static void peer_manager_init(void)
{
	ble_gap_sec_params_t sec_param;
	ret_code_t           err_code;

	err_code = pm_init();
	APP_ERROR_CHECK(err_code);

	memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

	// Security parameters to be used for all security procedures.
	sec_param.bond           = SEC_PARAM_BOND;
	sec_param.mitm           = SEC_PARAM_MITM;
	sec_param.lesc           = SEC_PARAM_LESC;
	sec_param.keypress       = SEC_PARAM_KEYPRESS;
	sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
	sec_param.oob            = SEC_PARAM_OOB;
	sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
	sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
	sec_param.kdist_own.enc  = 1;
	sec_param.kdist_own.id   = 1;
	sec_param.kdist_peer.enc = 1;
	sec_param.kdist_peer.id  = 1;

	err_code = pm_sec_params_set(&sec_param);
	APP_ERROR_CHECK(err_code);

	err_code = pm_register(pm_evt_handler);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Database Discovery events.
 *
 * @details This function is a callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function should forward the events
 *          to their respective service instances.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
	ble_cts_c_on_db_disc_evt(&m_cts_c, p_evt);
}


/**
 * @brief Database discovery collector initialization.
 */
static void db_discovery_init(void)
{
	ble_db_discovery_init_t db_init;

	memset(&db_init, 0, sizeof(ble_db_discovery_init_t));

	db_init.evt_handler  = db_disc_handler;
	db_init.p_gatt_queue = &m_ble_gatt_queue;

	ret_code_t err_code = ble_db_discovery_init(&db_init);
	APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
	ret_code_t err_code;

	switch (ble_adv_evt)
	{
		case BLE_ADV_EVT_FAST:
			{
				NRF_LOG_INFO("Fast advertising");
//				err_code = indication_set(INDICATE_ADVERTISING);
//				APP_ERROR_CHECK(err_code);
			} break;

		case BLE_ADV_EVT_SLOW:
			{
				NRF_LOG_INFO("Slow advertising");
//				err_code = indication_set(INDICATE_ADVERTISING_SLOW);
//				APP_ERROR_CHECK(err_code);
			} break;

		case BLE_ADV_EVT_FAST_WHITELIST:
			{
				NRF_LOG_INFO("Fast advertising with WhiteList");
//				err_code = indication_set(INDICATE_ADVERTISING_WHITELIST);
//				APP_ERROR_CHECK(err_code);
			} break;

		case BLE_ADV_EVT_SLOW_WHITELIST:
			{
				NRF_LOG_INFO("Slow advertising with WhiteList");
//				err_code = indication_set(INDICATE_ADVERTISING_WHITELIST);
//				APP_ERROR_CHECK(err_code);
				err_code = ble_advertising_restart_without_whitelist(&m_advertising);
				APP_ERROR_CHECK(err_code);
			} break;

		case BLE_ADV_EVT_IDLE:
			{
				// sleep_mode_enter();
			} break;

		case BLE_ADV_EVT_WHITELIST_REQUEST:
			{
				ble_gap_addr_t whitelist_addrs[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
				ble_gap_irk_t  whitelist_irks[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
				uint32_t       addr_cnt = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;
				uint32_t       irk_cnt  = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

				err_code = pm_whitelist_get(whitelist_addrs, &addr_cnt,
											whitelist_irks,  &irk_cnt);
				APP_ERROR_CHECK(err_code);
				NRF_LOG_INFO("pm_whitelist_get returns %d addr in whitelist and %d irk whitelist",
							   addr_cnt,
							   irk_cnt);

				// Apply the whitelist.
				err_code = ble_advertising_whitelist_reply(&m_advertising,
														   whitelist_addrs,
														   addr_cnt,
														   whitelist_irks,
														   irk_cnt);
				APP_ERROR_CHECK(err_code);
			} break;

		default:
			{
			} break;
	}
}

/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
	ret_code_t             err_code;
	ble_advertising_init_t init;

	memset(&init, 0, sizeof(init));

	init.advdata.name_type                = BLE_ADVDATA_FULL_NAME;
	init.advdata.include_appearance       = true;
	init.advdata.flags                    = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;
	init.advdata.uuids_solicited.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
	init.advdata.uuids_solicited.p_uuids  = m_adv_uuids;

	init.config.ble_adv_whitelist_enabled = true;
	init.config.ble_adv_fast_enabled      = true;
	init.config.ble_adv_fast_interval     = APP_ADV_FAST_INTERVAL;
	init.config.ble_adv_fast_timeout      = APP_ADV_FAST_DURATION;
	init.config.ble_adv_slow_enabled      = true;
	init.config.ble_adv_slow_interval     = APP_ADV_SLOW_INTERVAL;
	init.config.ble_adv_slow_timeout      = APP_ADV_SLOW_DURATION;

	init.evt_handler = on_adv_evt;

	err_code = ble_advertising_init(&m_advertising, &init);
	APP_ERROR_CHECK(err_code);

	ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

void ble_init(void)
{
	scheduler_init();
	ble_stack_init();
	gap_params_init();
	gatt_init();
	db_discovery_init();
	advertising_init();
	peer_manager_init();
	services_init();
	conn_params_init();
}

// /**@brief Function for starting advertising.
//  */
// void start_ble_advertising(void)
// {
// 	uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
// 	APP_ERROR_CHECK(err_code);
// }
