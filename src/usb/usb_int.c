//
// Created by marijn on 2/11/24.
//

#include "usb.h"
#include "usb_t.h"  // TODO remove file and hide contents


/*!<
 * defines
* */
#define inline __attribute__((always_inline))

#define USB (handle->usb)
#define DEV (handle->device)
#define I_EP (iep->ep)
#define O_EP (oep->ep)


/*!<
 * register types
 * */
typedef struct {
	uint32_t EPNUM		: 4;	// endpoint number
	uint32_t BCNT		: 10;	// byte count
	uint32_t DPID		: 2;	// data PID
	uint32_t PKTSTS		: 4;	// packet status
	uint32_t FRMNUM		: 4;	// frame number
	uint32_t _0			: 2;	// reserved 0
	uint32_t STSPHST	: 1;	// status phase start
	uint32_t _1			: 4;	// reserved 1
} GRXSTSR_t;


/*!<
 * variables
 * */
extern USB_handle_t* USB_handle[2];


/*!<
 * imported functions
 * */
extern void flush_RX_FIFO(USB_OTG_GlobalTypeDef* usb);
extern void flush_TX_FIFO(USB_OTG_GlobalTypeDef* usb, uint8_t ep);
extern void flush_TX_FIFOS(USB_OTG_GlobalTypeDef* usb);


/*!<
 * static
* */
/*!< setup */
static inline void start_OEP0(USB_handle_t* handle, USB_OEP_t* oep) {
	// TODO: CORE VERISON?	[stm32h7xx_ll_usb.c] @2263
	if (!(O_EP->DOEPCTL & USB_OTG_DOEPCTL_EPENA)) {
		O_EP->DOEPTSIZ = (
			0x18UL									|	// expect 24 bytes to be received
			(0b1UL << USB_OTG_DOEPTSIZ_PKTCNT_Pos)	|	// set bit (will be reset when packet is received)
			(0x3UL << USB_OTG_DOEPTSIZ_STUPCNT_Pos)		// set max number of back to back packets to 3
		);
		// TODO: DMA?			[stm32h7xx_ll_usb.c] @1427
	}
}

