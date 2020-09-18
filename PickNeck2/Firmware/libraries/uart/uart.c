#include "uart.h"
#include "nrf_uart.h"
#include "app_uart.h"
#include "pin_config.h"


#define UART_TX_BUF_SIZE   256   /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE   256   /**< UART RX buffer size. */


/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to
 *          a string. The string will be be sent over BLE when the last character received was a
 *          'new line' '\n' (hex 0x0A) or if the string has reached the maximum data length.
 */
/**@snippet [Handling the data received over UART] */
static void uart_event_handle(app_uart_evt_t * p_event)
{
	static uint8_t data_byte;
	ret_code_t       err_code;

	switch (p_event->evt_type)
	{
		case APP_UART_DATA_READY:
			{
				UNUSED_VARIABLE(app_uart_get(&data_byte));

				err_code = app_uart_put(data_byte);

				if ((err_code != NRF_ERROR_INVALID_STATE) &&
					(err_code != NRF_ERROR_RESOURCES) &&
					(err_code != NRF_ERROR_NOT_FOUND))
				{
					APP_ERROR_CHECK(err_code);
				}
			} break;

		case APP_UART_COMMUNICATION_ERROR:
			{
				APP_ERROR_HANDLER(p_event->data.error_communication);
			} break;

		case APP_UART_FIFO_ERROR:
			{
				APP_ERROR_HANDLER(p_event->data.error_code);
			} break;

		default:
			{
			} break;
	}
}
/**@snippet [Handling the data received over UART] */


/**@brief  Function for initializing the UART module.
 */
/**@snippet [UART Initialization] */
void uart_init(void)
{
	ret_code_t                   err_code;
	app_uart_comm_params_t const comm_params =
	{
		.rx_pin_no    = RX_PIN_NUMBER,
		.tx_pin_no    = TX_PIN_NUMBER,
		.flow_control = APP_UART_FLOW_CONTROL_DISABLED,
		.use_parity   = false,
		.baud_rate    = NRF_UART_BAUDRATE_115200
	};

	APP_UART_FIFO_INIT(&comm_params,
					   UART_RX_BUF_SIZE,
					   UART_TX_BUF_SIZE,
					   uart_event_handle,
					   APP_IRQ_PRIORITY_LOWEST,
					   err_code);
	APP_ERROR_CHECK(err_code);
}
/**@snippet [UART Initialization] */
