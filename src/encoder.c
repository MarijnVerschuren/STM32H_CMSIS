//
// Created by marijn on 4/19/23.
//
#include "encoder.h"


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
void config_encoder_S0S90(TIM_GPIO_t encoder_pin_a, TIM_GPIO_t encoder_pin_b) {
	TIM_TypeDef *tim_a = NULL, *tim_b = NULL, *tim = NULL;
	GPIO_TypeDef *port_a = NULL, *port_b = NULL;
	uint8_t channel_a = 0, pin_a = 0, af_a = 0;
	uint8_t channel_b = 0, pin_b = 0, af_b = 0;
	TIM_GPIO_to_args(encoder_pin_a, &tim_a, &channel_a, &af_a, &port_a, &pin_a);
	TIM_GPIO_to_args(encoder_pin_b, &tim_b, &channel_b, &af_b, &port_b, &pin_b);
	if (tim_a != tim_b) { return; }; tim = tim_a;
	fconfig_GPIO(port_a, pin_a, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, af_a);
	fconfig_GPIO(port_b, pin_b, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, af_b);
	config_TIM(tim, 0, 65535);
	tim->SMCR = 0b011ul;													// encoder mode 3
	(&tim->CCMR1)[channel_a >> 1] &= ~(0xff << ((channel_a & 0b1u) << 3));	// clear CCMR channel config
	(&tim->CCMR1)[channel_b >> 1] &= ~(0xff << ((channel_b & 0b1u) << 3));	// clear CCMR channel config
	(&tim->CCMR1)[channel_a >> 1] |= (
			(0b1 << ((channel_a & 0b1u) << 3))		|						// enable direct TI
			(10 << (4 + ((channel_a & 0b1u) << 3)))							// set filter for A
	);
	(&tim->CCMR1)[channel_b >> 1] |= 0b1 << ((channel_b & 0b1u) << 3);		// enable direct TI
	tim->CCER &= ~(0b1 << (1 + (channel_a << 2)));							// set polarity high
	tim->CCER &= ~(0b1 << (1 + (channel_b << 2)));							// set polarity high
	tim->CCER |= 0b1 << (channel_a << 2);									// enable capture compare
	tim->CCER |= 0b1 << (channel_b << 2);									// enable capture compare
}


/*!< actions */
void start_encoder_S0S90(TIM_TypeDef* tim)	{ start_TIM(tim); }
void stop_encoder_S0S90(TIM_TypeDef* tim)	{ stop_TIM(tim); }