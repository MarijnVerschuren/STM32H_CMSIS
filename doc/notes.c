/// usb->GOTGCTL					TODO: [57.14.1		OTG control and status register]							@2621
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

/// usb->GOTGINT					TODO: [57.14.2		OTG interrupt register]										@2624
// DBCDNE		->	Debounce done (host only)
// ADTOCHG		->	A-device timeout change
// HNGDET		->	Nost negotiation detected
// HNSSCHG		->	Host negotiation success status change
// SRSSCHG		->	Session request success status change
// SEDET		->	Session end detected

/// usb->GAHBCFG					TODO: [57.14.3		OTG AHB configuration register]								@2626
// PTXFELVL		->	TX FIFO buffer interrupt trigger
// TXFELVL		->	^
// DMAEN		->	dma enable					| DMA STUFF
// HBSTLEN		->	bus transaction length		| ???
// GINTMSK		->	global interrupt mask

/// usb->GUSBCFG					TODO:  [57.14.4	OTG USB configuration register]									@2627
// ULPIIPD		->	ULPI interface protect disable
// PTCI			->	disable complement output qualification with internal Vbus
// PCCI			->	enable external/Vbus indicator signal inversion by PHY
// TSDPS		->	data line pulsing using (0b0: utmi_txvalid, 0b1: utmi_termsel)
// ULPIEVBUSI	->	PHY uses an exernal Vbus valid comparator
// ULPIEVBUSD	->	PHY drives Vbus using external supply									| TODO
// ULPICSM		->	PHY does (0b1: not) power down internal clock when suspended
// ULPAIR		->	enable auto resume on PHY												| ??
// ULPIFSLS		->	ULPI (0b1: FS/LS serial) interface
// PHYLPC		->	(0b0: 480 MHz internal PLL clock, 0b1: 48 MHz external clock)			| TODO!!!
// TRDT			->	USB turnaround time TODO: configure according to [Table 519]
// HNPCAP		->	HNP capability enabled
// SRPCAP		->	SRP capability enabled
// PHYSEL		->	(0b0: USB 2.0 external ULPI high-speed PHY, 0b1: 1.1 full-speed serial)	|
// TOCAL		->	FS timeout calibration													| TODO

/// usb->GRSTCTL					TODO: [57.14.5		OTG reset register]											@2630
// AHBIDL	(r)	->	indicates that AHB master state machine is in the idle state
// DMAREQ	(r)	->	indicates that a DMA request is in progress
// TXFNUM		->	the FIFO number that should be flushed using the flush bit
// TXFFLSH		->	flush selection in TXFNUM
// RXFFLSH		->	flush entire RX FIFO
// FCRST		->	reset host frame counter
// PSRST		->	partial soft reset	TODO: check documentation
// CSRST		->	core soft reset		TODO: check documentation

/// usb->GINTSTS					TODO: [57.14.6		OTG core interrupt register]								@2633
/// usb->GINTMSK					TODO: [57.14.7		OTG interrupt mask register]								@2637
/*!<
	usb->GRXSTSR (device)			TODO: [57.14.8		OTG receive status debug read register]						@2641
	usb->GRXSTSP (device)			TODO: [57.14.10		OTG status read and pop registers]							@2643
*/
// STSPHST	(r)	->	status phrase start
// FRMNUM	(r)	->	frame number
// PKTSTS	(r)	->	packet status
//	- 0001:	global out NAK (interrupt trigger)
//	- 0010:	out data packet received
//	- 0011:	out transfer completed (interrupt trigger)
//	- 0100: setup transaction completed (interrupt trigger)
//	- 0110: setup data packet received
// DPID		(r)	->	data PID		(0b00: DATA0, 0b01: DATA2, 0b10: DATA1, 0b11: MDATA)
// BCNT		(r) ->	transmitted byte count	TODO: transmitted or received?
// EPNUM	(r)	->	endpoint number

/*!<
	usb->GRXSTSR (host)				TODO: [57.14.9		OTG receive status debug read]								@2642
	usb->GRXSTSP (host)				TODO: [57.14.11		OTG status read and pop registers]							@2644
*/
// PKTSTS	(r)	->	packet status
//	- 0010:	in data packet received
//	- 0011:	in transfer completed (interrupt trigger)
//	- 0101: data toggle error (interrupt trigger)
//	- 0111: channel halted (interrupt trigger)
// DPID		(r)	->	data PID		(0b00: DATA0, 0b01: DATA2, 0b10: DATA1, 0b11: MDATA)
// BCNT		(r) ->	received byte count		TODO: transmitted or received?
// CHNUM	(r)	->	channel number

