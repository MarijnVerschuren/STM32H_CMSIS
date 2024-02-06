//
// Created by marijn on 7/8/23.
//

#include "pwm.h"


/*!<
 * init
 * */
void config_PWM(TIM_GPIO_t pin, uint32_t prescaler, uint32_t period) {
	dev_pin_t		dev = *((dev_pin_t*)&pin);
	TIM_TypeDef*	tim = id_to_dev(dev.id);
	uint8_t			channel = dev.id.sub & 0x3UL;

	fconfig_GPIO(
		int_to_GPIO(dev.port), dev.num, GPIO_alt_func,
		GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, dev.alt
	);
	fconfig_TIM(tim, prescaler, period, 1, 0, 0);  // enable auto-reload preload
	(&tim->CCMR1)[channel >> 1] &=	~(0x3UL << ((channel & 0b1u) << 3));			// clear register
	tim->CCER &=					~(0b1UL << (1 + (channel << 2)));				// default polarity
	(&tim->CCMR1)[channel >> 1] |=	(
		(0b110UL << (4 + ((channel & 0b1UL) << 3))) |								// set PWM mode
		(0b1UL << (3 + ((channel & 0b1UL) << 3)))									// enable preload
	);
	tim->CCER |= (0b1u << (channel << 2));											// enable capture compare
	(&tim->CCR1)[channel] = 0;														// set duty cycle to 0
	tim->BDTR |= TIM_BDTR_MOE;														// main output enable
	start_TIM(tim);
}
