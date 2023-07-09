//
// Created by marijn on 7/10/23.
//

#include "encoder.h"


/*!<
 * init
 * */
void config_encoder_S0S90(TIM_GPIO_t encoder_pin_a, TIM_GPIO_t encoder_pin_b) {
	dev_pin_t dev_a = *((dev_pin_t*)&encoder_pin_a);
	dev_pin_t dev_b = *((dev_pin_t*)&encoder_pin_b);
	TIM_TypeDef*	tim_a =		id_to_dev(dev_a.dev_id);
	TIM_TypeDef*	tim_b =		id_to_dev(dev_b.dev_id);
	GPIO_TypeDef*	port_a =	int_to_GPIO(dev_a.port_num);
	GPIO_TypeDef*	port_b =	int_to_GPIO(dev_b.port_num);
	uint8_t channel_a = dev_a.dev_id.sub & 0x3UL, pin_a = dev_a.pin_num, af_a = dev_a.alt_func;
	uint8_t channel_b = dev_b.dev_id.sub & 0x3UL, pin_b = dev_b.pin_num, af_b = dev_b.alt_func;
	if (tim_a != tim_b) { return; }; TIM_TypeDef* tim = tim_a;
	fconfig_GPIO(port_a, pin_a, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, af_a);
	fconfig_GPIO(port_b, pin_b, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, af_b);
	config_TIM(tim, 0, 0xffff);
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


/*!<
 * actions
 * */
void(*start_encoder_S0S90)(TIM_TypeDef* tim) =	start_TIM;
void(*stop_encoder_S0S90)(TIM_TypeDef* tim) =	stop_TIM;