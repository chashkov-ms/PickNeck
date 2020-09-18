#ifndef BUTTON_ACTION_H__
#define BUTTON_ACTION_H__

#include <stdint.h>
#include <stdbool.h>


#define LONG_PUSH_TIMEOUT_MS (1000) /**< The time to hold for a long push (in milliseconds). */


/* UART_SUPPORT
 * This define enables UART support module.
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t button_action_t; /**< The different actions that can be performed on a button. */

/**@brief BSP events.
 *
 * @note Events from EVENT_KEY_0 to EVENT_KEY_LAST are by default assigned to buttons.
 */
typedef enum
{
	EVENT_NOTHING = 0,                  /**< Assign this event to an action to prevent the action from generating an event (disable the action). */
	EVENT_DEFAULT,                      /**< Assign this event to an action to assign the default event to the action. */
	EVENT_CLEAR_BONDING_DATA,           /**< Persistent bonding data should be erased. */
	EVENT_CLEAR_ALERT,                  /**< An alert should be cleared. */
	EVENT_DISCONNECT,                   /**< A link should be disconnected. */
	EVENT_ADVERTISING_START,            /**< The device should start advertising. */
	EVENT_ADVERTISING_STOP,             /**< The device should stop advertising. */
	EVENT_WHITELIST_OFF,                /**< The device should remove its advertising whitelist. */
	EVENT_BOND,                         /**< The device should bond to the currently connected peer. */
	EVENT_RESET,                        /**< The device should reset. */
	EVENT_SLEEP,                        /**< The device should enter sleep mode. */
	EVENT_WAKEUP,                       /**< The device should wake up from sleep mode. */
	EVENT_SYSOFF,                       /**< The device should enter system off mode (without wakeup). */
	EVENT_DFU,                          /**< The device should enter DFU mode. */
	EVENT_KEY_0,                        /**< Default event of the push action of BUTTON_0 (only if this button is present). */
	EVENT_KEY_1,                        /**< Default event of the push action of BUTTON_1 (only if this button is present). */
	EVENT_KEY_2,                        /**< Default event of the push action of BUTTON_2 (only if this button is present). */
	EVENT_KEY_3,                        /**< Default event of the push action of BUTTON_3 (only if this button is present). */
} event_t;


typedef struct
{
	event_t push_event;      /**< The event to fire on regular button press. */
	event_t long_push_event; /**< The event to fire on long button press. */
	event_t release_event;   /**< The event to fire on button release. */
} button_event_config_t;


/**@brief BSP module event callback function type.
 *
 * @details     Upon an event in the BSP module, this callback function will be called to notify
 *              the application about the event.
 *
 * @param[in]   event_t BSP event type.
 */
typedef void (* event_callback_t)(event_t);


/**@brief Button Module error handler type. */
typedef void (*btn_error_handler_t) (uint32_t nrf_error);


/**@brief       Function for initializing buttons and buttons actions.
 *
 * @details     The function initializes the board support package to allow state indication and
 *              button reaction. Default events are assigned to buttons.
 * @note        Before calling this function, you must initiate the following required modules:
 *              - @ref app_timer for LED support
 *              - @ref app_gpiote for button support
 *
 * @param[in]   type               Type of peripherals used.
 * @param[in]   callback           Function to be called when button press/event is detected.
 *
 * @retval      NRF_SUCCESS               If the module was successfully initialized.
 * @retval      NRF_ERROR_INVALID_STATE   If the application timer module has not been initialized.
 * @retval      NRF_ERROR_NO_MEM          If the maximum number of timers has already been reached.
 * @retval      NRF_ERROR_INVALID_PARAM   If GPIOTE has too many users.
 * @retval      NRF_ERROR_INVALID_STATE   If button or GPIOTE has not been initialized.
 */
uint32_t buttons_init(btn_error_handler_t error_handler, event_t * p_startup_evt, event_callback_t callback);


/**@brief       Function for assigning a specific event to a button.
 *
 * @details     This function allows redefinition of standard events assigned to buttons.
 *              To unassign events, provide the event @ref EVENT_NOTHING.
 *
 * @param[in]   button                   Button ID to be redefined.
 * @param[in]   action                   Button action to assign event to.
 * @param[in]   event                    Event to be assigned to button.
 *
 * @retval      NRF_SUCCESS              If the event was successfully assigned to button.
 * @retval      NRF_ERROR_INVALID_PARAM  If the button ID or button action was invalid.
 */
uint32_t event_to_button_action_assign(uint32_t button, button_action_t action, event_t event);


/**@brief This macro will return from the current function if err_code
 *        is not NRF_SUCCESS or NRF_ERROR_INVALID_PARAM.
 */
#define RETURN_ON_ERROR_NOT_INVALID_PARAM(err_code)                             \
do                                                                              \
{                                                                               \
	if (((err_code) != NRF_SUCCESS) && ((err_code) != NRF_ERROR_INVALID_PARAM)) \
	{                                                                           \
		return err_code;                                                        \
	}                                                                           \
}                                                                               \
while (0)




/**@brief This macro will call the registered error handler if err_code
 *        is not NRF_SUCCESS and the error handler is not NULL.
 */
#define CALL_HANDLER_ON_ERROR(err_code)                           \
do                                                                \
{                                                                 \
	if (((err_code) != NRF_SUCCESS) && (m_error_handler != NULL)) \
	{                                                             \
		m_error_handler(err_code);                                \
	}                                                             \
}                                                                 \
while (0)


#ifdef __cplusplus
}
#endif

#endif // BUTTON_ACTION_H__

/** @} */