/*!< usb IRQ logic */
static inline void USB_OTG_IRQ(USB_handle_t* handle) {
	uint32_t tmp = USB->GOTGINT;
	if (tmp & USB_OTG_GOTGINT_SEDET) {
		// HAL_PCD_DisconnectCallback (not needed for HID) TODO!!!!!!!!!!!!
	}
	USB->GOTGINT |= tmp;  // restore GOTGINT
}
static inline void USB_SOF_IRQ(USB_handle_t* handle) {
	// HAL_PCD_SOFCallback (not needed for HID) TODO!!!!!!!!!!!!!!!!!!!!!!!
	USB->GINTSTS = USB_OTG_GINTSTS_SOF;
}
static inline void USB_receive_packet_IRQ(USB_handle_t* handle) {
	USB_OEP_t*		oep;

	USB->GINTMSK &= ~USB_OTG_GINTMSK_RXFLVLM_Msk;	// note that RXFLVL is masked instead of cleared since that is not possible
	GRXSTSR_t				RX_status; *((uint32_t*)&RX_status) = USB->GRXSTSR;
	volatile uint32_t*		FIFO = (void*)(((uint32_t)USB) + USB_OTG_FIFO_BASE);
	oep = handle->OEP[RX_status.EPNUM];

	switch (RX_status.PKTSTS) {
		case 0b0010U:  // OUT data packet
			if (!RX_status.BCNT) { break; }
		const uint32_t	words = ((RX_status.BCNT + 3U) >> 2U);  // + 3UL to yield round up
		uint32_t		buffer = (uint32_t)oep->buffer;
		for (uint32_t i = 0UL; i < words; i++) {
			__UNALIGNED_UINT32_WRITE(oep->buffer, *FIFO);
			// 4x inc is faster than an add due to pipelining
			oep->buffer++; oep->buffer++;
			oep->buffer++; oep->buffer++;
		} oep->count += ((uint32_t)oep->buffer) - buffer;
		break;

		case 0b0110U:  // SETUP data packet
			// TODO: seperate SETUP data array needed??????
				__UNALIGNED_UINT32_WRITE(handle->setup, *FIFO);
		__UNALIGNED_UINT32_WRITE(&handle->setup[1], *FIFO);
		oep->count += 8UL;  // TODO: is BCNT also 8???
		break;

		default: break;  // TODO: other cases ????
	}
	USB->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM_Msk;	// unmask interrupt
}
static inline void USB_reset_IRQ(USB_handle_t* handle) {
	uint8_t			ep_num;
	USB_IEP_t*		iep;
	USB_OEP_t*		oep;

	DEV->DCTL &= ~USB_OTG_DCTL_RWUSIG;		// clear remote wake-up signaling TODO: needed?
	flush_TX_FIFOS(USB);

	for (ep_num = 0U; ep_num < USB_OTG_ENDPOINT_COUNT; ep_num++) {
		iep = handle->IEP[ep_num]; oep = handle->OEP[ep_num];
		I_EP->DIEPINT = O_EP->DOEPINT = 0xFB7FU;
		I_EP->DIEPCTL &= ~USB_OTG_DIEPCTL_STALL;	// TODO: verify: datasheet says that it can only be set
		O_EP->DOEPCTL &= ~USB_OTG_DOEPCTL_STALL;	// TODO: verify: datasheet says that it can only be set
		O_EP->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
	}
	DEV->DAINTMSK |= (
		0b1UL << 16	|	// enable OEP0 interrupts
		0b1UL			// enable IEP0 interrupts
	);
	// TODO: dedicated EP1?		[stm32h7xx_hal_pcd.c] @1352
	DEV->DOEPMSK |= (
		USB_OTG_DOEPMSK_STUPM		|
		USB_OTG_DOEPMSK_XFRCM		|
		USB_OTG_DOEPMSK_EPDM		|
		USB_OTG_DOEPMSK_OTEPSPRM	|
		USB_OTG_DOEPMSK_NAKM
	);
	DEV->DIEPMSK |= (
		USB_OTG_DIEPMSK_TOM			|
		USB_OTG_DIEPMSK_XFRCM		|
		USB_OTG_DIEPMSK_EPDM
	);
	DEV->DCFG &= ~USB_OTG_DCFG_DAD;				// reset DEV address

	start_OEP0(handle, *handle->OEP);
	USB->GINTSTS = USB_OTG_GINTSTS_USBRST;
}
static inline void USB_enumeration_done_IRQ(USB_handle_t* handle, USB_IEP_t* iep) {
	I_EP->DIEPCTL &= ~USB_OTG_DIEPCTL_MPSIZ;	// reset IEP0 maximum packet size
	DEV->DCTL |= USB_OTG_DCTL_CGINAK;			// clear global IN NAK
	// TODO: HS?				[stm32h7xx_ll_usb.c] @205
	USB->GUSBCFG &= ~USB_OTG_GUSBCFG_TRDT;  // reset turnaround time
	if		(AHB_clock_frequency < 15000000UL)	{ USB->GUSBCFG |= 0xFU << USB_OTG_GUSBCFG_TRDT_Pos; }
	else if	(AHB_clock_frequency < 16000000UL)	{ USB->GUSBCFG |= 0xFU << USB_OTG_GUSBCFG_TRDT_Pos; }
	else if	(AHB_clock_frequency < 17200000UL)	{ USB->GUSBCFG |= 0xFU << USB_OTG_GUSBCFG_TRDT_Pos; }
	else if	(AHB_clock_frequency < 18500000UL)	{ USB->GUSBCFG |= 0xFU << USB_OTG_GUSBCFG_TRDT_Pos; }
	else if	(AHB_clock_frequency < 20000000UL)	{ USB->GUSBCFG |= 0xFU << USB_OTG_GUSBCFG_TRDT_Pos; }
	else if	(AHB_clock_frequency < 21800000UL)	{ USB->GUSBCFG |= 0xFU << USB_OTG_GUSBCFG_TRDT_Pos; }
	else if	(AHB_clock_frequency < 24000000UL)	{ USB->GUSBCFG |= 0xFU << USB_OTG_GUSBCFG_TRDT_Pos; }
	else if	(AHB_clock_frequency < 27700000UL)	{ USB->GUSBCFG |= 0xFU << USB_OTG_GUSBCFG_TRDT_Pos; }
	else if	(AHB_clock_frequency < 32000000UL)	{ USB->GUSBCFG |= 0xFU << USB_OTG_GUSBCFG_TRDT_Pos; }
	else										{ USB->GUSBCFG |= 0xFU << USB_OTG_GUSBCFG_TRDT_Pos; }
	// HAL_PCD_ResetCallback (not needed for HID) TODO!!!!!!!!!!!!!!!!!!!!!
	USB->GINTSTS = USB_OTG_GINTSTS_ENUMDNE;
}
static inline void USB_connection_IRQ(USB_handle_t* handle) {
	// HAL_PCD_ConnectCallback (not needed for HID) TODO!!!!!!!!!!!!!!!!!!!
	USB->GINTSTS = USB_OTG_GINTSTS_SRQINT;
}

