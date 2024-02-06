//
// Created by marijn on 7/10/23.
//

#include "encoder.h"


/*!<
 * init
 * */
void config_encoder_S0S90(TIM_GPIO_t a, TIM_GPIO_t b) {
	dev_pin_t		pin_a =		*((dev_pin_t*)&a),
					pin_b =		*((dev_pin_t*)&b);
	TIM_TypeDef		*tim_a =	id_to_dev(pin_a.id),
					*tim_b =	id_to_dev(pin_b.id),
					*tim = tim_a;
	GPIO_TypeDef	*port_a =	int_to_GPIO(pin_a.port),
					*port_b =	int_to_GPIO(pin_b.port);
	uint8_t			channel_a =	pin_a.id.sub & 0x3UL,
					channel_b =	pin_b.id.sub & 0x3UL;
	if (tim_a != tim_b) { return; }  // error if devices do not match up

	fconfig_GPIO(port_a, pin_a.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, pin_a.alt);
	fconfig_GPIO(port_b, pin_b.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, pin_b.alt);
	config_TIM(tim, 0, 0xffff);
	tim->SMCR = 0b011ul;													// encoder mode 3
	(&tim->CCMR1)[channel_a >> 1] &= ~(0xff << ((channel_a & 0b1u) << 3));	// clear CCMR channel config
	(&tim->CCMR1)[channel_b >> 1] &= ~(0xff << ((channel_b & 0b1u) << 3));	// clear CCMR channel config
	(&tim->CCMR1)[channel_a >> 1] |= (
		(0b1 << ((channel_a & 0b1u) << 3))		|							// enable direct TI
		(10 << (4 + ((channel_a & 0b1u) << 3)))								// set filter for A
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