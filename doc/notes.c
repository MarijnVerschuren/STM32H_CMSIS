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
// TSDPS		->	data line pulsing using (0: utmi_txvalid, 1: utmi_termsel)
// ULPIEVBUSI	->	PHY uses an exernal Vbus valid comparator
// ULPIEVBUSD	->	PHY drives Vbus using external supply								| TODO
// ULPICSM		->	PHY does (1: not) power down internal clock when suspended
// ULPAIR		->	enable auto resume on PHY											| ??
// ULPIFSLS		->	ULPI (1: FS/LS serial) interface
// PHYLPC		->	(0: 480 MHz internal PLL clock, 1: 48 MHz external clock)			| TODO!!!
// TRDT			->	USB turnaround time TODO: configure according to [Table 519]
// HNPCAP		->	HNP capability enabled
// SRPCAP		->	SRP capability enabled
// PHYSEL		->	(0: USB 2.0 external ULPI high-speed PHY, 1: 1.1 full-speed serial)	|
// TOCAL		->	FS timeout calibration												| TODO

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
// DPID		(r)	->	data PID		(00: DATA0, 10: DATA1, 01: DATA2, 11: MDATA)
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
// DPID		(r)	->	data PID		(00: DATA0, 10: DATA1, 01: DATA2, 11: MDATA)
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
/// device->DVBUSPULSE				TODO: [57.14.47		OTG device VBUS pulsing time register]						@2681
/// device->DTHRCTL					TODO: [57.14.48		OTG device threshold control register]						@2682
/// device->DIEPEMPMSK				TODO: [57.14.49		OTG device IN endpoint FIFO empty interrupt mask register]	@2683
/// device->DEACHINT				TODO: [57.14.50		OTG device each endpoint interrupt register]				@2683
/// device->DEACHINTMSK				TODO: [57.14.51		OTG device each endpoint interrupt mask register]			@2684
/// device->HS_DIEPEACHMSK1			TODO: [57.14.52		OTG device each IN endpoint-1 interrupt mask register]		@2684
/// device->HS_DOEPEACHMSK1			TODO: [57.14.53		OTG device each OUT endpoint-1 interrupt mask register]		@2685
/// device->DIEPCTLx (x: 0 to 8)	TODO: [57.14.54		OTG device IN endpoint x control register]					@2687
/// device->DIEPINTx (x: 0 to 8)	TODO: [57.14.55		OTG device IN endpoint x interrupt register]				@2689
/// device->DIEPTSIZ0				TODO: [57.14.56		OTG device IN endpoint 0 transfer size register]			@2691
/// device->DIEPDMAx (x: 0 to 8)	TODO: [57.14.57		OTG device IN endpoint x DMA address register]				@2691
/// device->DTXFSTSx (x: 0 to 8)	TODO: [57.14.58		OTG device IN endpoint transmit FIFO status register]		@2692
/// device->DIEPTSIZx (x: 1 to 8)	TODO: [57.14.59		OTG device IN endpoint x transfer size register]			@2692
/// device->DOEPCTL0				TODO: [57.14.60		OTG device control OUT endpoint 0 control register]			@2693
/// device->DOEPINTx (x: 0 to 8)	TODO: [57.14.61		OTG device OUT endpoint x interrupt register]				@2695
/// device->DOEPTSIZ0				TODO: [57.14.62		OTG device OUT endpoint 0 transfer size register]			@2697
/// device->DOEPDMAx (x: 0 to 8)	TODO: [57.14.63		OTG device OUT endpoint x DMA address register]				@2698
/// device->DOEPCTLx (x: 1 to 8)	TODO: [57.14.64		OTG device OUT endpoint x control register]					@2698
/// device->DOEPTSIZx (x: 1 to 8)	TODO: [57.14.65		OTG device OUT endpoint x transfer size register]			@2701
/// device->PCGCCTL					TODO: [57.14.66		OTG power and clock gating control register]				@2702
