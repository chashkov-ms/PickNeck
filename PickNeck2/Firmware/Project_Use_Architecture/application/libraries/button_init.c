#include "button_init.h"
#include "app_button.h"
#include <stdint.h>
#include <stdbool.h>


static const uint8_t m_board_btn_list[BUTTONS_NUMBER] = BUTTONS_LIST;


bool button_state_get(uint32_t button_idx)
{
	ASSERT(button_idx < BUTTONS_NUMBER);
	bool pin_set = nrf_gpio_pin_read(m_board_btn_list[button_idx]) ? true : false;
	return (pin_set == (BUTTONS_ACTIVE_STATE ? true : false));
}


uint32_t pin_to_button_idx(uint32_t pin_number)
{
	uint32_t i;
	uint32_t ret = 0xFFFFFFFF;
	for (i = 0; i < BUTTONS_NUMBER; ++i)
	{
		if (m_board_btn_list[i] == pin_number)
		{
			ret = i;
			break;
		}
	}
	return ret;
}


uint32_t button_idx_to_pin(uint32_t button_idx)
{
	ASSERT(button_idx < BUTTONS_NUMBER);
	return m_board_btn_list[button_idx];
}


uint32_t buttons_enable()
{
#if (BUTTONS_NUMBER > 0)
	return app_button_enable();
#else
	return NRF_ERROR_NOT_SUPPORTED;
#endif
}


uint32_t buttons_disable()
{
#if (BUTTONS_NUMBER > 0)
	return app_button_disable();
#else
	return NRF_ERROR_NOT_SUPPORTED;
#endif
}


static uint32_t wakeup_button_config(uint32_t button_idx, bool enable)
{
	if (button_idx <  BUTTONS_NUMBER)
	{
		nrf_gpio_pin_sense_t sense = enable ?
						(BUTTONS_ACTIVE_STATE ? NRF_GPIO_PIN_SENSE_HIGH : NRF_GPIO_PIN_SENSE_LOW) :
						NRF_GPIO_PIN_NOSENSE;
		nrf_gpio_cfg_sense_set(button_idx_to_pin(button_idx), sense);
		return NRF_SUCCESS;
	}
	return NRF_ERROR_NOT_SUPPORTED;

}


uint32_t wakeup_button_enable(uint32_t button_idx)
{
	return wakeup_button_config(button_idx, true);
}


uint32_t wakeup_button_disable(uint32_t button_idx)
{
	return wakeup_button_config(button_idx, false);
}



