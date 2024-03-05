//
// Created by marijn on 7/19/23.
//

#ifndef STM32H_CMSIS_USB_H
#define STM32H_CMSIS_USB_H
#include "main.h"
#include "sys.h"
#include "nvic.h"
#include "gpio.h"
#include "usb/usb_handle.h"


#ifdef USB_LIB
/*!<
 * definitions
 * */
#define USB_OTG_ENDPOINT_COUNT		9U
#define USB_OTG_MAX_INTERFACE_COUNT	15U
#define USB_OTG_MAX_CONFIG_COUNT	1U


/*!<
 * types
 * */
typedef enum {
	USB_CLK_SRC_DISABLED =	 	 0b00,	//R
	USB_CLK_SRC_PLL1_Q =	 	 0b01,
	USB_CLK_SRC_PLL3_Q =	 	 0b10,
	USB_CLK_SRC_HSI48 =		 	 0b11
}	USB_CLK_SRC_t;
typedef enum {
	USB_PIN_DISABLE =		 	 0x00000000,
	// USB1_FS
	USB1_FS_SOF_A4 =		 	 0x040C0039,
	USB1_FS_ID_B12 =		 	 0x0C1C0039,
	USB1_FS_DN_B14 =		 	 0x0E1C0039,
	USB1_FS_DM_B14 =		 	 USB1_FS_DN_B14,  // D(Negative) instead of D(Minus)
	USB1_FS_DP_B15 =		 	 0x0F1C0039,
	// USB1_HS (ULPI)
	USB1_HS_ULPI_CLK_A5 =	 	 0x050AE839,
	USB1_HS_ULPI_D0_A3 =	 	 0x030AE839,
	USB1_HS_ULPI_D1_B0 =	 	 0x001AE839,
	USB1_HS_ULPI_D2_B1 =	 	 0x011AE839,
	USB1_HS_ULPI_D3_B10 =	 	 0x0A1AE839,
	USB1_HS_ULPI_D4_B11 =	 	 0x0B1AE839,
	USB1_HS_ULPI_D5_B12 =	 	 0x0C1AE839,
	USB1_HS_ULPI_D6_B13 =	 	 0x0D1AE839,
	USB1_HS_ULPI_D7_B5 =	 	 0x051AE839,
	USB1_HS_ULPI_STP_C0 =	 	 0x002AE839,
	USB1_HS_ULPI_DIR_C2 =	 	 0x022AE839,
	USB1_HS_ULPI_NXT_C3 =	 	 0x032AE839,
	USB1_HS_ULPI_NXT_H4 =	 	 0x047AE839,
	USB1_HS_ULPI_DIR_I11 =	 	 0x0B8AE839,
	// USB2_FS
	USB2_FS_SOF_A8 =		 	 0x080A003B,
	USB2_FS_ID_A10 =		 	 0x0A0A003B,
	USB2_FS_DN_A11 =		 	 0x0B0A003B,
	USB2_FS_DM_A11 =		 	 USB2_FS_DN_A11,
	USB2_FS_DP_A12 =		 	 0x0C0A003B
}	USB_GPIO_t;
typedef enum {
	USB_CLASS_COMPOSITE =		 0x0U,
	USB_CLASS_HID_KEYBOARD =	 0x1U,
	USB_CLASS_HID_MOUSE =		 0x2U,
	USB_CLASS_HID_CUSTOM =		 0x4U,
	USB_CLASS_MSC =				 0x8U
} USB_class_type_t;

typedef enum {
	USB_device_descriptor_type =			 	0x1U,
	USB_config_descriptor_type =			 	0x2U,
	USB_string_descriptor_type =			 	0x3U,
	USB_interface_descriptor_type =			 	0x4U,
	USB_endpoint_descriptor_type =			 	0x5U,
	USB_qualifier_descriptor_type =			 	0x6U,
	USB_other_speed_descriptor_type =		 	0x7U,
	//USB_interface_power_descriptor_type =		0x8U,
	//USB_OTG_descriptor_type =					0x9U,
	//USB_debug_descriptor_type =				0xAU,
	USB_IAD_descriptor_type =				 	0xBU,
	USB_BOS_descriptor_type =				 	0xFU
} USB_descriptor_type_t;
typedef enum {
	USB_device_descriptor_size =		 	 	0x12U,
	USB_config_descriptor_size =		 	 	0x09U,
	USB_interface_descriptor_size =		 	 	0x09U,
	USB_endpoint_descriptor_size =		 	 	0x07U,
	USB_qualifier_descriptor_size =		 	 	0x0AU,
	USB_other_speed_descriptor_size =	 	 	0x09U,
	//USB_interface_power_descriptor_size =	 	0x02U,
	//USB_OTG_descriptor_size =				 	0x03U
} USB_descriptor_size_t;
typedef enum {
	USB_language_ID_string_descriptor_type =	0x0U,
	USB_manufacturer_string_descriptor_type =	0x1U,
	USB_product_string_descriptor_type =		0x2U,
	USB_serial_string_descriptor_type =			0x3U,
	USB_config_string_descriptor_type =			0x4U,
	USB_interface_string_descriptor_type =		0x5U,
} USB_string_descriptor_type_t;
typedef enum {
	USB_language_ID_descriptor_size =			0x04U,
	USB_max_string_descriptor_size =	 	 	0x200U
} USB_string_descriptor_size_t;

typedef enum {
	USB_bus_powered =	0xA0U,
	USB_self_powered =	0xE0U
} USB_power_type_t;


/*!<
 * variables
 * */
extern uint32_t USB_kernel_frequency;


/*!<
 * init
 * */  // TODO: collapse unneeded fconfig functions!!!!!!!
void config_USB_kernel_clock(USB_CLK_SRC_t src);

void fconfig_USB_FS_device(USB_GPIO_t dp, USB_GPIO_t dn, uint32_t RX_FIFO_size);
void config_USB_FS_device(USB_GPIO_t dp, USB_GPIO_t dn);
// TODO: external phy, host mode

void fconfig_USB_interface(USB_OTG_GlobalTypeDef* usb, USB_descriptor_t* descriptor, USB_class_t* class);
void config_USB_interface(USB_OTG_GlobalTypeDef* usb);

USB_class_t* init_class(USB_handle_t* handle, USB_class_type_t class_type, uint8_t iep_num, uint8_t oep_num);
void* write_descriptor(void* ptr, USB_descriptor_type_t type, ...);

void config_USB_RX_FIFO(USB_OTG_GlobalTypeDef* usb, uint32_t size);
void config_USB_TX_FIFO(USB_OTG_GlobalTypeDef* usb, uint8_t ep, uint32_t size);

void start_USB(USB_OTG_GlobalTypeDef* usb);
void stop_USB(USB_OTG_GlobalTypeDef* usb);

// TODO: deinit func

#endif // USB_LIB
#endif // STM32H_CMSIS_USB_H
