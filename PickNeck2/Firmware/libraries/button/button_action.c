#include "button_action.h"
#include <stddef.h>
#include <stdio.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_error.h"

#include "button.h"
#include "led.h"

#include "app_timer.h"
#include "app_button.h"

#include "nrf_sdh_ble.h"

#define BUTTON_MEAS_INTERVAL     APP_TIMER_TICKS(50)                      /**< Button measurement interval (MS). */

#define BTN_ID_WAKEUP             0  /**< ID of button used to wake up the application. */
#define BTN_ID_SLEEP              0  /**< ID of button used to put the application into sleep mode. */
#define BTN_ID_DISCONNECT         0  /**< ID of button used to gracefully terminate a connection on long press. */
#define BTN_ID_WAKEUP_BOND_DELETE 1  /**< ID of button used to wake up the application and delete all bonding information. */
#define BTN_ID_WHITELIST_OFF      1  /**< ID of button used to turn off usage of the whitelist. */

#define BTN_ACTION_SLEEP          BUTTON_ACTION_RELEASE    /**< Button action used to put the application into sleep mode. */
#define BTN_ACTION_DISCONNECT     BUTTON_ACTION_RELEASE  /**< Button action used to gracefully terminate a connection on long press. */
#define BTN_ACTION_WHITELIST_OFF  BUTTON_ACTION_LONG_PUSH  /**< Button action used to turn off usage of the whitelist. */


static btn_error_handler_t    m_error_handler = NULL; /**< Error handler registered by the user. */
static uint32_t                   m_num_connections = 0;  /**< Number of connections the device is currently in. */
static event_callback_t           m_registered_callback         = NULL;
static button_event_config_t      m_events_list[BUTTONS_NUMBER] = {{EVENT_NOTHING, EVENT_NOTHING}};
APP_TIMER_DEF(m_button_tmr);

static void button_event_handler(uint8_t pin_no, uint8_t button_action);

static const app_button_cfg_t app_buttons[BUTTONS_NUMBER] =
{
	#ifdef BUTTON_1
		{BUTTON_1, false, BUTTON_PULL, button_event_handler},
	#endif // BUTTON_1

	#ifdef BUTTON_2
		{BUTTON_2, false, BUTTON_PULL, button_event_handler},
	#endif // BUTTON_2

	#ifdef BUTTON_3
		{BUTTON_3, false, BUTTON_PULL, button_event_handler},
	#endif // BUTTON_3

	#ifdef BUTTON_4
		{BUTTON_4, false, BUTTON_PULL, button_event_handler},
	#endif // BUTTON_4

};


/**@brief Function for configuring the buttons for connection.
 *
 * @retval NRF_SUCCESS  Configured successfully.
 * @return A propagated error code.
 */
static uint32_t connection_buttons_configure()
{
	uint32_t err_code;

	// err_code = event_to_button_action_assign(BTN_ID_SLEEP, BTN_ACTION_SLEEP, EVENT_DEFAULT);
	// RETURN_ON_ERROR_NOT_INVALID_PARAM(err_code);

	// err_code = event_to_button_action_assign(BTN_ID_WHITELIST_OFF, BTN_ACTION_WHITELIST_OFF, EVENT_WHITELIST_OFF);
	// RETURN_ON_ERROR_NOT_INVALID_PARAM(err_code);

	err_code = event_to_button_action_assign(BTN_ID_DISCONNECT, BTN_ACTION_DISCONNECT, EVENT_DISCONNECT);
	RETURN_ON_ERROR_NOT_INVALID_PARAM(err_code);

	return NRF_SUCCESS;
}


/**@brief Function for configuring the buttons for advertisement.
 *
 * @retval NRF_SUCCESS  Configured successfully.
 * @return A propagated error code.
 */
static uint32_t advertising_buttons_configure()
{
	uint32_t err_code;

	// err_code = event_to_button_action_assign(BTN_ID_DISCONNECT, BTN_ACTION_DISCONNECT, EVENT_DEFAULT);
	// RETURN_ON_ERROR_NOT_INVALID_PARAM(err_code);

	// err_code = event_to_button_action_assign(BTN_ID_WHITELIST_OFF, BTN_ACTION_WHITELIST_OFF, EVENT_WHITELIST_OFF);
	// RETURN_ON_ERROR_NOT_INVALID_PARAM(err_code);

	err_code = event_to_button_action_assign(BTN_ID_SLEEP, BTN_ACTION_SLEEP, EVENT_SLEEP);
	RETURN_ON_ERROR_NOT_INVALID_PARAM(err_code);

	return NRF_SUCCESS;
}


/**@brief Function for extracting the BSP event valid at startup.
 *
 * @details When a button was used to wake up the device, the button press will not generate an
 *          interrupt. This function reads which button was pressed at startup, and returns the
 *          appropriate BSP event.
 *
 * @param[out] p_startup_event  Where to put the extracted BSP event.
 */
static void startup_event_extract(event_t * p_startup_event)
{
	// React to button states
	if (button_state_get(BTN_ID_WAKEUP_BOND_DELETE))
	{
		*p_startup_event = EVENT_CLEAR_BONDING_DATA;
	}
	else if (button_state_get(BTN_ID_WAKEUP))
	{
		*p_startup_event = EVENT_WAKEUP;
	}
	else
	{
		*p_startup_event = EVENT_NOTHING;
	}
}


