//
// Created by marijn on 7/19/23.
//

#include "usb.h"


/*!<
 * variables
 * */
uint32_t USB_kernel_frequency = 0;


/*!<
 * static
 * */
static inline uint8_t USB_to_IRQn(USB_OTG_GlobalTypeDef* usb) {
	if (usb == USB_OTG_HS) { return OTG_HS_IRQn; }
	return OTG_FS_IRQn;
}

static inline void OTG_common_handler(USB_OTG_GlobalTypeDef* usb) {
	if (usb->GINTSTS & USB_OTG_GINTSTS_USBRST) {  // reset
		usb->GINTSTS = USB_OTG_GINTSTS_USBRST;
	}

	if (usb->GINTSTS & USB_OTG_GINTSTS_ENUMDNE) {  // enumeration done
		usb->GINTSTS = USB_OTG_GINTSTS_ENUMDNE;
	}

	if (usb->GINTSTS & USB_OTG_GINTSTS_RXFLVL) {  // rx FIFO non-empty
		usb->GINTSTS = USB_OTG_GINTSTS_RXFLVL;
	}
}


/*!<
 * interrupts
 * */
extern void OTG_HS_IRQHandler(void) { OTG_common_handler(USB_OTG_HS); }
extern void OTG_FS_IRQHandler(void) { OTG_common_handler(USB_OTG_FS); }


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

	/* GPIO config */
	fconfig_GPIO(dp_port, dp_dev.pin_num, GPIO_alt_func, GPIO_pull_up, GPIO_open_drain, GPIO_high_speed, dp_dev.alt_func);
	fconfig_GPIO(dn_port, dn_dev.pin_num, GPIO_alt_func, GPIO_pull_up, GPIO_open_drain, GPIO_high_speed, dn_dev.alt_func);

	/* power config */
	PWR->CR3 |= (
		PWR_CR3_USBREGEN	|					// TODO: needed?
		PWR_CR3_USB33DEN	|
		PWR_CR3_SCUEN		|
		PWR_CR3_LDOEN
	);

	/* core config */
	usb->GOTGCTL = (
		0b1U << USB_OTG_GOTGCTL_OTGVER_Pos		// OTG Version 2.0
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

	usb->GRXFSIZ = 0x80;						// rx FIFO size
	usb->DIEPTXF0_HNPTXFSIZ = (
		(0x40 << 16)	|						// tx FIFO size
		0x80									// FIFO memory offset
	);  // TODO: research!!!!

	/* device config */
	device->DCFG = (
		0b11U << USB_OTG_DCFG_DSPD_Pos			// use internal FS PHY
	);

	// device->DCTL = ();
	// device->DSTS

	// common endpoint initialization
	device->DIEPMSK = (
		0b1U << USB_OTG_DIEPMSK_XFRCM_Pos		// enable transfer completed interrupt
	);
	device->DOEPMSK = (
		0b1U << USB_OTG_DIEPMSK_XFRCM_Pos		// enable transfer completed interrupt
	);

	device->DAINTMSK = (
		0b1U << USB_OTG_DAINTMSK_OEPM_Pos		|	// endpoint 0 OUT
		0b1U << USB_OTG_DAINTMSK_IEPM_Pos		|	// endpoint 0 IN
		0b1U << (USB_OTG_DAINTMSK_OEPM_Pos + 1)	|	// endpoint 1 OUT
		0b1U << (USB_OTG_DAINTMSK_IEPM_Pos + 1)		// endpoint 1 IN
	);

	// TODO: only config EP0 and let other functions do the rest
	/* endpoint config */
	in[0].DIEPCTL = (
		0  // TODO
	);
	in[1].DIEPCTL = (
		0  // TODO
	);

	out[0].DOEPCTL = (
		0  // TODO
	);
	out[1].DOEPCTL = (
		0  // TODO
	);


	NVIC->ISER[(USB_to_IRQn(usb) >> 5UL)] = 1UL << (USB_to_IRQn(usb) & 0x1FUL);  // enable OTG_FS_IRQn

	// start USB connection
	device->DCTL |= USB_OTG_DCTL_SDIS;		// soft disconnect
	device->DCTL &= ~USB_OTG_DCTL_SDIS;		// connect

	// TODO: PCGCCTL??
	return;
}


void config_USB_FS_device(USB_GPIO_t dp, USB_GPIO_t dn) {
	fconfig_USB_FS_device(dp, dn);
}