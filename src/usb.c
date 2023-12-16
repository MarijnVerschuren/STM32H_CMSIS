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

// TODO: ULPI?
/* uint16_t				dp_ulpi = dp_dev.dev_id.sub,				dn_ulpi = dn_dev.dev_id.sub;
dev_id_t				ulpi = {0, 0, 0}; if (dp_ulpi == dn_ulpi) { ulpi = *((dev_id_t*)&dp_ulpi); }
if (ulpi.clk)			{ enable_id(ulpi); } */
void fconfig_USB_FS_device(USB_GPIO_t dp, USB_GPIO_t dn) {
	if (dp == USB_PIN_DISABLE || dn == USB_PIN_DISABLE) { return; }
	dev_pin_t					dp_dev = *((dev_pin_t*)&dp),				dn_dev = *((dev_pin_t*)&dn);
	USB_OTG_GlobalTypeDef		*dp_usb = (void*)(((dp_dev.dev_id.num - 25) << 17) + USB1_OTG_HS_PERIPH_BASE),
								*dn_usb = (void*)(((dn_dev.dev_id.num - 25) << 17) + USB1_OTG_HS_PERIPH_BASE),
								*usb = NULL;
	USB_OTG_DeviceTypeDef		*device =	(void*)((uint32_t)usb + 0x800);
	USB_OTG_INEndpointTypeDef	*in =		(void*)((uint32_t)usb + 0x900);
	USB_OTG_OUTEndpointTypeDef	*out =		(void*)((uint32_t)usb + 0xB00);
	GPIO_TypeDef				*dp_port = int_to_GPIO(dp_dev.port_num),	*dn_port = int_to_GPIO(dn_dev.port_num);
	if (dp_usb != dn_usb) { return; } usb = dp_usb; enable_id(dp_dev.dev_id);

	PWR->CR3 |= (
		PWR_CR3_USBREGEN	|		// TODO: needed?
		PWR_CR3_USB33DEN	|
		PWR_CR3_SCUEN		|
		PWR_CR3_LDOEN
	);

	fconfig_GPIO(dp_port, dp_dev.pin_num, GPIO_alt_func, GPIO_pull_up, GPIO_open_drain, GPIO_high_speed, dp_dev.alt_func);
	fconfig_GPIO(dn_port, dn_dev.pin_num, GPIO_alt_func, GPIO_pull_up, GPIO_open_drain, GPIO_high_speed, dn_dev.alt_func);

	usb->GOTGCTL = (
		0b1U << USB_OTG_GOTGCTL_OTGVER_Pos	// OTG Version 2.0
	);

	/// usb->GOTGINT TODO: @2624
	/// usb->GAHBCFG TODO: @2626

	usb->GUSBCFG = (
		0b10U << USB_OTG_GUSBCFG_FHMOD_Pos	|	// force device device mode
		0b1U << USB_OTG_GUSBCFG_PHYSEL_Pos		// set FS mode
	);

	usb->GINTMSK = (
		USB_OTG_GINTMSK_RXFLVLM				|	// enable receive FIFO non-empty interrupt
		USB_OTG_GINTMSK_USBRST				|	// enable USB reset interrupt
		USB_OTG_GINTMSK_ENUMDNEM			|	// enable enumeration done interrupt
		USB_OTG_GINTMSK_IEPINT				|	// enable IN endpoints interrupt
		USB_OTG_GINTMSK_OEPINT					// enable OUT endpoints interrupt
	);
	usb->GINTSTS = 0xFFFCFCFF;					// clear all interrupts

	usb->GCCFG = (
		0b1U << USB_OTG_GCCFG_VBDEN_Pos			// enable Vbus detection
	);

	// usb->CID	-> 32-bit id TODO !!!!!!

	device->DCFG = (
		0b11U << USB_OTG_DCFG_DSPD_Pos		// use internal FS PHY
	);

	// device->DCTL = ();

	// device->DSTS

	// device->DIEPMSK		TODO!!!!!!!!!!

	// device->DOEPMSK		TODO!!!!!!!!!!

	//

	/* GPT CODE: TODO: validate / test (testing in: main.c)
	// Basic USB_OTG initialization for device mode
	USB_OTG_FS->GUSBCFG |= USB_OTG_GUSBCFG_PHYSEL;  // Select FS Interface
	USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_PWRDWN;     // Power down deactivation

	// Core initialization
	USB_OTG_FS->GINTSTS = 0xFFFFFFFF;              // Clear all interrupts
	USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM | USB_OTG_GINTMSK_USBRST |
						   USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_IEPINT |
						   USB_OTG_GINTMSK_OEPINT; // Enable required interrupts

	USB_OTG_DeviceTypeDef*		device =	(void*)((uint32_t)USB_OTG_FS + 0x800);
	USB_OTG_INEndpointTypeDef*	in =		(void*)((uint32_t)USB_OTG_FS + 0x900);
	// Device mode initialization
	device->DCFG |= USB_OTG_DCFG_DSPD;         // Set device speed to Full Speed

	// Endpoint 0 initialization (Control endpoint)
	device->DIEPMSK |= USB_OTG_DIEPMSK_XFRCM;  // Transfer completed interrupt mask
	device->DOEPMSK |= USB_OTG_DOEPMSK_XFRCM;  // Transfer completed interrupt mask

	in->DIEPCTL &= ~USB_OTG_DIEPCTL_MPSIZ;  // Clear max packet size
	in->DIEPCTL |= 0x40;                     // Set max packet size to 64 bytes

	USB_OTG_FS->GRXFSIZ = 0x80;                       // Set Rx FIFO size
	USB_OTG_FS->DIEPTXF0_HNPTXFSIZ = (0x40 << 16) | 0x80;  // Set EP0 Tx FIFO size

	// Unmask EP0 interrupts
	device->DAINTMSK |= (1 << 0) | (1 << 16);    // IN EP0 and OUT EP0
	*/

	return;
}


void config_USB_FS_device(USB_GPIO_t dp, USB_GPIO_t dn) {
	fconfig_USB_FS_device(dp, dn);
}