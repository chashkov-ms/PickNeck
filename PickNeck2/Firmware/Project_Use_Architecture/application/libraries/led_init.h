#ifndef LED_INIT_H_
#define LED_INIT_H_

#include "nrf_gpio.h"

#include "pin_config.h"


#ifdef __cplusplus
extern "C" {
#endif

#define ADVERTISING_LED_ON_INTERVAL            200
#define ADVERTISING_LED_OFF_INTERVAL           1800

#define ADVERTISING_DIRECTED_LED_ON_INTERVAL   200
#define ADVERTISING_DIRECTED_LED_OFF_INTERVAL  200

#define ADVERTISING_WHITELIST_LED_ON_INTERVAL  200
#define ADVERTISING_WHITELIST_LED_OFF_INTERVAL 800

#define ADVERTISING_SLOW_LED_ON_INTERVAL       400
#define ADVERTISING_SLOW_LED_OFF_INTERVAL      4000

#define BONDING_INTERVAL                       100

#define SENT_OK_INTERVAL                       100
#define SEND_ERROR_INTERVAL                    500

#define RCV_OK_INTERVAL                        100
#define RCV_ERROR_INTERVAL                     500

#define ALERT_INTERVAL                         200

#define LED_INDICATE_SENT_OK               INDEX_LED_1
#define LED_INDICATE_SEND_ERROR            INDEX_LED_1
#define LED_INDICATE_RCV_OK                INDEX_LED_1
#define LED_INDICATE_RCV_ERROR             INDEX_LED_1
#define LED_INDICATE_CONNECTED             INDEX_LED_0
#define LED_INDICATE_BONDING               INDEX_LED_0
#define LED_INDICATE_ADVERTISING_DIRECTED  INDEX_LED_0
#define LED_INDICATE_ADVERTISING_SLOW      INDEX_LED_0
#define LED_INDICATE_ADVERTISING_WHITELIST INDEX_LED_0
#define LED_INDICATE_INDICATE_ADVERTISING  INDEX_LED_0

#define LED_ALERT                         INDEX_LED_2

/**
 * Function for returning the state of an LED.
 *
 * @param led_idx LED index (starting from 0), as defined in the board-specific header.
 *
 * @return True if the LED is turned on.
 */
bool led_state_get(uint32_t led_idx);

/**
 * Function for turning on an LED.
 *
 * @param led_idx LED index (starting from 0), as defined in the board-specific header.
 */
void led_on(uint32_t led_idx);

/**
 * Function for turning off an LED.
 *
 * @param led_idx LED index (starting from 0), as defined in the board-specific header.
 */
void led_off(uint32_t led_idx);

/**
 * Function for inverting the state of an LED.
 *
 * @param led_idx LED index (starting from 0), as defined in the board-specific header.
 */
void led_invert(uint32_t led_idx);
/**
 * Function for turning off all LEDs.
 */
void leds_off(void);

/**
 * Function for turning on all LEDs.
 */
void leds_on(void);

/**
 * Function for converting pin number to LED index.
 *
 * @param pin_number Pin number.
 *
 * @return LED index of the given pin or 0xFFFFFFFF if invalid pin provided.
 */
uint32_t pin_to_led_idx(uint32_t pin_number);

/**
 * Function for converting LED index to pin number.
 *
 * @param led_idx LED index.
 *
 * @return Pin number.
 */
uint32_t led_idx_to_pin(uint32_t led_idx);

/**
 * Function for initialization all LEDs.
 */
void leds_init(void);


#ifdef __cplusplus
}
#endif

#endif // LED_H__

/** @} */