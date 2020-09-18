#include "tim.h"
#include "app_timer.h"
#include "ble_init.h"


#define BATTERY_LEVEL_MEAS_INTERVAL     APP_TIMER_TICKS(10000)                      /**< Battery level measurement interval (MS). */
#define MIN_BATTERY_LEVEL               81                                          /**< Minimum battery level as returned by the simulated measurement function. */
#define MAX_BATTERY_LEVEL               100                                         /**< Maximum battery level as returned by the simulated measurement function. */

APP_TIMER_DEF(m_battery_timer_id);                                                  /**< Battery timer. */


/**@brief Function for handling the Battery measurement timer timeout.
 *
 * @details This function will be called each time the battery level measurement timer expires.
 *
 * @param[in] p_context   Pointer used for passing some arbitrary information (context) from the
 *                        app_start_timer() call to the timeout handler.
 */
static void battery_level_meas_timeout_handler(void * p_context)
{
	UNUSED_PARAMETER(p_context);
	battery_level_update();
}


/**@brief Function for initializing the timer module.
 */
void timers_init(void)
{
	ret_code_t err_code;

	err_code = app_timer_init();
	APP_ERROR_CHECK(err_code);
	// Create timers.
	err_code = app_timer_create(&m_battery_timer_id, APP_TIMER_MODE_REPEATED, battery_level_meas_timeout_handler);
	APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting application timers.
 */
void application_timers_start(void)
{
	ret_code_t err_code;

	// Start application timers.
	err_code = app_timer_start(m_battery_timer_id, BATTERY_LEVEL_MEAS_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code);
}


