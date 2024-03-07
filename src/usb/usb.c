//
// Created by marijn on 7/19/23.
//

#include "usb/usb.h"


#ifdef USB_LIB


/*!<
 * variables
 * */
uint32_t USB_kernel_frequency = 0;
USB_handle_t* USB_handle[2] = {NULL, NULL};


/*!<
 * static / hidden
 * */
static inline IRQn_Type USB_to_IRQn(USB_OTG_GlobalTypeDef* usb) {
	if (usb == USB_OTG_HS) { return OTG_HS_IRQn; }
	return OTG_FS_IRQn;
}

void flush_RX_FIFO(USB_OTG_GlobalTypeDef* usb) {
	while (!(usb->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL));		// wait for AHB master IDLE state
	usb->GRSTCTL = USB_OTG_GRSTCTL_RXFFLSH;					// flush RX FIFO
	while (usb->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH);			// wait until reset is processed
}
void flush_TX_FIFO(USB_OTG_GlobalTypeDef* usb, uint8_t ep) {
	while (!(usb->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL));		// wait for AHB master IDLE state
	usb->GRSTCTL = (
		ep << USB_OTG_GRSTCTL_TXFNUM_Pos		|			// select ep TX FIFO
		0b1UL << USB_OTG_GRSTCTL_TXFFLSH_Pos				// flush TX FIFO
	);
	while (usb->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH);			// wait until reset is processed
}
void flush_TX_FIFOS(USB_OTG_GlobalTypeDef* usb) {
	while (!(usb->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL));		// wait for AHB master IDLE state
	usb->GRSTCTL = (
		0x10UL << USB_OTG_GRSTCTL_TXFNUM_Pos		|		// select all TX FIFOs
		0b1UL << USB_OTG_GRSTCTL_TXFFLSH_Pos				// flush TX FIFOs
	);
	while (usb->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH);			// wait until reset is processed
}


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

