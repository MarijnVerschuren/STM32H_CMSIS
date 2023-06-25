#include "main.h"


#if defined(STM32H7xx)
int main(void) {
	// APB1
	volatile void* i2c1_p =		I2C1;
	volatile dev_id_t i2c1 =	dev_to_id(I2C1);		// APB1_BASE + 0x5400UL
	volatile void* i2c1_r = 	id_to_dev(i2c1);
	// APB2
	volatile void* tim1_p =		TIM1;
	volatile dev_id_t tim1 =	dev_to_id(TIM1);		// APB2_BASE + 0x0000UL
	volatile void* tim1_r = 	id_to_dev(tim1);
	// APB3
	volatile void* lcd_p =		LTDC;
	volatile dev_id_t lcd =		dev_to_id(LTDC);		// APB3_BASE + 0x1000UL
	volatile void* lcd_r =	 	id_to_dev(lcd);
	// APB4
	volatile void* lp_uart_p =	LPUART1;
	volatile dev_id_t lp_uart =	dev_to_id(LPUART1);		// APB4_BASE + 0x0C00UL
	volatile void* lp_uart_r = 	id_to_dev(lp_uart);
	// AHB1
	volatile void* otg_hs_p =	USB_OTG_HS;
	volatile void* otg_fs_p =	USB_OTG_FS;
	volatile void* adc1_p =		ADC1;
	volatile dev_id_t otg_hs =	dev_to_id(USB_OTG_HS);	// AHB1_BASE -> 0x40040000UL
	volatile dev_id_t otg_fs =	dev_to_id(USB_OTG_FS);	// AHB1_BASE -> 0x40080000UL
	volatile dev_id_t adc1 =	dev_to_id(ADC1);		// AHB1_BASE + 0x2000UL
	volatile void* otg_hs_r = 	id_to_dev(otg_hs);
	volatile void* otg_fs_r = 	id_to_dev(otg_fs);
	volatile void* adc1_r = 	id_to_dev(adc1);
	// AHB2
	volatile void* hash_p =		HASH;
	volatile dev_id_t hash =	dev_to_id(HASH);		// AHB2_BASE + 0x1400UL
	volatile void* hash_r = 	id_to_dev(hash);
	// AHB3
	volatile void* jpeg_p =		JPEG;
	volatile dev_id_t jpeg =	dev_to_id(JPEG);		// AHB3_BASE + 0x3000UL
	volatile void* jpeg_r = 	id_to_dev(jpeg);
	// AHB4
	volatile void* crc_p =		CRC;
	volatile dev_id_t crc =		dev_to_id(CRC);			// AHB4_BASE + 0x4C00UL
	volatile void* crc_r = 		id_to_dev(crc);

	for(;;);
}

/*
Error during pretty printers setup: Undefined info command: "pretty-printer". Try "help info".
Some features and performance optimizations will not be available.
*/

#endif