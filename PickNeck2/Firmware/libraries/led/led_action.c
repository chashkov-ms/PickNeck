#include "led_action.h"
#include "led.h"
#include "app_timer.h"
#include <stdint.h>
#include <stdbool.h>


static indication_t m_stable_state        = INDICATE_IDLE;
static bool             m_leds_clear          = false;
static bool             m_alert_on            = false;
APP_TIMER_DEF(m_leds_tmr);
APP_TIMER_DEF(m_alert_tmr);


static void alert_leds_off(void)
{
	if (m_alert_on)
	{
		uint32_t i;
		for (i = 0; i < LEDS_NUMBER; i++)
		{
			if (i != LED_ALERT)
			{
				led_off(i);
			}
		}
	}
	else
	{
		leds_off();
	}
}


/**@brief       Configure leds to indicate required state.
 * @param[in]   indicate   State to be indicated.
 */
static uint32_t led_indication(indication_t indicate)
{
	uint32_t err_code   = NRF_SUCCESS;
	uint32_t next_delay = 0;

	if (m_leds_clear)
	{
		m_leds_clear = false;
		alert_leds_off();
	}

	switch (indicate)
	{
		case INDICATE_IDLE:
			alert_leds_off();
			err_code       = app_timer_stop(m_leds_tmr);
			m_stable_state = indicate;
			break;

		case INDICATE_SCANNING:
			break;
		case INDICATE_ADVERTISING:
				// in advertising blink LED_0
				if (led_state_get(LED_INDICATE_INDICATE_ADVERTISING))
				{
					led_off(LED_INDICATE_INDICATE_ADVERTISING);
					next_delay = indicate ==
								 INDICATE_ADVERTISING ? ADVERTISING_LED_OFF_INTERVAL :
								 ADVERTISING_SLOW_LED_OFF_INTERVAL;
				}
				else
				{
					led_on(LED_INDICATE_INDICATE_ADVERTISING);
					next_delay = indicate ==
								 INDICATE_ADVERTISING ? ADVERTISING_LED_ON_INTERVAL :
								 ADVERTISING_SLOW_LED_ON_INTERVAL;
				}

				m_stable_state = indicate;
				err_code       = app_timer_start(m_leds_tmr, APP_TIMER_TICKS(next_delay), NULL);
			break;

		case INDICATE_ADVERTISING_WHITELIST:
				// in advertising quickly blink LED_0
				if (led_state_get(LED_INDICATE_ADVERTISING_WHITELIST))
				{
					led_off(LED_INDICATE_ADVERTISING_WHITELIST);
					next_delay = indicate ==
								 INDICATE_ADVERTISING_WHITELIST ?
								 ADVERTISING_WHITELIST_LED_OFF_INTERVAL :
								 ADVERTISING_SLOW_LED_OFF_INTERVAL;
				}
				else
				{
					led_on(LED_INDICATE_ADVERTISING_WHITELIST);
					next_delay = indicate ==
								 INDICATE_ADVERTISING_WHITELIST ?
								 ADVERTISING_WHITELIST_LED_ON_INTERVAL :
								 ADVERTISING_SLOW_LED_ON_INTERVAL;
				}
				m_stable_state = indicate;
				err_code       = app_timer_start(m_leds_tmr, APP_TIMER_TICKS(next_delay), NULL);
			break;

		case INDICATE_ADVERTISING_SLOW:
				// in advertising slowly blink LED_0
				if (led_state_get(LED_INDICATE_ADVERTISING_SLOW))
				{
					led_off(LED_INDICATE_ADVERTISING_SLOW);
					next_delay = indicate ==
								 INDICATE_ADVERTISING_SLOW ? ADVERTISING_SLOW_LED_OFF_INTERVAL :
								 ADVERTISING_SLOW_LED_OFF_INTERVAL;
				}
				else
				{
					led_on(LED_INDICATE_ADVERTISING_SLOW);
					next_delay = indicate ==
								 INDICATE_ADVERTISING_SLOW ? ADVERTISING_SLOW_LED_ON_INTERVAL :
								 ADVERTISING_SLOW_LED_ON_INTERVAL;
				}
				m_stable_state = indicate;
				err_code       = app_timer_start(m_leds_tmr, APP_TIMER_TICKS(next_delay), NULL);
			break;

		case INDICATE_ADVERTISING_DIRECTED:
				// in advertising very quickly blink LED_0
				if (led_state_get(LED_INDICATE_ADVERTISING_DIRECTED))
				{
					led_off(LED_INDICATE_ADVERTISING_DIRECTED);
					next_delay = indicate ==
								 INDICATE_ADVERTISING_DIRECTED ?
								 ADVERTISING_DIRECTED_LED_OFF_INTERVAL :
								 ADVERTISING_SLOW_LED_OFF_INTERVAL;
				}
				else
				{
					led_on(LED_INDICATE_ADVERTISING_DIRECTED);
					next_delay = indicate ==
								 INDICATE_ADVERTISING_DIRECTED ?
								 ADVERTISING_DIRECTED_LED_ON_INTERVAL :
								 ADVERTISING_SLOW_LED_ON_INTERVAL;
				}
				m_stable_state = indicate;
				err_code       = app_timer_start(m_leds_tmr, APP_TIMER_TICKS(next_delay), NULL);
			break;

		case INDICATE_BONDING:
				// in bonding fast blink LED_0
				led_invert(LED_INDICATE_BONDING);

				m_stable_state = indicate;
				err_code       =
					app_timer_start(m_leds_tmr, APP_TIMER_TICKS(BONDING_INTERVAL), NULL);
			break;

		case INDICATE_CONNECTED:
				led_on(LED_INDICATE_CONNECTED);
				m_stable_state = indicate;
			break;

		case INDICATE_SENT_OK:
				// when sending shortly invert LED_1
				m_leds_clear = true;
				led_invert(LED_INDICATE_SENT_OK);
				err_code = app_timer_start(m_leds_tmr, APP_TIMER_TICKS(SENT_OK_INTERVAL), NULL);
			break;

		case INDICATE_SEND_ERROR:
				// on receving error invert LED_1 for long time
				m_leds_clear = true;
				led_invert(LED_INDICATE_SEND_ERROR);
				err_code = app_timer_start(m_leds_tmr, APP_TIMER_TICKS(SEND_ERROR_INTERVAL), NULL);
			break;

		case INDICATE_RCV_OK:
				// when receving shortly invert LED_1
				m_leds_clear = true;
				led_invert(LED_INDICATE_RCV_OK);
				err_code = app_timer_start(m_leds_tmr, APP_TIMER_TICKS(RCV_OK_INTERVAL), NULL);
			break;

		case INDICATE_RCV_ERROR:
				// on receving error invert LED_1 for long time
				m_leds_clear = true;
				led_invert(LED_INDICATE_RCV_ERROR);
				err_code = app_timer_start(m_leds_tmr, APP_TIMER_TICKS(RCV_ERROR_INTERVAL), NULL);
			break;

		case INDICATE_FATAL_ERROR:
				// on fatal error turn on all leds
				leds_on();
				m_stable_state = indicate;
			break;

		case INDICATE_ALERT_0:
			break;
		case INDICATE_ALERT_1:
			break;
		case INDICATE_ALERT_2:
			break;
		case INDICATE_ALERT_3:
			break;
		case INDICATE_ALERT_OFF:
				err_code   = app_timer_stop(m_alert_tmr);
				next_delay = (uint32_t)INDICATE_ALERT_OFF - (uint32_t)indicate;

				// a little trick to find out that if it did not fall through ALERT_OFF
				if (next_delay && (err_code == NRF_SUCCESS))
				{
					if (next_delay > 1)
					{
						err_code = app_timer_start(m_alert_tmr,
												   APP_TIMER_TICKS(((uint16_t)next_delay * ALERT_INTERVAL)),
												   NULL);
					}
					led_on(LED_ALERT);
					m_alert_on = true;
				}
				else
				{
					led_off(LED_ALERT);
					m_alert_on = false;
				}
			break;

		case INDICATE_USER_STATE_OFF:
				alert_leds_off();
				m_stable_state = indicate;
			break;

		case INDICATE_USER_STATE_0:
				alert_leds_off();
				led_on(INDEX_LED_0);
				m_stable_state = indicate;
			break;

		case INDICATE_USER_STATE_1:
				alert_leds_off();
				led_on(INDEX_LED_1);
				m_stable_state = indicate;
			break;

		case INDICATE_USER_STATE_2:
				alert_leds_off();
				led_on(INDEX_LED_0);
				led_on(INDEX_LED_1);
				m_stable_state = indicate;
			break;

		case INDICATE_USER_STATE_3:
			break;

		case INDICATE_USER_STATE_ON:
				leds_on();
				m_stable_state = indicate;
			break;

		default:
			break;
	}

	return err_code;
}


