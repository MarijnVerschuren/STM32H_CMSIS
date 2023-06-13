//
// Created by marijn on 3/27/23.
//
#include "pwm.h"


/*!< static */
static inline void TIM_GPIO_to_args(uint32_t tim_pin, TIM_TypeDef** tim, uint8_t* channel, uint8_t* alternate_function, GPIO_TypeDef** port, uint8_t* pin) {
	uint8_t dev_id =		(tim_pin >> 16);
	(*tim) =				id_to_dev(*((dev_id_t*)&dev_id));
	(*channel) =			(tim_pin >> 14) & 0x3u;
	(*alternate_function) =	(tim_pin >> 8) & 0xfu;
	(*port) =				int_to_GPIO(tim_pin >> 4);
	(*pin) =				tim_pin & 0xfu;
}


/*!< init / enable / disable */
void config_PWM(TIM_GPIO_t pwm_pin, uint32_t prescaler, uint32_t period) {
	TIM_TypeDef* tim; GPIO_TypeDef* port;	// registers
	uint8_t channel, pin, af;				// params
	TIM_GPIO_to_args(pwm_pin, &tim, &channel, &af, &port, &pin);
	fconfig_GPIO(port, pin, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, af);
	config_TIM(tim, prescaler, period);
	(&tim->CCMR1)[channel >> 1] &=	~(0x3u << ((channel & 0b1u) << 3));	// clear register
	tim->CCER &=					~(0b1u << (1 + (channel << 2)));	// default polarity
	(&tim->CCMR1)[channel >> 1] |=	(
		(0b110 << (4 + ((channel & 0b1u) << 3))) |						// set PWM mode
		(0b1u << (3 + ((channel & 0b1u) << 3)))							// enable preload
	);
	tim->CR1 |= TIM_CR1_ARPE;											// enable auto-reload preload
	tim->CCER |= (0b1u << (channel << 2));								// enable capture compare
	(&tim->CCR1)[channel] = 0;											// set duty cycle to 0
	tim->CR1 |= TIM_CR1_CEN;											// start timer
}