/// usb->GRXFSIZ					TODO: [57.14.12		OTG receive FIFO size register]								@2645
// RXFD			->	RX FIFO depth in words (max: 1024)

/*!<
	usb->HNPTXFSIZ					TODO: [57.14.13		OTG host non-periodic transmit FIFO size register]			@2645
	usb->DIEPTXF0					TODO: [57.14.13   Endpoint 0 Transmit FIFO size]								@2645
*/

/// usb->HNPTXSTS					TODO: [57.14.14		OTG non-periodic transmit FIFO/queue status register]		@2646
/// usb->GCCFG						TODO: [57.14.15		OTG general core configuration register]					@2647
// VBDEN		->	USB Vbus detection enable
// SDEN			->	secondary detection mode enable			|
// PDEN			->	primary detection mode enable			| TODO
// DCDEN		->	data contact detection mode enable		|
// BCDEN		->	battery charging detector enable		|
// PWRDWN		->	powerdown control of FS PHY
// PS2DET	(r)	->	D(N/M) pull-up detection status
// SDET		(r)	->	secondary detection status
// PDET		(r)	->	primary detection status
// DCDET	(r)	->	data contact detection status

/// usb->CID						TODO: [57.14.16		OTG core ID register]										@2649
/// usb->GLPMCFG					TODO: [57.14.17		OTG core LPM configuration register]						@2649
// ENBESL		->	LPM errata best effort service latency enable TODO ??
// LPMRCNTSTS(r)->	LPM retry count status
// SNDLPM		->	send LPM transaction
// LPMRCNT		->	LPM retry count
// LPMCHIDX		->	LPM channel index
// L1RSMOK	(r)	->	sleep state resume OK
// SLPSTS	(r) ->	port sleep status
// LPMRSP	(r) ->	LPM response
// L1DSEN		->	L1 deep sleep enable
// BESLTHRS		->	BESL threshold
//	- DEVICE:	the core puts the PHY into deep sleep (L1) when BESL > BESL_THRESHOLD
//	- HOST:		the core puts the PHY into deel sleep (L1) for x us TODO: doc
// L1SSEN		->	L1 shallow sleep enable
// REMWAKE	(r in device mode) -> TODO
// BESL			->	best effort service latency TODO: doc
// LPMACK		->	LPM token acknowledge enable
// LPMEN		->	LPM support enable

/// usb->HPTXFSIZ					TODO: [57.14.18		OTG host periodic transmit FIFO size register]				@2653
/// usb->DIEPTXF[0 - 7]				TODO: [57.14.19		OTG device IN endpoint transmit FIFO x size register]		@2653


/*!< HOST MODE REGISTERS			TODO: [57.14.20   Host-mode registers] @2654 */


/// device->DCFG					TODO: [57.14.39		OTG device configuration register]							@2672
// PERSCHIVL	->	periodic schedule interval (specifies the ammount of time the internal DMA engine must allocate for fetching periodic IN endpoint data)
// DESCDMA		->	enable scatter/gather DMA in device mode
// ERRATIM		->	erratic error interrupt mask
// XCVRDLY		->	tranceiver delay
// PFIVL		->	periodic frmae interval
// DAD			->	device address TODO!!!!!!!!!!!!
// NZLSOHSK		->	non-zero-length status OUT handshake TODO!!!!!!
// DSPD			->	device speed

/// device->DCTL					TODO: [57.14.40		OTG device control register]								@2674
// DSBESLRJCT	->	deep sleep BESL reject
// ENCONTONBNA	->	enable continue on BNA
// POPRGDNE		->	power-on programming done TODO completes init!!!!
// CGONAK		->	clear global out NAK
// SGONAK		->	set global out NAK
// CGINAK		->	clear global in NAK
// SGINAK		->	set global in NAK
// TCTL			->	test control
// GONSTS		->	global out NAK status
// GINSTS		->	global in NAK status
// SDIS			->	soft disconnect
// RWUSIG		->	remote wake-up signaling	// TODO??