/*!< IEP IRQ logic */
static inline void IEP_transfer_complete_IRQ(USB_handle_t* handle, USB_IEP_t* iep, uint8_t ep_num) {
	DEV->DIEPEMPMSK &= ~(0b1UL << ep_num);	// mask interrupt
	I_EP->DIEPINT |= USB_OTG_DIEPINT_XFRC;
	// TODO: DMA?				[stm32h7xx_hal_pcd.c] @1209
	if (
		ep_num && iep->class->data_in &&		// make sure the class has a data_in function
		handle->state == USB_CONFIGURED		// make sure the device is configured
	) {
		iep->class->data_in(handle->usb, ep_num);
	}
	// USBD_LL_DataOutStage  // TODO: implement fully... also do this for IEP etc
}
static inline void IEP_disabled_IRQ(USB_handle_t* handle, USB_IEP_t* iep, uint8_t ep_num) {
	flush_TX_FIFO(handle->usb, ep_num);
	// TODO: ISO?				[stm32h7xx_hal_pcd.c] @1245
	I_EP->DIEPINT |= USB_OTG_DIEPINT_EPDISD;
}
static inline void IEP_timeout_IRQ(USB_IEP_t* iep) {
	I_EP->DIEPINT |= USB_OTG_DIEPINT_TOC;
}
static inline void IEP_ITTXFE_IRQ(USB_IEP_t* iep) {
	I_EP->DIEPINT |= USB_OTG_DIEPINT_ITTXFE;
}
static inline void IEP_NAK_effective_IRQ(USB_IEP_t* iep) {
	I_EP->DIEPINT |= USB_OTG_DIEPINT_INEPNE;
}
static inline void IEP_FIFO_empty_IRQ(USB_handle_t* handle, USB_IEP_t* iep, uint8_t ep_num) {
	if (iep->count <= iep->size) {
		uint32_t	len, len32, i;
		uint32_t	buffer = (uint32_t)iep->buffer;
		uint8_t		*src, *FIFO = (uint8_t*)((uint32_t)USB + (0x1000UL * (ep_num + 1U)));
		while (iep->size && iep->count < iep->size) {
			len =	iep->size - iep->count;
			len32 =	((len > iep->packet_size ? iep->packet_size : len) + 3U) >> 2U;
			if ((I_EP->DTXFSTS & 0xFFFFUL) < len32) { break; }
			src =	iep->buffer;
			for (i = 0UL; i < len32; i++) {
				*FIFO = __UNALIGNED_UINT32_READ(src);
				src++; src++; src++; src++;  // 4x inc is faster than an add due to pipelining
			} iep->buffer +=	len;
		} iep->count +=			((uint32_t)iep->buffer) - buffer;
		if (iep->size <= iep->count) {
			// mask TX FIFO empty interrupt after all data is sent
			DEV->DIEPEMPMSK &= ~(0b1UL << ep_num);
		}
	}
}

