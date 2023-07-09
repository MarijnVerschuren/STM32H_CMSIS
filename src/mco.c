//
// Created by marijn on 7/6/23.
//

#include "mco.h"


/*!<
 * init
 * */
void config_MCO(MCO_GPIO_t pin) {
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
}