/// device->DSTS					TODO: [57.14.41		OTG device status register]									@2676
// DEVLNSTS	(r)	->	device line status
// FNSOF	(r)	->	frame number of the received SOF
// EERR		(r)	->	erratic error
// ENUMSPD	(r)	->	enumerated speed
// SUSPSTS	(r)	->	suspend status

/// device->DIEPMSK					TODO: [57.14.42		OTG device IN endpoint common interrupt mask register]		@2677
// NAKM			->	NAK interrupt mask
// BNAM			->	BNA interrupt mask
// TXFURM		->	FIFO underrun mask
// INEPNEM		->	IN endpoint NAK effective interrupt mask
// INEPNMM		->	IN token received with EP mismatch interrupt mask
// ITTXFEMSK	->	IN token received when Tx FIFO empty interrupt mask
// TOM			->	timeout condition interrupt mask
// AHBERRM		->	AHB error interrupt mask
// EPDM			->	endpoint disabled interrupt mask
// XFRCM		->	transfer completed interrupt mask

/// device->DOEPMSK					TODO: [57.14.43		OTG device OUT endpoint common interrupt mask register]		@2678
// NYETMSK		->	NYET interrupt mask
// NAKMSK		->	NAK interrupt mask
// BERRM		->	babble error interrupt mask
// BNAM			->	BNA interrupt mask
// OUTPKTERRM	->	OUT packet error interrupt mask
// B2BSTUPM		->	back-to-back setup packets received interrupt mask
// STSPHSRXM	->	status phase received for control write interrupt mask
// OTEPDM		->	OUT token received when endpoint disabled interrupt mask
// STUPM		->	setup phase done interrupt mask
// AHBERRM		->	AHB error interrupt mask
// EPDM			->	endpoint disabled interrupt mask
// XFRCM		->	transfer completed interrupt mask

/// device->DAINT					TODO: [57.14.44		OTG device all endpoints interrupt register]				@2679
/// device->DAINTMSK				TODO: [57.14.45		OTG all endpoints interrupt mask register]					@2680
/// device->DVBUSDIS				TODO: [57.14.46		OTG device Vbus discharge time register]					@2681
/// device->DVBUSPULSE				TODO: [57.14.47		OTG device Vbus pulsing time register]						@2681
/// device->DTHRCTL					TODO: [57.14.48		OTG device threshold control register]						@2682
/// device->DIEPEMPMSK				TODO: [57.14.49		OTG device IN endpoint FIFO empty interrupt mask register]	@2683
/// device->DEACHINT				TODO: [57.14.50		OTG device each endpoint interrupt register]				@2683
/// device->DEACHINTMSK				TODO: [57.14.51		OTG device each endpoint interrupt mask register]			@2684
/// device->HS_DIEPEACHMSK1			TODO: [57.14.52		OTG device each IN endpoint-1 interrupt mask register]		@2684
/// device->HS_DOEPEACHMSK1			TODO: [57.14.53		OTG device each OUT endpoint-1 interrupt mask register]		@2685

