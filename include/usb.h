//
// Created by marijn on 7/19/23.
//

#ifndef STM32H_CMSIS_USB_H
#define STM32H_CMSIS_USB_H
#include "main.h"
#include "sys.h"
#include "nvic.h"
#include "gpio.h"


/*!<
 * constants/definitions
 * */
#define USB_OTG_ENDPOINT_COUNT 9


/*!<
 * enum types
 * */
typedef enum {
	USB_CLK_SRC_DISABLED =	0b00,	//R
	USB_CLK_SRC_PLL1_Q =	0b01,
	USB_CLK_SRC_PLL3_Q =	0b10,
	USB_CLK_SRC_HSI48 =		0b11
} USB_CLK_SRC_t;

typedef enum {
	USB_PIN_DISABLE =		0x00000000,
	// USB1_FS
	USB1_FS_SOF_A4 =		0x040C0039,
	USB1_FS_ID_B12 =		0x0C1C0039,
	USB1_FS_DN_B14 =		0x0E1C0039,
	USB1_FS_DM_B14 =		USB1_FS_DN_B14,  // D(Negative) instead of D(Minus)
	USB1_FS_DP_B15 =		0x0F1C0039,
	// USB1_HS (ULPI)
	USB1_HS_ULPI_CLK_A5 =	0x050AE839,
	USB1_HS_ULPI_D0_A3 =	0x030AE839,
	USB1_HS_ULPI_D1_B0 =	0x001AE839,
	USB1_HS_ULPI_D2_B1 =	0x011AE839,
	USB1_HS_ULPI_D3_B10 =	0x0A1AE839,
	USB1_HS_ULPI_D4_B11 =	0x0B1AE839,
	USB1_HS_ULPI_D5_B12 =	0x0C1AE839,
	USB1_HS_ULPI_D6_B13 =	0x0D1AE839,
	USB1_HS_ULPI_D7_B5 =	0x051AE839,
	USB1_HS_ULPI_STP_C0 =	0x002AE839,
	USB1_HS_ULPI_DIR_C2 =	0x022AE839,
	USB1_HS_ULPI_NXT_C3 =	0x032AE839,
	USB1_HS_ULPI_NXT_H4 =	0x047AE839,
	USB1_HS_ULPI_DIR_I11 =	0x0B8AE839,
	// USB2_FS
	USB2_FS_SOF_A8 =		0x080A003B,
	USB2_FS_ID_A10 =		0x0A0A003B,
	USB2_FS_DN_A11 =		0x0B0A003B,
	USB2_FS_DM_A11 =		USB2_FS_DN_A11,
	USB2_FS_DP_A12 =		0x0C0A003B
} USB_GPIO_t;


/*!<
 * struct types
 * */
typedef struct {
} USB_IEP_status_t;  // (host) in endpoint status type

typedef struct {
	void* buffer;
	uint32_t count;
} USB_OEP_status_t;  // (host) out endpoint status type

typedef struct {
	USB_IEP_status_t	IEP[16];
	USB_OEP_status_t	OEP[16];
	uint32_t			setup[16];	// max controll stage data size: 64B TODO: HAL has 12???
	// classes / interfaces ??
} USB_status_t;


/*!<
 * variables
 * */
extern uint32_t		USB_kernel_frequency;


/*!<
 * init
 * */  // TODO: collapse unneeded fconfig functions!!!!!!!
void config_USB_kernel_clock(USB_CLK_SRC_t src);

void fconfig_USB_FS_device(USB_GPIO_t dp, USB_GPIO_t dn);
void config_USB_FS_device(USB_GPIO_t dp, USB_GPIO_t dn);
// TODO: external phy, host mode

void fconfig_USB_interface(USB_OTG_GlobalTypeDef* usb);
void config_USB_interface(USB_OTG_GlobalTypeDef* usb);

void config_USB_RX_FIFO(USB_OTG_GlobalTypeDef* usb, uint32_t size);
void config_USB_TX_FIFO(USB_OTG_GlobalTypeDef* usb, uint8_t ep, uint32_t size);

void start_USB(USB_OTG_GlobalTypeDef* usb);
void stop_USB(USB_OTG_GlobalTypeDef* usb);

// TODO: deinit func

#endif // STM32H_CMSIS_USB_H
