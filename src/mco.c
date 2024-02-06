//
// Created by marijn on 7/6/23.
//

#include "mco.h"


/*!<
 * variables
 * */
uint32_t MCO1_kernel_frequency = 0;
uint32_t MCO2_kernel_frequency = 0;


/*!<
 * init
 * */
void config_MCO(MCO_GPIO_t pin, MCO_SRC_t src, uint8_t prescaler) {
	dev_pin_t dev = *((dev_pin_t*)&pin);
	fconfig_GPIO(
		int_to_GPIO(dev.port),
		dev.num,
		GPIO_alt_func,
		GPIO_no_pull,
		GPIO_push_pull,
		GPIO_very_high_speed,
		dev.alt
	);
	uint8_t offset = (7 * (dev.id.num & 0b1UL));
	RCC->CFGR &= ~(
		(RCC_CFGR_MCO1 << offset)  |
		(RCC_CFGR_MCO1PRE << offset)
	);
	RCC->CFGR |= (
		(src <<					(RCC_CFGR_MCO1_Pos + offset))  |
		((prescaler & 0xFUL) <<	(RCC_CFGR_MCO1PRE_Pos + offset))
	);
	if (dev.id.num) {
		switch (src) {
			case MCO2_SRC_SYS:		MCO2_kernel_frequency = SYS_clock_frequency;	return;
			case MCO2_SRC_PLL2_P:	MCO2_kernel_frequency = PLL2_P_clock_frequency;	return;
			case MCO2_SRC_HSE:		MCO2_kernel_frequency = HSE_clock_frequency;	return;
			case MCO2_SRC_PLL1_P:	MCO2_kernel_frequency = PLL1_P_clock_frequency;	return;
			case MCO2_SRC_CSI:		MCO2_kernel_frequency = CSI_clock_frequency;	return;
			case MCO2_SRC_LSI:		MCO2_kernel_frequency = LSI_clock_frequency;	return;
			default: return;
		}
	}
	switch (src) {
		case MCO1_SRC_HSI:			MCO1_kernel_frequency = HSI_clock_frequency;	return;
		case MCO1_SRC_LSE:			MCO1_kernel_frequency = LSE_clock_frequency;	return;
		case MCO1_SRC_HSE:			MCO1_kernel_frequency = HSE_clock_frequency;	return;
		case MCO1_SRC_PLL1_Q:		MCO1_kernel_frequency = PLL1_Q_clock_frequency;	return;
		case MCO1_SRC_HSI48:		MCO1_kernel_frequency = HSI48_clock_frequency;	return;
		default: return;
	}
}