void fconfig_USB_FS_device(USB_GPIO_t dp, USB_GPIO_t dn, uint32_t RX_FIFO_size) {
	/* argument and variable setup */
	if (dp == USB_PIN_DISABLE || dn == USB_PIN_DISABLE) { return; }
	dev_pin_t					dp_pin =	*((dev_pin_t*)&dp),
								dn_pin =	*((dev_pin_t*)&dn);
	USB_OTG_GlobalTypeDef		*dp_usb =	(void*)(((dp_pin.id.num - 25) << 17) + USB1_OTG_HS_PERIPH_BASE),
								*dn_usb =	(void*)(((dn_pin.id.num - 25) << 17) + USB1_OTG_HS_PERIPH_BASE),
								*usb =		dp_usb;
	USB_OTG_DeviceTypeDef		*device =	(void*)((uint32_t)usb + 0x800);
	USB_OTG_INEndpointTypeDef	*in =		(void*)((uint32_t)usb + 0x900);
	USB_OTG_OUTEndpointTypeDef	*out =		(void*)((uint32_t)usb + 0xB00);
	volatile uint32_t			*PCGCCTL =	(void*)((uint32_t)usb + 0xE00);
	GPIO_TypeDef				*dp_port =	int_to_GPIO(dp_pin.port),
								*dn_port =	int_to_GPIO(dn_pin.port);
	if (dp_usb != dn_usb) { return; }  // error if devices do not match up

	/* GPIO config */
	fconfig_GPIO(dp_port, dp_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_open_drain, GPIO_high_speed, dp_pin.alt);
	fconfig_GPIO(dn_port, dn_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_open_drain, GPIO_high_speed, dn_pin.alt);

	/* enable USB device clock and global interrupt */
	enable_id(dp_pin.id);

	usb->GAHBCFG = 0x00000000UL;							// clear GAHBCFG config to disable interrupts during setup
	IRQn_Type irqn = USB_to_IRQn(usb);
	set_IRQ_priority(irqn, 0);
	enable_IRQ(irqn);

	/* power config */
	PWR->CR3 |= (
		PWR_CR3_USBREGEN	|	// TODO: needed?
		PWR_CR3_USB33DEN	|
		PWR_CR3_SCUEN		|
		PWR_CR3_LDOEN
	);

	/* phy and mode config */
	usb->GUSBCFG = (
		USB_OTG_GUSBCFG_FDMOD	|							// force device device mode
		USB_OTG_GUSBCFG_PHYSEL								// select internal PHY
	);
	while (usb->GINTSTS & USB_OTG_GINTSTS_CMOD);			// wait until device mode is set

	while (!(usb->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL));		// wait for AHB master IDLE state
	usb->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;					// reset the core
	while (usb->GRSTCTL & USB_OTG_GRSTCTL_CSRST);			// wait until reset is processed

	usb->GCCFG = (
		USB_OTG_GCCFG_VBDEN		|							// enable Vbus detection
		USB_OTG_GCCFG_PWRDWN								// power up internal PHY transceiver
	);

	*PCGCCTL = 0x00000000UL;								// restart PHY clock

	device->DCFG = (
		0b00UL << USB_OTG_DCFG_PFIVL_Pos			|		// 80% (TODO: try 90%)
		0b11UL << USB_OTG_DCFG_DSPD_Pos						// FS on internal PHY
	);

	/* FIFO buffer setup */
	uint8_t i;
	for (i = 0UL; i < 0xFUL; i++) {
		usb->DIEPTXF[i] = 0x00000000UL;						// clear IN endpoint FIFO sizes/offsets
	}

	flush_TX_FIFOS(usb);
	flush_RX_FIFO(usb);

	/* endpoint and device interrupt config */
	device->DIEPMSK =	0x00000000UL;						// mask all IN endpoint interrupts
	device->DOEPMSK =	0x00000000UL;						// mask all OUT endpoint interrupts
	device->DAINTMSK =	0x00000000UL;						// mask all endpoint generated interrupts

	for (i = 0x0UL; i < USB_OTG_ENDPOINT_COUNT; i++) {
		if (in[i].DIEPCTL & USB_OTG_DIEPCTL_EPENA) {
			if (i == 0x0UL) { in[i].DIEPCTL = USB_OTG_DIEPCTL_SNAK; }
			else { in[i].DIEPCTL = USB_OTG_DIEPCTL_EPDIS | USB_OTG_DIEPCTL_SNAK; }
		} else { in[i].DIEPCTL = 0x00000000UL; }

		if (out[i].DOEPCTL & USB_OTG_DOEPCTL_EPENA) {
			if (i == 0x0UL) { out[i].DOEPCTL = USB_OTG_DOEPCTL_SNAK; }
			else { out[i].DOEPCTL = USB_OTG_DOEPCTL_EPDIS | USB_OTG_DOEPCTL_SNAK; }
		} else { out[i].DOEPCTL = 0x00000000UL; }

		in[i].DIEPTSIZ	= out[i].DOEPTSIZ	= 0x00000000UL;	// clear transfer config
		in[i].DIEPINT	= out[i].DOEPINT	= 0xFB7FUL;		// clear all interrupt status bits
	}

	/* usb core interrupt config */
	usb->GINTMSK = 0x00000000UL;							// mask all interrupts
	usb->GINTSTS = 0xBFFFFFFFUL;							// clear all interrupts (except for session request interrupt)
	usb->GINTMSK = (
		USB_OTG_GINTMSK_WUIM			|					// resume / remote wake-up detected interrupt
		USB_OTG_GINTMSK_SRQIM			|					// session request / new session detected interrupt
		USB_OTG_GINTMSK_DISCINT			|					// disconnect detected interrupt
		USB_OTG_GINTMSK_PXFRM_IISOOXFRM	|					// incomplete periodic transfer interrupt / incomplete isochronous OUT transfer interrupt
		USB_OTG_GINTMSK_IISOIXFRM		|					// incomplete isochronous IN transfer interrupt
		USB_OTG_GINTMSK_OEPINT			|					// OUT endpoints interrupt
		USB_OTG_GINTMSK_IEPINT			|					// IN endpoints interrupt
		USB_OTG_GINTMSK_ENUMDNEM		|					// enumeration done interrupt
		USB_OTG_GINTMSK_USBRST			|					// USB reset interrupt
		USB_OTG_GINTMSK_USBSUSPM		|					// USB suspend interrupt
		USB_OTG_GINTMSK_RXFLVLM			|					// RX FIFO non empty interrupt
		USB_OTG_GINTMSK_SOFM			|					// start of frame interrupt
		USB_OTG_GINTMSK_OTGINT								// OTG interrupt
	);

	/* disconnect */
	*PCGCCTL &= ~(
		USB_OTG_PCGCCTL_STOPCLK			|
		USB_OTG_PCGCCTL_GATECLK
	);
	device->DCTL |= USB_OTG_DCTL_SDIS;

	/* setup FIFO buffers */
	config_USB_RX_FIFO(USB2_OTG_FS, RX_FIFO_size);
	config_USB_TX_FIFO(USB2_OTG_FS, 0, 0x40);		// max setup packet size
}

