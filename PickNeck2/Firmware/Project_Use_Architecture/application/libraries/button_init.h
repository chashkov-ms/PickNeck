#ifndef BUTTON_INIT_H_
#define BUTTON_INIT_H_

#include "nrf_gpio.h"

#include "pin_config.h"


#ifdef __cplusplus
extern "C" {
#endif

#define BUTTON_ACTION_PUSH      (APP_BUTTON_PUSH)    /**< Represents pushing a button. See @ref button_action_t. */
#define BUTTON_ACTION_RELEASE   (APP_BUTTON_RELEASE) /**< Represents releasing a button. See @ref button_action_t. */
#define BUTTON_ACTION_LONG_PUSH (2)                  /**< Represents pushing and holding a button for @ref LONG_PUSH_TIMEOUT_MS milliseconds. See also @ref button_action_t. */


/**
 * Function for returning the state of a button.
 *
 * @param button_idx Button index (starting from 0), as defined in the board-specific header.
 *
 * @return True if the button is pressed.
 */
bool button_state_get(uint32_t button_idx);


/**
 * Function for converting pin number to button index.
 *
 * @param pin_number Pin number.
 *
 * @return Button index of the given pin or 0xFFFFFFFF if invalid pin provided.
 */
uint32_t pin_to_button_idx(uint32_t pin_number);


/**
 * Function for converting button index to pin number.
 *
 * @param button_idx Button index.
 *
 * @return Pin number.
 */
uint32_t button_idx_to_pin(uint32_t button_idx);


/**@brief       Function for enabling all buttons.
 *
 * @details     After calling this function, all buttons will generate events when pressed, and
 *              all buttons will be able to wake the system up from sleep mode.
 *
 * @retval      NRF_SUCCESS              If the buttons were successfully enabled.
 * @retval      NRF_ERROR_NOT_SUPPORTED  If the board has no buttons or SIMPLE is defined.
 * @return      A propagated error.
 */
uint32_t buttons_enable(void);


/**@brief       Function for disabling all buttons.
 *
 * @details     After calling this function, no buttons will generate events when pressed, and
 *              no buttons will be able to wake the system up from sleep mode.
 *
 * @retval      NRF_SUCCESS              If the buttons were successfully disabled.
 * @retval      NRF_ERROR_NOT_SUPPORTED  If the board has no buttons or SIMPLE is defined.
 * @return      A propagated error.
 */
uint32_t buttons_disable(void);


/**@brief       Function for enabling wakeup from SYSTEM OFF for given button.
 *
 * @details     After calling this function, button can be used to wake up the chip.
 *              This function should only be called immediately before going into sleep.
 *
 * @param[in]   button_idx  Index of the button.
 *
 * @retval      NRF_SUCCESS              If the button was successfully enabled.
 * @retval      NRF_ERROR_NOT_SUPPORTED  If the board has no buttons or SIMPLE is defined.
 */
uint32_t wakeup_button_enable(uint32_t button_idx);


/**@brief       Function for disabling wakeup for the given button.
 *
 * @param[in]   button_idx  index of the button.
 *
 * @retval      NRF_SUCCESS              If the button was successfully disabled.
 * @retval      NRF_ERROR_NOT_SUPPORTED  If the board has no buttons or SIMPLE is defined.
 */
uint32_t wakeup_button_disable(uint32_t button_idx);


#ifdef __cplusplus
}
#endif

#endif // BUTTON_H__

/** @} */