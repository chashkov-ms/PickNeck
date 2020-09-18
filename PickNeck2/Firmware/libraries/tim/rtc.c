#include "rtc.h"
#include "pin_config.h"
#include "nrf_gpio.h"
#include "nrf_drv_rtc.h"

#include "nrf_log.h"
#include "ble_cts_c.h"


#define COMPARE_COUNTERTIME  (3UL)                 /**< Get Compare event COMPARE_TIME seconds after the counter starts from 0. */
#define RTC_PRESCALER 2047                         /**< 32786 (Гц) / 4096 (max) = 8 (0.125 мс) */

static const uint8_t max_fractions_seconds = (uint8_t) (RTC_DEFAULT_CONFIG_FREQUENCY / RTC_PRESCALER);
static const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(2); /**< Declaring an instance of nrf_drv_rtc for RTC2. */


typedef struct
{
	uint8_t fractions_seconds;
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t date;
	uint8_t month;
	uint16_t year;
} system_time;

system_time time;

void initial_calendar(void)
{
	memset(&time, 0, sizeof(time));
	time.fractions_seconds = 0;
	time.seconds = 10;
	time.minutes = 59;
	time.hours = 23;
	time.date = 30;
	time.month = 10;
	time.year = 2020;
}

static uint16_t not_leap(void)      //check for leap year
{
	if (!(time.year % 100))
	{
		return (uint16_t)(time.year % 400);
	} else {
		return (uint16_t)(time.year % 4);
	}
}

/** @brief: Function for handling the RTC2 interrupts.
 * Triggered on TICK and COMPARE2 match.
 */
static void rtc2_handler(nrf_drv_rtc_int_type_t int_type)
{
	// if (int_type == NRF_DRV_RTC_INT_COMPARE0)
	// {
	// 	nrf_gpio_pin_toggle(LED_3);
	// }
	// else if (int_type == NRF_DRV_RTC_INT_TICK)
	// {
	// 	nrf_gpio_pin_toggle(LED_4);
	// }
	if (int_type == NRF_DRV_RTC_INT_TICK)
	{
		if (++time.fractions_seconds == max_fractions_seconds)
		{
			time.fractions_seconds = 0;
//			nrf_gpio_pin_toggle(LED_4);
			if (++time.seconds == 60)
			{
				time.seconds = 0;
				if (++time.minutes == 60)
				{
					time.minutes = 0;
					if (++time.hours == 24)
					{
						time.hours = 0;
						if (++time.date == 32)
						{
							time.month++;
							time.date = 1;
						}
						else if (time.date == 31)
						{
							if ((time.month == 4) || (time.month == 6) || (time.month == 9) || (time.month == 11))
							{
								time.month++;
								time.date = 1;
							}
						}
						else if (time.date == 30)
						{
							if (time.month == 2)
							{
								time.month++;
								time.date = 1;
							}
						}
						else if (time.date == 29)
						{
							if ((time.month == 2) && (not_leap()))
							{
								time.month++;
								time.date = 1;
							}
						}
						if (time.month == 13)
						{
							time.month = 1;
							time.year++;
						}
					}
				}
			}
		}
	}
}

/** @brief Function initialization and configuration of RTC driver instance.
 */
void rtc2_config(void)
{
	uint32_t err_code;

	// Initialize RTC instance
	nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
	config.prescaler = RTC_PRESCALER;
	err_code = nrf_drv_rtc_init(&rtc, &config, rtc2_handler);
	APP_ERROR_CHECK(err_code);

	// Enable tick event & interrupt
	nrf_drv_rtc_tick_enable(&rtc, true);

	// Set compare channel to trigger interrupt after COMPARE_COUNTERTIME secondss
	err_code = nrf_drv_rtc_cc_set(&rtc, 0, COMPARE_COUNTERTIME * 8, true);
	APP_ERROR_CHECK(err_code);

	// Power on RTC instance
	nrf_drv_rtc_enable(&rtc);
}


void module_time_print(void)
{
	NRF_LOG_INFO("\r\nDate:");

	NRF_LOG_INFO("\tDay %d", time.date);
	NRF_LOG_INFO("\tMonth %d", time.month);
	NRF_LOG_INFO("\tYear %d", time.year);

	NRF_LOG_INFO("\r\nTime:");

	NRF_LOG_INFO("\tHours     %d", time.hours);
	NRF_LOG_INFO("\tMinutes   %d", time.minutes);
	NRF_LOG_INFO("\tSeconds   %d", time.seconds);
}

void set_calendar(ble_cts_c_evt_t * p_evt)
{
	time.fractions_seconds = 0;
	time.seconds = p_evt->params.current_time.exact_time_256.day_date_time.date_time.seconds;
	time.minutes = p_evt->params.current_time.exact_time_256.day_date_time.date_time.minutes;
	time.hours = p_evt->params.current_time.exact_time_256.day_date_time.date_time.hours;
	time.date = p_evt->params.current_time.exact_time_256.day_date_time.date_time.day;
	time.month = p_evt->params.current_time.exact_time_256.day_date_time.date_time.month;
	time.year = p_evt->params.current_time.exact_time_256.day_date_time.date_time.year;
}
