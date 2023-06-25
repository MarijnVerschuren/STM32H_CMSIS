#include "main.h"


#if defined(STM32H7xx)
int main(void) {
	// APB1
	volatile dev_id_t i2c1 =				dev_to_id(I2C1);		// APB1_BASE + 0x5400UL
	if (I2C1 != id_to_dev(i2c1))			{ for(;;); }
	// APB2
	volatile dev_id_t tim1 =				dev_to_id(TIM1);		// APB2_BASE + 0x0000UL
	if (TIM1 != id_to_dev(tim1))			{ for(;;); }
	// APB3
	volatile dev_id_t lcd =					dev_to_id(LTDC);		// APB3_BASE + 0x1000UL
	if (LTDC != id_to_dev(lcd))				{ for(;;); }
	// APB4
	volatile dev_id_t lp_uart =				dev_to_id(LPUART1);		// APB4_BASE + 0x0C00UL
	if (LPUART1 != id_to_dev(lp_uart))		{ for(;;); }
	// AHB1
	volatile dev_id_t otg_hs =				dev_to_id(USB_OTG_HS);	// AHB1_BASE -> 0x40040000UL
	volatile dev_id_t otg_fs =				dev_to_id(USB_OTG_FS);	// AHB1_BASE -> 0x40080000UL
	volatile dev_id_t adc1 =				dev_to_id(ADC1);		// AHB1_BASE + 0x2000UL
	if (USB_OTG_HS != id_to_dev(otg_hs))	{ for(;;); }
	if (USB_OTG_FS != id_to_dev(otg_fs))	{ for(;;); }
	if (ADC1 != id_to_dev(adc1))			{ for(;;); }
	// AHB2
	volatile dev_id_t hash =				dev_to_id(HASH);		// AHB2_BASE + 0x1400UL
	if (HASH != id_to_dev(hash))			{ for(;;); }
	// AHB3
	volatile dev_id_t jpeg =				dev_to_id(JPEG);		// AHB3_BASE + 0x3000UL
	if (JPEG != id_to_dev(jpeg))			{ for(;;); }
	// AHB4
	volatile dev_id_t crc =					dev_to_id(CRC);			// AHB4_BASE + 0x4C00UL
	volatile void* crc_p =					id_to_dev(crc);
	if (CRC != crc_p)						{ for(;;); }

	for(;;) {
		__NOP();
	}
}

/*
Error during pretty printers setup: Undefined info command: "pretty-printer". Try "help info".
Some features and performance optimizations will not be available.
*/

#endif