/**@brief Handle events from leds timer.
 *
 * @note Timer handler does not support returning an error code.
 * Errors from led_indication() are not propagated.
 *
 * @param[in]   p_context   parameter registered in timer start function.
 */
static void leds_timer_handler(void * p_context)
{
	UNUSED_PARAMETER(p_context);

	UNUSED_VARIABLE(led_indication(m_stable_state));
}


/**@brief Handle events from alert timer.
 *
 * @param[in]   p_context   parameter registered in timer start function.
 */
static void alert_timer_handler(void * p_context)
{
	UNUSED_PARAMETER(p_context);
	led_invert(LED_ALERT);
}


/**@brief Configure indicators to required state.
 */
uint32_t indication_set(indication_t indicate)
{
	uint32_t err_code = NRF_SUCCESS;

	err_code = led_indication(indicate);

	return err_code;
}


uint32_t leds_action_init(void)
{
	uint32_t err_code = NRF_SUCCESS;

#if LEDS_NUMBER > 0
	leds_init();

	// timers module must be already initialized!
	if (err_code == NRF_SUCCESS)
	{
		err_code = app_timer_create(&m_leds_tmr, APP_TIMER_MODE_SINGLE_SHOT, leds_timer_handler);
	}

	if (err_code == NRF_SUCCESS)
	{
		err_code = app_timer_create(&m_alert_tmr, APP_TIMER_MODE_REPEATED, alert_timer_handler);
	}
#endif // LEDS_NUMBER > 0

	return err_code;
}