/*!< OEP IRQ logic */
static inline void OEP_transfer_complete(USB_handle_t* handle, USB_OEP_t* oep, uint8_t ep_num) {
	O_EP->DOEPINT |= USB_OTG_DOEPINT_XFRC;
	// TODO: DMA?				[stm32h7xx_hal_pcd.c] @2217
	// TODO: CORE VERISON?		[stm32h7xx_hal_pcd.c] @2274
	if (!ep_num) {
		if (!oep->size) { start_OEP0(handle, oep); }
		// TODO: @326 [usbd_core.c]
		if (handle->EP0_state == EP_DATA_OUT) {
			// TODO: CONTINUE HERE!!!!!!!!!!!!!!!!!!!!!!!<<<<<<<<<<<<<<<<<<<<<<<<<
			if (oep->size > oep->packet_size) {

			}
			else {

			}
		}
	}
	// TODO: @326 [usbd_core.c]
}
static inline void OEP_disabled(USB_handle_t* handle, USB_OEP_t* oep) {
	if (handle->usb->GINTSTS & USB_OTG_GINTSTS_BOUTNAKEFF) {
		DEV->DCTL |= USB_OTG_DCTL_CGONAK;
	}
	// TODO: ISO?				[stm32h7xx_hal_pcd.c] @1158
	O_EP->DOEPINT |= USB_OTG_DOEPINT_EPDISD;
}
static inline void OEP_setup_done(USB_OEP_t* oep) {
	O_EP->DOEPINT |= USB_OTG_DOEPINT_STUP;
	// TODO: CORE VERISON/DMA?	[stm32h7xx_hal_pcd.c] @2317
	// HAL_PCD_SetupStageCallback (not needed for HID) TODO!!!!!!!!!!!!!!!!
	// TODO: CORE VERISON/DMA?	[stm32h7xx_hal_pcd.c] @2330
}
static inline void OEP_OTEPDIS(USB_OEP_t* oep) {
	O_EP->DOEPINT |= USB_OTG_DOEPINT_OTEPDIS;
}
static inline void OEP_OTEPSPR(USB_OEP_t* oep) {
	O_EP->DOEPINT |= USB_OTG_DOEPINT_OTEPSPR;
}
static inline void OEP_NAK(USB_OEP_t* oep) {
	O_EP->DOEPINT |= USB_OTG_DOEPINT_NAK;
}

