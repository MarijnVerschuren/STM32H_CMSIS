//
// Created by marijn on 7/19/23.
//

#include "usb.h"


/*!<
 * variables
 * */
uint32_t USB_kernel_frequency = 0;


/*!<
 * init
 * */
void config_USB_kernel_clock(USB_CLK_SRC_t src) {
	RCC->D2CCIP2R &= ~RCC_D2CCIP2R_USBSEL;
	RCC->D2CCIP2R |= src << RCC_D2CCIP2R_USBSEL_Pos;
	switch (src) {
		case USB_CLK_SRC_DISABLED:	USB_kernel_frequency = 0; return;
		case USB_CLK_SRC_PLL1_Q:	USB_kernel_frequency = PLL1_Q_clock_frequency; return;
		case USB_CLK_SRC_PLL3_Q:	USB_kernel_frequency = PLL3_Q_clock_frequency; return;
		case USB_CLK_SRC_HSI48:		USB_kernel_frequency = HSI48_clock_frequency; return;
	}
}

void fconfig_USB(USB_GPIO_t dp, USB_GPIO_t dn) {
	//#define USB1_OTG_HS_PERIPH_BASE  -->  (dev_id){25, DEV_CLOCK_ID_AHB1}  --[((num - 23) << 16) + AHB1_BASE]-->  (0x40040000UL)
	//#define USB2_OTG_FS_PERIPH_BASE  -->  (dev_id){27, DEV_CLOCK_ID_AHB1}  --[((num - 23) << 16) + AHB1_BASE]-->  (0x40080000UL)

	return;
}

void config_USB(USB_GPIO_t dp, USB_GPIO_t dn) {
	fconfig_USB(dp, dn);
}