// (x = 0 to 8)
/// in[x]->DIEPCTL					TODO: [57.14.54		OTG device IN endpoint x control register]					@2687
// EPENA			->	endpoint enable
// EPDIS			->	endpoint disable
// SODDFRM			->	set odd frame	(isochronous endpoints)
// SD0PID/SEVNFRM	->	set DATA0 PID / set even frame
// 	- interrupt/bulk endpoints	->	write DPID to DATA0
//	- isochronous endpoints		->	set even frame
// SNAK				->	set NAK
// CNAK				->	clear NAK
// TXFNUM (IN)		->	tx FIFO number
// STALL			->	STALL handshake	(non-controll, non isochronous, IN endpoints)
// EPTYP			->	endpoint type
//  - 00: control
//  - 01: isochronous
//  - 10: bulk
//  - 11: interrupt
// NAKSTS		(r)	->	NAK status
// EONUM/DPID	(r)	->	even/odd fame / endpoint data PID
//	- isochronous endpoints		->	EONUM (0b0: even, 0b1: odd)
//  - interrupt/bulk endpoints	->	DPID (0b0: DATA0, 0b1: DATA1)
// USBAEP		(r?)->	USB active endpoint TODO!!
// MPSIZ			->	maximum packet size
/// in[x]->DIEPINT					TODO: [57.14.55		OTG device IN endpoint x interrupt register]				@2689
/// in[0]->DIEPTSIZ					TODO: [57.14.56		OTG device IN endpoint 0 transfer size register]			@2691
// PKTCNT			->	packet count
// XFRSIZ			->	transfer size
/// in[x]->DIEPDMA					TODO: [57.14.57		OTG device IN endpoint x DMA address register]				@2691
/// in[x]->DTXFSTS					TODO: [57.14.58		OTG device IN endpoint transmit FIFO status register]		@2692
/// in[x]->DIEPTSIZ (x > 0)			TODO: [57.14.59		OTG device IN endpoint x transfer size register]			@2692
// MCNT				->	multi count
// PKTCNT			->	packet count
// XFRSIZ			->	transfer size
/// out[0]->DOEPCTL					TODO: [57.14.60		OTG device control OUT endpoint 0 control register]			@2693
// EPENA			->	endpoint enable
// EPDIS		(r)	->	endpoint disabled
// SNAK				->	set NAK
// CNAK				->	clear NAK
// STALL		(rs)->	STALL handshake
// SNPM				->	snoop mode TODO
// EPTYP		(r) ->	endpoint type (for endpoint 0 it is hardcoded to 0b00)
// NAKSTS		(r)	->	NAK status
// USBAEP		(r) ->	USB active endpoint
// MPSIZ			->	maximum packet size
//  - 0b00: 64 bytes
//  - 0b01: 32 bytes
//  - 0b10: 16 bytes
//  - 0b11: 8 bytes
/// out[x]->DOEPINT					TODO: [57.14.61		OTG device OUT endpoint x interrupt register]				@2695
/// out[0]->DOEPTSIZ				TODO: [57.14.62		OTG device OUT endpoint 0 transfer size register]			@2697
/// out[x]->DOEPDMA					TODO: [57.14.63		OTG device OUT endpoint x DMA address register]				@2698
/// out[x]->DOEPCTL (x > 0)			TODO: [57.14.64		OTG device OUT endpoint x control register]					@2698
// EPENA			->	endpoint enable
// EPDIS			->	endpoint disable
// SD1PID/SODDFRM	->	set data1 PID / set odd frame
// 	- interrupt/bulk endpoints	->	write DPID to DATA1
//	- isochronous endpoints		->	set even frame
// SD0PID/SEVNFRM	->	set data0 PID / set even frame
// 	- interrupt/bulk endpoints	->	write DPID to DATA0
//	- isochronous endpoints		->	set even frame
// SNAK				->	set NAK
// CNAK				->	clear NAK
// STALL			->	STALL handshake
// SNPM				->	snoop mode
// EPTYP			->	endpoint type
//  - 0b00: control
//  - 0b01: isochronous
//  - 0b10: bulk
//  - 0b11: interrupt
// NAKSTS		(r)	->	NAK status
// EONUM/DPID	(r)	->	even/odd fame / endpoint data PID
//	- isochronous endpoints		->	EONUM (0b0: even, 0b1: odd)
//  - interrupt/bulk endpoints	->	DPID (0b0: DATA0, 0b1: DATA1)
// USBAEP			->	USB active endpoint
// MPSIZ			->	maximum packet size
/// out[x]->DOEPTSIZx (x > 0)		TODO: [57.14.65		OTG device OUT endpoint x transfer size register]			@2701
// RXDPID/STUPCNT	->	received data PID / setup packet count
//  - PID: (0b00: DATA0, 0b01: DATA2, 0b10: DATA1, 0b11: MDATA)
// PKTCNT			->	packet count
// XFRSIZ			->	transfer size
/// (usb + 0xE00)=>PCGCCTL			TODO: [57.14.66		OTG power and clock gating control register]				@2702


// USB SOF pin?: https://community.st.com/t5/stm32-mcus-products/usb-sof-pin/td-p/433413


// HAL:
/* stm32h7xx_hal_pcd.c @174
	// CID?????
	if ((USBx->CID & (0x1U << 8)) == 0U) {
		hpcd->Init.dma_enable = 0U;
	}
*/

