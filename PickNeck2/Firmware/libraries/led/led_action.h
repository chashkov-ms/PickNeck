#ifndef LED_ACTION_H__
#define LED_ACTION_H__

#include "nrf_gpio.h"

#include "pin_config.h"


#ifdef __cplusplus
extern "C" {
#endif

/**@brief indication states.
 *
 * @details See @ref examples_states for a list of how these states are indicated for the PCA10028/PCA10040 board and the PCA10031 dongle.
 */
typedef enum
{
	INDICATE_FIRST = 0,
	INDICATE_IDLE  = INDICATE_FIRST, /**< See \ref INDICATE_IDLE.*/
	INDICATE_SCANNING,                   /**< See \ref INDICATE_SCANNING.*/
	INDICATE_ADVERTISING,                /**< See \ref INDICATE_ADVERTISING.*/
	INDICATE_ADVERTISING_WHITELIST,      /**< See \ref INDICATE_ADVERTISING_WHITELIST.*/
	INDICATE_ADVERTISING_SLOW,           /**< See \ref INDICATE_ADVERTISING_SLOW.*/
	INDICATE_ADVERTISING_DIRECTED,       /**< See \ref INDICATE_ADVERTISING_DIRECTED.*/
	INDICATE_BONDING,                    /**< See \ref INDICATE_BONDING.*/
	INDICATE_CONNECTED,                  /**< See \ref INDICATE_CONNECTED.*/
	INDICATE_SENT_OK,                    /**< See \ref INDICATE_SENT_OK.*/
	INDICATE_SEND_ERROR,                 /**< See \ref INDICATE_SEND_ERROR.*/
	INDICATE_RCV_OK,                     /**< See \ref INDICATE_RCV_OK.*/
	INDICATE_RCV_ERROR,                  /**< See \ref INDICATE_RCV_ERROR.*/
	INDICATE_FATAL_ERROR,                /**< See \ref INDICATE_FATAL_ERROR.*/
	INDICATE_ALERT_0,                    /**< See \ref INDICATE_ALERT_0.*/
	INDICATE_ALERT_1,                    /**< See \ref INDICATE_ALERT_1.*/
	INDICATE_ALERT_2,                    /**< See \ref INDICATE_ALERT_2.*/
	INDICATE_ALERT_3,                    /**< See \ref INDICATE_ALERT_3.*/
	INDICATE_ALERT_OFF,                  /**< See \ref INDICATE_ALERT_OFF.*/
	INDICATE_USER_STATE_OFF,             /**< See \ref INDICATE_USER_STATE_OFF.*/
	INDICATE_USER_STATE_0,               /**< See \ref INDICATE_USER_STATE_0.*/
	INDICATE_USER_STATE_1,               /**< See \ref INDICATE_USER_STATE_1.*/
	INDICATE_USER_STATE_2,               /**< See \ref INDICATE_USER_STATE_2.*/
	INDICATE_USER_STATE_3,               /**< See \ref INDICATE_USER_STATE_3.*/
	INDICATE_USER_STATE_ON,              /**< See \ref INDICATE_USER_STATE_ON.*/
	INDICATE_LAST = INDICATE_USER_STATE_ON
} indication_t;

/**@brief       Function for configuring indicators to required state.
 *
 * @details     This function indicates the required state by means of LEDs (if enabled).
 *
 * @note        Alerts are indicated independently.
 *
 * @param[in]   indicate   State to be indicated.
 *
 * @retval      NRF_SUCCESS               If the state was successfully indicated.
 * @retval      NRF_ERROR_NO_MEM          If the internal timer operations queue was full.
 * @retval      NRF_ERROR_INVALID_STATE   If the application timer module has not been initialized,
 *                                        or internal timer has not been created.
 */
uint32_t indication_set(indication_t indicate);


/**
 * Function for initialization all LEDs.
 */
uint32_t leds_action_init(void);


#ifdef __cplusplus
}
#endif

#endif // LED_ACTION_H__

/** @} */