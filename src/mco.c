//
// Created by marijn on 7/6/23.
//

#include "mco.h"


/*!<
 * init
 * */
void config_MCO(MCO_GPIO_t pin, MCO_SRC_t src, uint8_t prescaler) {
	dev_pin_t dev = *((dev_pin_t*)&pin);
	fconfig_GPIO(
			int_to_GPIO(dev.port_num),
			dev.pin_num,
			GPIO_alt_func,
			GPIO_no_pull,
			GPIO_push_pull,
			GPIO_very_high_speed,
			dev.alt_func
	);
	uint8_t offset = (7 * (dev.dev_id.num & 0b1UL));
	RCC->CFGR &= ~(
			(RCC_CFGR_MCO1 << offset)  |
			(RCC_CFGR_MCO1PRE << offset)
	);
	RCC->CFGR |= (
			(src <<					(RCC_CFGR_MCO1_Pos + offset))  |
			((prescaler & 0xFUL) <<	(RCC_CFGR_MCO1PRE_Pos + offset))
	);
}