/*!< IRQ handleing */
static inline void IEP_common_handler(USB_handle_t* handle, uint8_t ep_num) {
	USB_IEP_t	*iep =		handle->IEP[ep_num];

	uint32_t	ep_int =	(
		I_EP->DIEPINT & DEV->DIEPMSK &										// get all triggered interrupts
		((DEV->DIEPEMPMSK >> ep_num) & 0b1UL) << USB_OTG_DIEPINT_TXFE_Pos	// include TXFE interrupt if enabled
	);

	/* transfer complete interrupt */
	if (ep_int & USB_OTG_DIEPINT_XFRC)					{ IEP_transfer_complete_IRQ(handle, iep, ep_num); }
	/* endpoint disabled interrupt */
	if (ep_int & USB_OTG_DIEPINT_EPDISD)				{ IEP_disabled_IRQ(handle, iep, ep_num); }
	/* AHB error interrupt */							// TODO: DMA?
	I_EP->DIEPINT = USB_OTG_DIEPINT_AHBERR;				// clear AHB error interrupt
	/* timeout condition interrupt */
	if (ep_int & USB_OTG_DIEPINT_TOC)					{ IEP_timeout_IRQ(iep); }
	/* IN token received when Tx FIFO is empty interrupt */
	if (ep_int & USB_OTG_DIEPINT_ITTXFE)				{ IEP_ITTXFE_IRQ(iep); }
	/* IN token recieved with EP mismatch interrupt */
	I_EP->DIEPINT = USB_OTG_DIEPINT_INEPNM;				// clear IN token EP mismatch interrupt
	/* IN enpoint NAK effective interrupt */
	if (ep_int & USB_OTG_DIEPINT_INEPNE)				{ IEP_NAK_effective_IRQ(iep); }
	/* TX FIFO empty interrupt */
	if (ep_int & USB_OTG_DIEPINT_TXFE)					{ IEP_FIFO_empty_IRQ(handle, iep, ep_num); }
	/* TX FIFO underrun interrupt */
	I_EP->DIEPINT = USB_OTG_DIEPINT_TXFIFOUDRN;			// clear TX FIFO underrun interrupt
	/* buffer not available interrupt */				// TODO: DMA?
	I_EP->DIEPINT = USB_OTG_DIEPINT_BNA;				// clear buffer not available interrupt
	/* packet dropped interrupt */						// TODO: ISO?
	I_EP->DIEPINT = USB_OTG_DIEPINT_PKTDRPSTS;			// clear packet dropped interrupt
	/* NAK interrupt */
	I_EP->DIEPINT = USB_OTG_DIEPINT_NAK;				// clear NAK interrupt
}
static inline void OEP_common_handler(USB_handle_t* handle, uint8_t ep_num) {
	USB_OEP_t	*oep =		handle->OEP[ep_num];

	uint32_t	ep_int =	O_EP->DOEPINT & DEV->DOEPMSK;	// get all triggered interrupts

	/* transfer complete interrupt */
	if (ep_int & USB_OTG_DOEPINT_XFRC)					{ OEP_transfer_complete(handle, oep, ep_num); }
	/* endpoint disabled interrupt */
	if (ep_int & USB_OTG_DOEPINT_EPDISD)				{ OEP_disabled(handle, oep); }
	/* AHB error interrupt */							// TODO: DMA?
	O_EP->DOEPINT = USB_OTG_DOEPINT_AHBERR;				// clear AHB error interrupt
	/* SETUP phase done */
	if (ep_int & USB_OTG_DOEPINT_STUP)					{ OEP_setup_done(oep); }
	/* OUT token received when endpoint disabled interrupt */
	if (ep_int & USB_OTG_DOEPINT_OTEPDIS)				{ OEP_OTEPDIS(oep); }
	/* status phase received for control write */
	if (ep_int & USB_OTG_DOEPINT_OTEPSPR)				{ OEP_OTEPSPR(oep); }
	/* back to back setup packet recived interrupt */
	O_EP->DOEPINT = USB_OTG_DOEPINT_B2BSTUP;			// clear back to back setup packet received interrupt
	/* OUT packet error interrupt */
	O_EP->DOEPINT = USB_OTG_DOEPINT_OUTPKTERR;			// clear OUT packet error interrupt
	/* buffer not available interrupt */				// TODO: DMA?
	O_EP->DOEPINT = USB_OTG_DIEPINT_BNA;				// clear buffer not available interrupt
	/* babble error interrupt */
	O_EP->DOEPINT = USB_OTG_DOEPINT_BERR;				// clear babble error interrupt
	/* NAK interrupt */
	if (ep_int & USB_OTG_DOEPINT_NAK)					{ OEP_NAK(oep); }
	/* NYET interrupt */								// TODO: HS?
	O_EP->DOEPINT = USB_OTG_DOEPINT_NYET;				// clear NYET interrupt
	/* setup packet received interrupt */				// TODO: DMA?
	O_EP->DOEPINT = USB_OTG_DOEPINT_STPKTRX;			// clear setup packet received interrupt
}
static inline void USB_common_handler(USB_handle_t* handle) {
	const uint32_t	irqs = USB->GINTSTS & USB->GINTMSK;
	if (!irqs) {  // TODO why was this interrupt triggered if no unmasked interrupts are triggered??? NEEDED??
		USB->GINTSTS = USB->GINTSTS;  // clear the masked interrupts
		return;
	}
	if ((USB->GINTSTS) & 0b1UL) { return; }				// exit in host mode (not implemented)

	uint8_t			ep_num;
	uint16_t		ep_gint;

	/* mode mismatch interrupt */
	USB->GINTSTS = USB_OTG_GINTSTS_MMIS;				// clear mode mismatch interrupt
	/* OTG interrupt */
	if (irqs & USB_OTG_GINTSTS_OTGINT)					{ USB_OTG_IRQ(handle); }
	/* start of frame interrupt */
	if (irqs & USB_OTG_GINTSTS_SOF)						{ USB_SOF_IRQ(handle); }
	/* receive packet interrupt */
	if (irqs & USB_OTG_GINTSTS_RXFLVL)					{ USB_receive_packet_IRQ(handle); }
	/* global IN non-periodic NAK effective interrupt */
	USB->GINTSTS = USB_OTG_GINTSTS_GINAKEFF;			// clear global IN non-periodic NAK effective interrupt
	/* global OUT NAK effective interrupt */
	USB->GINTSTS = USB_OTG_GINTSTS_BOUTNAKEFF;			// clear global OUT NAK effective interrupt
	/* early suspend interrupt */						// TODO: LPM?
	USB->GINTSTS = USB_OTG_GINTSTS_ESUSP;				// clear early suspend interrupt
	/* suspend interrupt */								// TODO: LPM?
	USB->GINTSTS = USB_OTG_GINTSTS_USBSUSP;				// clear suspend interrupt
	/* reset interrupt */
	if (irqs & USB_OTG_GINTSTS_USBRST)					{ USB_reset_IRQ(handle); }
	/* enumeration done interrupt */
	if (irqs & USB_OTG_GINTSTS_ENUMDNE)					{ USB_enumeration_done_IRQ(handle, *handle->IEP); }
	/* isochronous OUT packet dropped interrupt */		// TODO: ISO?
	USB->GINTSTS = USB_OTG_GINTSTS_ISOODRP;				// clear isochronous OUT packet dropped interrupt
	/* end of periodic frame interrupt */
	USB->GINTSTS = USB_OTG_GINTSTS_EOPF;				// clear end of periodic frame interrupt
	/* IN endpoint interrupts */
	if (irqs & USB_OTG_GINTSTS_IEPINT) {
		ep_num = 0U;
		ep_gint = DEV->DAINT & DEV->DAINTMSK & 0xFFFFU;	// get interrupt bits for each IN endpoint
		while (ep_gint) {
			if (ep_gint & 0b1UL) { IEP_common_handler(handle, ep_num); }
			ep_num++; ep_gint >>= 1UL;
		}
	}
	/* OUT endpoint interrupts */
	if (irqs & USB_OTG_GINTSTS_OEPINT) {
		ep_num = 0U;
		ep_gint = (DEV->DAINT & DEV->DAINTMSK) >> 0x10UL;	// get interrupt bits for each OUT endpoint
		while (ep_gint) {
			if (ep_gint & 0b1UL) { OEP_common_handler(handle, ep_num); }
			ep_num++; ep_gint >>= 1UL;
		}
	}
	/* incomplete isochronous IN interrupt */			// TODO: ISO?
	USB->GINTSTS = USB_OTG_GINTSTS_IISOIXFR;			// clear incomplete isochronous IN interrupt
	/* incomplete isochronous OUT interrupt */			// TODO: ISO?
	USB->GINTSTS = USB_OTG_GINTSTS_PXFR_INCOMPISOOUT;	// clear incomplete isochronous OUT interrupt
	/* data fetch suspend interrupt */					// TODO: DMA?
	USB->GINTSTS = USB_OTG_GINTSTS_DATAFSUSP;			// clear data fetch suspend interrupt
	/* reset detected interrupt */
	USB->GINTSTS = USB_OTG_GINTSTS_RSTDET;				// clear reset detected interrupt
	/* LPM interrupt */									// TODO: LPM?
	USB->GINTSTS = USB_OTG_GINTSTS_LPMINT;				// clear LPM interrupt
	/* connector ID status change interrupt */
	USB->GINTSTS = USB_OTG_GINTSTS_CIDSCHG;				// clear connector ID status change interrupt
	/* connection event interrupt */
	if (irqs & USB_OTG_GINTSTS_SRQINT)					{ USB_connection_IRQ(handle); }
	/* wake-up interrupt */								// TODO: LPM?
	USB->GINTSTS = USB_OTG_GINTSTS_WKUINT;				// clear wake-up interrupt
}


/*!<
 * interrupts
 * */
extern void OTG_HS_IRQHandler(void) { USB_common_handler(USB_handle[0]); }
extern void OTG_FS_IRQHandler(void) { USB_common_handler(USB_handle[1]); }



//const uint16_t	frame =	(DEV->DSTS >> USB_OTG_DSTS_FNSOF_Pos) & 0x3FFFUL;

/// NOTES
// from OTG_common_iep_handler -> XFRC
// TODO: EP0!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// TODO: EP0 should never have a XFRC interrupt because EP0_IN is only used as a response to a setup token not an IN token
// TODO: it the code for it in USBD_LL_DataInStage will be called from USBD_CtlSendData which will eventually be called from PCD_EP_OutSetupPacket_int
