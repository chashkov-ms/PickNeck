#include "led.h"
#include <stdint.h>
#include <stdbool.h>


static const uint32_t led_list[LEDS_NUMBER] = LEDS_LIST;

bool led_state_get(uint32_t led_idx)
{
	ASSERT(led_idx < LEDS_NUMBER);
	bool pin_set = nrf_gpio_pin_out_read(led_list[led_idx]) ? true : false;
	return (pin_set == (LEDS_ACTIVE_STATE ? true : false));
}

void led_on(uint32_t led_idx)
{
	ASSERT(led_idx < LEDS_NUMBER);
	nrf_gpio_pin_write(led_list[led_idx], LEDS_ACTIVE_STATE ? 1 : 0);
}

void led_off(uint32_t led_idx)
{
	ASSERT(led_idx < LEDS_NUMBER);
	nrf_gpio_pin_write(led_list[led_idx], LEDS_ACTIVE_STATE ? 0 : 1);
}

void leds_off(void)
{
	uint32_t i;
	for (i = 0; i < LEDS_NUMBER; ++i)
	{
		led_off(i);
	}
}

void leds_on(void)
{
	uint32_t i;
	for (i = 0; i < LEDS_NUMBER; ++i)
	{
		led_on(i);
	}
}

void led_invert(uint32_t led_idx)
{
	ASSERT(led_idx < LEDS_NUMBER);
	nrf_gpio_pin_toggle(led_list[led_idx]);
}

uint32_t led_idx_to_pin(uint32_t led_idx)
{
	ASSERT(led_idx < LEDS_NUMBER);
	return led_list[led_idx];
}

uint32_t pin_to_led_idx(uint32_t pin_number)
{
	uint32_t ret = 0xFFFFFFFF;
	uint32_t i;
	for (i = 0; i < LEDS_NUMBER; ++i)
	{
		if (led_list[i] == pin_number)
		{
			ret = i;
			break;
		}
	}
	return ret;
}

void leds_init(void)
{
	uint32_t i;
	for (i = 0; i < LEDS_NUMBER; ++i)
	{
		nrf_gpio_cfg_output(led_list[i]);
	}
	leds_off();
        asm("nop");
}