void config_USB_FS_device(USB_GPIO_t dp, USB_GPIO_t dn) {
	fconfig_USB_FS_device(dp, dn, 0x80);
}


void config_USB_RX_FIFO(USB_OTG_GlobalTypeDef* usb, uint32_t size) {
	usb->GRXFSIZ = size;
	usb->DIEPTXF0_HNPTXFSIZ = (
		(usb->DIEPTXF0_HNPTXFSIZ & 0xFFFF0000UL) |
		size		// offset
	);
	size += usb->DIEPTXF0_HNPTXFSIZ >> 16;
	// fix FIFO offsets for all IN endpoints
	for (uint8_t i = 0; i < 0xEUL; i++) {
		usb->DIEPTXF[i] = (
			(usb->DIEPTXF[i] & 0xFFFF0000UL) |
			size	// offset
		);
		size += usb->DIEPTXF[i] >> 16;
	}
}
void config_USB_TX_FIFO(USB_OTG_GlobalTypeDef* usb, uint8_t ep, uint32_t size) {
	uint32_t offset = usb->GRXFSIZ;
	uint32_t block_size = (usb->DIEPTXF0_HNPTXFSIZ >> 16);
	if (ep == 0) { block_size = size; }	ep -= 1;
	usb->DIEPTXF0_HNPTXFSIZ = (
		(block_size << 16) |
		offset
	);	offset += block_size;
	// fix FIFO offsets for all IN endpoints (and set size)
	for (uint8_t i = 0; i < 0xEUL; i++) {
		block_size = usb->DIEPTXF[i] >> 16;
		if (ep == i) { block_size = size; }
		usb->DIEPTXF[i] = (
			(block_size << 16) |
			offset
		);	offset += block_size;
	}
}


void start_USB(USB_OTG_GlobalTypeDef* usb) {
	register USB_OTG_DeviceTypeDef	*device =	(void*)((uint32_t)usb + 0x800);
	register volatile uint32_t		*PCGCCTL =	(void*)((uint32_t)usb + 0xE00);
	usb->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
	*PCGCCTL &= ~(USB_OTG_PCGCCTL_STOPCLK | USB_OTG_PCGCCTL_GATECLK);
	device->DCTL &= ~USB_OTG_DCTL_SDIS;
}
void stop_USB(USB_OTG_GlobalTypeDef* usb) {
	register USB_OTG_DeviceTypeDef	*device =	(void*)((uint32_t)usb + 0x800);
	register volatile uint32_t		*PCGCCTL =	(void*)((uint32_t)usb + 0xE00);
	usb->GAHBCFG &= ~USB_OTG_GAHBCFG_GINT;
	*PCGCCTL &= ~(USB_OTG_PCGCCTL_STOPCLK | USB_OTG_PCGCCTL_GATECLK);
	device->DCTL |= USB_OTG_DCTL_SDIS;
	flush_TX_FIFOS(usb);
}

#endif // USB_LIB