#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

// LEDs definitions
#define LEDS_NUMBER    4

#define LED_1          6  // 17   
#define LED_2          7  // 18
#define LED_3          27 // 19
#define LED_4          28 // 20

#define LEDS_ACTIVE_STATE 1

#define LEDS_LIST { LED_1, LED_2, LED_3, LED_4 }

#define INDEX_LED_0 0
#define INDEX_LED_1 1
#define INDEX_LED_2 2
#define INDEX_LED_3 3


// Buttons definitions
#define BUTTONS_NUMBER 4

#define BUTTON_1       31 //13
#define BUTTON_2       14
#define BUTTON_3       15
#define BUTTON_4       16
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4 }

#define INDEX_BUTTON_0 0
#define INDEX_BUTTON_1 1
#define INDEX_BUTTON_2 2
#define INDEX_BUTTON_3 3


// UART definitions
#define RX_PIN_NUMBER  25 // 8
#define TX_PIN_NUMBER  26 // 6
// #define CTS_PIN_NUMBER 7
// #define RTS_PIN_NUMBER 5
#define HWFC           true


#define SDC_SCK_PIN     13 // 25  ///< SDC serial clock (SCK) pin.   // SCK  // 25
#define SDC_MOSI_PIN    14 // 23  ///< SDC serial data in (DI) pin.  // MOSI // 23
#define SDC_MISO_PIN    11 // 24  ///< SDC serial data out (DO) pin. // MISO // 24
#define SDC_CS_PIN      12 // 22  ///< SDC chip select (CS) pin.     // CS   // 22


#define IMU_SCK_PIN     18 // 22  ///< SDC serial clock (SCK) pin.   // SCK  // 19
#define IMU_MOSI_PIN    17 // 21  ///< SDC serial data in (DI) pin.  // MOSI // 18
#define IMU_MISO_PIN    16 // 20  ///< SDC serial data out (DO) pin. // MISO // 17
#define IMU_CS_PIN      19 // 23  ///< SDC chip select (CS) pin.     // CS   // 20


// SPI definitions
// #define SPIM0_SCK_PIN   29  // SPI clock GPIO pin number.
// #define SPIM0_MOSI_PIN  25  // SPI Master Out Slave In GPIO pin number.
// #define SPIM0_MISO_PIN  28  // SPI Master In Slave Out GPIO pin number.
// #define SPIM0_SS_PIN    12  // SPI Slave Select GPIO pin number.

// #define SPIM1_SCK_PIN   2   // SPI clock GPIO pin number.
// #define SPIM1_MOSI_PIN  3   // SPI Master Out Slave In GPIO pin number.
// #define SPIM1_MISO_PIN  4   // SPI Master In Slave Out GPIO pin number.
// #define SPIM1_SS_PIN    5   // SPI Slave Select GPIO pin number.


#ifdef __cplusplus
}
#endif

#endif // PIN_CONFIG_H