/**@brief Function for handling button events.
 *
 * @param[in]   pin_no          The pin number of the button pressed.
 * @param[in]   button_action   Action button.
 */
static void button_event_handler(uint8_t pin_no, uint8_t button_action)
{
	event_t          event  = EVENT_NOTHING;
	uint32_t         button = 0;
	uint32_t         err_code;
	static uint8_t   current_long_push_pin_no;              /**< Pin number of a currently pushed button, that could become a long push if held long enough. */
	static event_t   release_event_at_push[BUTTONS_NUMBER]; /**< Array of what the release event of each button was last time it was pushed, so that no release event is sent if the event was bound after the push of the button. */

	button = pin_to_button_idx(pin_no);

	if (button < BUTTONS_NUMBER)
	{
		switch (button_action)
		{
			case APP_BUTTON_PUSH:
					event = m_events_list[button].push_event;
					if (m_events_list[button].long_push_event != EVENT_NOTHING)
					{
						err_code = app_timer_start(m_button_tmr,
													APP_TIMER_TICKS(LONG_PUSH_TIMEOUT_MS),
													(void*)&current_long_push_pin_no);
						if (err_code == NRF_SUCCESS)
						{
							current_long_push_pin_no = pin_no;
						}
					}
					release_event_at_push[button] = m_events_list[button].release_event;
				break;
			case APP_BUTTON_RELEASE:
					(void)app_timer_stop(m_button_tmr);
					if (release_event_at_push[button] == m_events_list[button].release_event)
					{
						event = m_events_list[button].release_event;
					}
				break;
			case BUTTON_ACTION_LONG_PUSH:
					event = m_events_list[button].long_push_event;
				break;
		}
	}

	if ((event != EVENT_NOTHING) && (m_registered_callback != NULL))
	{
		m_registered_callback(event);
	}
}


/**@brief Handle events from button timer.
 *
 * @param[in]   p_context   parameter registered in timer start function.
 */
static void button_timer_handler(void * p_context)
{
	button_event_handler(*(uint8_t *)p_context, BUTTON_ACTION_LONG_PUSH);
}


uint32_t buttons_init(btn_error_handler_t error_handler, event_t * p_startup_evt, event_callback_t callback)
{
	uint32_t err_code = NRF_SUCCESS;

	m_registered_callback = callback;
	m_error_handler = error_handler;

	uint32_t num;

	for (num = 0; ((num < BUTTONS_NUMBER) && (err_code == NRF_SUCCESS)); num++)
	{
		err_code = event_to_button_action_assign(num, BUTTON_ACTION_PUSH, EVENT_DEFAULT);
	}

	if (err_code == NRF_SUCCESS)
	{
		err_code = app_button_init((app_button_cfg_t *)app_buttons, BUTTONS_NUMBER, BUTTON_MEAS_INTERVAL);
	}

	if (err_code == NRF_SUCCESS)
	{
		err_code = app_button_enable();
	}

	if (err_code == NRF_SUCCESS)
	{
		err_code = app_timer_create(&m_button_tmr, APP_TIMER_MODE_SINGLE_SHOT, button_timer_handler);
	}

	if (p_startup_evt != NULL)
	{
		startup_event_extract(p_startup_evt);
	}

	if (m_num_connections == 0)
	{
		err_code = advertising_buttons_configure();
	}

	return err_code;
}


/**@brief Assign specific event to button.
 */
uint32_t event_to_button_action_assign(uint32_t button, button_action_t action, event_t event)
{
	uint32_t err_code = NRF_SUCCESS;

	if (button < BUTTONS_NUMBER)
	{
		if (event == EVENT_DEFAULT)
		{
			// Setting default action: EVENT_KEY_x for PUSH actions, EVENT_NOTHING for RELEASE and LONG_PUSH actions.
			event = (action == BUTTON_ACTION_PUSH) ? (event_t)(EVENT_KEY_0 + button) : EVENT_NOTHING;
		}
		switch (action)
		{
			case BUTTON_ACTION_PUSH:
				m_events_list[button].push_event = event;
				break;
			case BUTTON_ACTION_LONG_PUSH:
				m_events_list[button].long_push_event = event;
				break;
			case BUTTON_ACTION_RELEASE:
				m_events_list[button].release_event = event;
				break;
			default:
				err_code = NRF_ERROR_INVALID_PARAM;
				break;
		}
	}
	else
	{
		err_code = NRF_ERROR_INVALID_PARAM;
	}

	return err_code;
}


/**
 * @brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt       Event received from the BLE stack.
 * @param[in]   p_context       Context.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
	uint32_t err_code;

	switch (p_ble_evt->header.evt_id)
	{
		case BLE_GAP_EVT_CONNECTED:
			if (m_num_connections == 0)
			{
				err_code = connection_buttons_configure();
				CALL_HANDLER_ON_ERROR(err_code);
			}

			m_num_connections++;
			break;

		case BLE_GAP_EVT_DISCONNECTED:
			m_num_connections--;

			if (m_num_connections == 0)
			{
				err_code = advertising_buttons_configure();
				CALL_HANDLER_ON_ERROR(err_code);
			}
			break;

		default:
			break;
	}
}

NRF_SDH_BLE_OBSERVER(m_ble_observer, BTN_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
