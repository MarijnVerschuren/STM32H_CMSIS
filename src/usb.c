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
void fconfig_USB_FS(USB_GPIO_t dp, USB_GPIO_t dn) {
	if (dp == USB_PIN_DISABLE || dn == USB_PIN_DISABLE) { return; }
	dev_pin_t				dp_dev = *((dev_pin_t*)&dp),				dn_dev = *((dev_pin_t*)&dn);
	USB_OTG_GlobalTypeDef	*dp_usb = (void*)(((dp_dev.dev_id.num - 25) << 17) + USB1_OTG_HS_PERIPH_BASE),
							*dn_usb = (void*)(((dn_dev.dev_id.num - 25) << 17) + USB1_OTG_HS_PERIPH_BASE),
							*usb = NULL;
	GPIO_TypeDef			*dp_port = int_to_GPIO(dp_dev.port_num),	*dn_port = int_to_GPIO(dn_dev.port_num);
	if (dp_usb != dn_usb) { return; } usb = dp_usb; enable_id(dp_dev.dev_id);

	usb->GOTGCTL = (
		0b1 << USB_OTG_GOTGCTL_OTGVER_Pos	|	// OTG Version 2.0
		0
	);
	// EHEN			->	embedded host enable		-> ???
	// DHNPEN		->	device HNP enable			|
	// HSHNPEN		->	host HNP enable				| Host negotiation
	// HNPRQ		->	HNP request					| ????
	// HNGSCS (r)	->	Host negotiation success	|
	// BVALOVAL		->	B-peripheral session valid override value	|
	// BVALOEN		->	B-peripheral session valid override enable	| A/B - peripheral session
	// AVALOVAL		->	A-peripheral session valid override value	| ????
	// AVALOEN		->	A-peripheral session valid override enable	|
	// VBVALOVAL	->	VBUS valid override value	| VBUS valid signal
	// VBVALOEN		->	VBUS valid override enable	| ????

	// SRQ			->	Session request				| IMPORTANT??
	// SRQSCS (r)	->	Session request success		| !!!!
	

	usb->GAHBCFG = (
		0
	);

	return;
}

void config_USB_FS(USB_GPIO_t dp, USB_GPIO_t dn) {
	fconfig_USB_FS(dp, dn);
}