// usb.c (todos):
/*
void fconfig_USB_FS_device(USB_GPIO_t dp, USB_GPIO_t dn) {
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

	/* GPIO config *
	fconfig_GPIO(dp_port, dp_pin.num, GPIO_alt_func, GPIO_pull_up, GPIO_open_drain, GPIO_high_speed, dp_pin.alt);
	fconfig_GPIO(dn_port, dn_pin.num, GPIO_alt_func, GPIO_pull_up, GPIO_open_drain, GPIO_high_speed, dn_pin.alt);

	/* enable USB device clock and global interrupt *
	enable_id(dp_pin.id);

	IRQn_Type irqn = USB_to_IRQn(usb);
	set_IRQ_priority(irqn, 0);
	enable_IRQ(irqn);

	/* power config *
	PWR->CR3 |= (
		PWR_CR3_USBREGEN	|	// TODO: needed?
		PWR_CR3_USB33DEN	|
		PWR_CR3_SCUEN		|
		PWR_CR3_LDOEN
	);

	usb->GAHBCFG = 0x00000000UL;							// clear GAHBCFG config to disable interrupts during setup

	// set phy and device mode
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
		// TODO: battery charging detection (PWRDWN should then be off)
	);

	/* select device mode was placed here TODO check *
	//usb->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;				// force device device mode
	//while (usb->GINTSTS & USB_OTG_GINTSTS_CMOD);			// wait until device mode is set

	uint8_t i;
	for (i = 0UL; i < 0xFUL; i++) {
		usb->DIEPTXF[i] = 0x00000000UL;						// clear IN endpoint FIFO sizes/offsets
	}
	// usb->GCCFG |= USB_OTG_GCCFG_VBDEN;  // TODO check (VBDEN enable originally here!)
	*PCGCCTL = 0x00000000UL;								// restart PHY clock

	device->DCFG = (
		0b00UL << USB_OTG_DCFG_PFIVL_Pos			|		// 80% (TODO: try 90%)
		0b11UL << USB_OTG_DCFG_DSPD_Pos						// FS on internal PHY
	);

	flush_RX_FIFOS(usb);
	flush_RX_FIFO(usb);

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

	// TODO: code from reference code seems to not have any effect?????
	// device->DIEPMSK &= ~(USB_OTG_DIEPMSK_TXFURM);

	usb->GINTMSK = 0x00000000UL;							// mask all interrupts
	usb->GINTSTS = 0xBFFFFFFFUL;							// clear all interrupts TODO: (except for SRQINT??)
	usb->GINTMSK = (
		USB_OTG_GINTMSK_WUIM			|					// resume / remote wake-up detected interrupt
		USB_OTG_GINTMSK_SRQIM			|					// session request / new session detected interrupt
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
	);	// TODO: other interrupts??

	// disconnect
	*PCGCCTL &= ~(
		USB_OTG_PCGCCTL_STOPCLK			|
		USB_OTG_PCGCCTL_GATECLK
	);
	device->DCTL |= USB_OTG_DCTL_SDIS;

	// set device info
	USB2_OTG_FS->CID = 0x4D2E562EUL;								// "M.V."

	/* TODO: why is this not done in the HAL???
	device->DIEPMSK = (
		0b1UL << USB_OTG_DIEPMSK_XFRCM_Pos		// enable transfer completed interrupt
	);
	device->DOEPMSK = (
		0b1UL << USB_OTG_DIEPMSK_XFRCM_Pos		// enable transfer completed interrupt
	);
	device->DAINTMSK = (
		0b1UL << USB_OTG_DAINTMSK_OEPM_Pos		|	// endpoint 0 OUT
		0b1UL << USB_OTG_DAINTMSK_IEPM_Pos		|	// endpoint 0 IN
		0b1UL << (USB_OTG_DAINTMSK_OEPM_Pos + 1)	|	// endpoint 1 OUT
		0b1UL << (USB_OTG_DAINTMSK_IEPM_Pos + 1)		// endpoint 1 IN
	);
	*
}



void fconfig_USB_interface(USB_OTG_GlobalTypeDef* usb) {  // TODO: args
	config_USB_RX_FIFO(usb, 0x80);  // TODO: settings!!!
	config_USB_TX_FIFO(usb, 0, 0x40);
	config_USB_TX_FIFO(usb, 1, 0x80);
	/* if (((USBx->CID & (0x1U << 8)) == 0U) &&
		(hpcd->Init.battery_charging_enable == 1U))
	{
		/* Enable USB Transceiver
		USBx->GCCFG |= USB_OTG_GCCFG_PWRDWN;
	}*
	// TODO: keep track of configuration, interface classes and enpoint behavior
	// TODO: make state struct
	// TODO: make a general purpose class init funciton within USB.h/c
}
*/