//
// Created by marijn on 7/19/23.
//

#ifndef STM32H_CMSIS_USB_H
#define STM32H_CMSIS_USB_H
#include "main.h"
#include "sys.h"
#include "gpio.h"


/*!<
 * types
 * */
typedef enum {
	USB_CLK_SRC_DISABLED =	0b00,	//R
	USB_CLK_SRC_PLL1_Q =	0b01,
	USB_CLK_SRC_PLL3_Q =	0b10,
	USB_CLK_SRC_HSI48 =		0b11
} USB_CLK_SRC_t;

typedef enum {
	USB_PIN_DISABLE =		0x00000000,
	// USB1
	USB1_HS_SOF_A4 =		0x040C0039,
	USB1_HS_ID_B12 =		0x0C1C0039,
	USB1_HS_DM_B14 =		0x0E1C0039,
	USB1_HS_DP_B15 =		0x0F1C0039,

	USB1_HS_ =		0x00000000,

	// USB1_ULPI
	USB1_HS_ULPI_CLK_A5 =	0x050AE839,
	USB1_HS_ULPI_D0_A3 =	0x030AE839,
	USB1_HS_ULPI_D1_B0 =	0x001AE839,
	USB1_HS_ULPI_D2_B1 =	0x011AE839,
	USB1_HS_ULPI_D3_B10 =	0x0A1AE839,
	USB1_HS_ULPI_D4_B11 =	0x0B1AE839,
	USB1_HS_ULPI_D5_B12 =	0x0C1AE839,
	USB1_HS_ULPI_D6_B13 =	0x0D1AE839,
	USB1_HS_ULPI_D7_B5 =	0x051AE839,

	USB1_HS_ULPI_ =	0x00000000,

	// USB2
	USB2_FS_SOF_A8 =		0x080A003B,
	USB2_FS_ID_A10 =		0x0A0A003B,
	USB2_FS_DM_A11 =		0x0B0A003B,
	USB2_FS_DP_A12 =		0x0C0A003B,

	USB2_FS_ =		0x00000000,

} USB_GPIO_t;
// TODO: alias DN


/*!<
 * variables
 * */
extern uint32_t USB_kernel_frequency;


/*!<
 * init
 * */
void config_USB_kernel_clock(USB_CLK_SRC_t src);
void fconfig_USB(USB_GPIO_t dp, USB_GPIO_t dn);
void config_USB(USB_GPIO_t dp, USB_GPIO_t dn);
// TODO: phy?


#endif //STM32H_CMSIS_USB_H
