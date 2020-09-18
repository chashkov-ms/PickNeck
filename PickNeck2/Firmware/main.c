#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"

#include "app_util_platform.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_drv_spi.h"

#include "ble.h"
#include "ble_init.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "led.h"
#include "led_action.h"
#include "button.h"
#include "button_action.h"

#include "uart.h"
#include "rtc.h"
#include "tim.h"



extern uint16_t   m_conn_handle;


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
        uint32_t err_code;
//	err_code = indication_set(INDICATE_IDLE);
//	APP_ERROR_CHECK(err_code);

	buttons_disable();

	// Prepare wakeup buttons.
	err_code = wakeup_button_enable(INDEX_BUTTON_0);
	APP_ERROR_CHECK(err_code);

	// Go to system-off mode (this function will not return; wakeup will cause a reset).
	err_code = sd_power_system_off();
	APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
void event_handler(event_t event)
{
	uint32_t err_code;
	switch (event)
	{
		case EVENT_SLEEP:
			{
				sleep_mode_enter();
			} break;

		case EVENT_DISCONNECT:
			{
				err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
				if (err_code != NRF_ERROR_INVALID_STATE)
				{
					APP_ERROR_CHECK(err_code);
				}
			} break;

		case EVENT_WHITELIST_OFF:
			{
				ble_restart_without_whitelist();
			} break;

		case EVENT_KEY_1:
			{
				led_invert(INDEX_LED_1);
				get_time(err_code);
			} break;

		case EVENT_KEY_2:
			{
				led_invert(INDEX_LED_2);
				delete_bonds();
			} break;

		case EVENT_KEY_3:
			{
				led_invert(INDEX_LED_3);
				module_time_print();
				// imu_measurement_send();
			} break;

		default:
			{
			} break;
	}
}

/**
 * @brief SPI user event handler.
 * @param event
 */
void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
    uint8_t imu_data [2];    /**< RX_spi buffer. */
    memset(imu_data, 0, 2);

    get_spi_rx(imu_data);
    NRF_LOG_INFO("Transfer completed.");
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(bool * p_erase_bonds)
{
	event_t startup_event;

	uint32_t err_code = buttons_init(NULL, &startup_event, event_handler);
	APP_ERROR_CHECK(err_code);

	err_code = leds_action_init();
	APP_ERROR_CHECK(err_code);

	*p_erase_bonds = (startup_event == EVENT_CLEAR_BONDING_DATA);
}


/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
	ret_code_t err_code = NRF_LOG_INIT(NULL);
	APP_ERROR_CHECK(err_code);

	NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
	ret_code_t err_code;
	err_code = nrf_pwr_mgmt_init();
	APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
	if (NRF_LOG_PROCESS() == false)
	{
		nrf_pwr_mgmt_run();
	}
}


/**@brief Application main function.
 */
int main(void)
{
	bool erase_bonds;

	// Initialize.

	log_init();
	NRF_LOG_INFO("Debug logging for UART over RTT started.");

	//uart_init();
	//printf("\r\nUART started.\r\n");

	timers_init();
	buttons_leds_init(&erase_bonds);
	power_management_init();

	ble_init();

	initial_calendar();
	rtc2_config();

	// Start execution.
	application_timers_start();
	start_ble_advertising(erase_bonds);

	// Enter main loop.
	while(1)
	{
		idle_state_handle();
	}
}


/**
 * @}
 */