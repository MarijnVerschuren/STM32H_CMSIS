//
// Created by marijn on 2/11/24.
//

#include "usb.h"
#include "usb_t.h"  // TODO remove file and hide contents


/*!<
 * defines
* */
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
static inline void start_OEP0(USB_handle_t* handle) {
	USB_OEP_t	*oep =		*handle->OEP;
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

/*!< IRQ handleing */
static inline void OTG_common_iep_handler(USB_handle_t* handle, uint8_t ep_num) {
	USB_IEP_t	*iep =		handle->IEP[ep_num];

	uint32_t	ep_int =	(
		I_EP->DIEPINT & DEV->DOEPMSK &											// get all triggered interrupts
		((DEV->DIEPEMPMSK >> ep_num) & 0b1UL) << USB_OTG_DIEPINT_TXFE_Pos	// include TXFE interrupt if enabled
	);

	/* transfer complete interrupt */
	if (ep_int & USB_OTG_DIEPINT_XFRC) {
		DEV->DIEPEMPMSK &= ~(0b1UL << ep_num);	// mask interrupt
		I_EP->DIEPINT |= USB_OTG_DIEPINT_XFRC;
		// TODO: DMA?				[stm32h7xx_hal_pcd.c] @1209
		if (
			ep_num && iep->class->data_in &&		// make sure the class has a data_in function
			handle->state == USB_configured		// make sure the device is configured
		) {
			iep->class->data_in(handle->usb, ep_num);
		}
		// USBD_LL_DataOutStage  // TODO: implement fully... also do this for IEP etc
	}

	/* endpoint disabled interrupt */
	if (ep_int & USB_OTG_DIEPINT_EPDISD) {
		flush_TX_FIFO(handle->usb, ep_num);
		// TODO: ISO?				[stm32h7xx_hal_pcd.c] @1245
		I_EP->DIEPINT |= USB_OTG_DIEPINT_EPDISD;
	}

	/* timeout condition interrupt */
	if (ep_int & USB_OTG_DIEPINT_TOC) {
		I_EP->DIEPINT |= USB_OTG_DIEPINT_TOC;
	}

	/* IN token received when Tx FIFO is empty interrupt */
	if (ep_int & USB_OTG_DIEPINT_ITTXFE) {
		I_EP->DIEPINT |= USB_OTG_DIEPINT_ITTXFE;
	}

	/* IN enpoint NAK effective interrupt */
	if (ep_int & USB_OTG_DIEPINT_INEPNE) {
		I_EP->DIEPINT |= USB_OTG_DIEPINT_INEPNE;
	}

	/* transmit FIFO empty interrupt */
	if (ep_int & USB_OTG_DIEPINT_TXFE) {
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
}

static inline void OTG_common_oep_handler(USB_handle_t* handle, uint8_t ep_num) {
	USB_OEP_t	*oep =		handle->OEP[ep_num];

	uint32_t	ep_int =	O_EP->DOEPINT & DEV->DOEPMSK;					// get all triggered interrupts

	/* transfer complete interrupt */
	if (ep_int & USB_OTG_DOEPINT_XFRC) {
		O_EP->DOEPINT |= USB_OTG_DOEPINT_XFRC;
		// TODO: DMA?				[stm32h7xx_hal_pcd.c] @2217
		// TODO: CORE VERISON?		[stm32h7xx_hal_pcd.c] @2274
		if (!ep_num) {
			if (!oep->size) { start_OEP0(handle); }
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

	/* endpoint disabled interrupt */
	if (ep_int & USB_OTG_DOEPINT_EPDISD) {
		if (handle->usb->GINTSTS & USB_OTG_GINTSTS_BOUTNAKEFF) {
			DEV->DCTL |= USB_OTG_DCTL_CGONAK;
		}
		// TODO: ISO?				[stm32h7xx_hal_pcd.c] @1158
		O_EP->DOEPINT |= USB_OTG_DOEPINT_EPDISD;
	}

	/* SETUP phase done */
	if (ep_int & USB_OTG_DOEPINT_STUP) {
		O_EP->DOEPINT |= USB_OTG_DOEPINT_STUP;
		// TODO: CORE VERISON/DMA?	[stm32h7xx_hal_pcd.c] @2317
		// HAL_PCD_SetupStageCallback (not needed for HID) TODO!!!!!!!!!!!!!!!!
		// TODO: CORE VERISON/DMA?	[stm32h7xx_hal_pcd.c] @2330
	}

	/* OUT token received when endpoint disabled interrupt */
	if (ep_int & USB_OTG_DOEPINT_OTEPDIS) {
		O_EP->DOEPINT |= USB_OTG_DOEPINT_OTEPDIS;
	}

	/* status phase received for control write */
	if (ep_int & USB_OTG_DOEPINT_OTEPSPR) {
		O_EP->DOEPINT |= USB_OTG_DOEPINT_OTEPSPR;
	}

	/* NAK interrupt */
	if (ep_int & USB_OTG_DOEPINT_NAK) {
		O_EP->DOEPINT |= USB_OTG_DOEPINT_NAK;
	}
}

static inline void OTG_common_handler(USB_handle_t* handle) {
	if ((USB->GINTSTS) & 0b1UL) { return; }		// exit in host mode  TODO: host?

	/* clear unneeded interrupts */
	USB->GINTSTS = USB_OTG_GINTSTS_MMIS;		// clear mode mismatch (it does not have any effect)

	const uint32_t	irqs = USB->GINTSTS & USB->GINTMSK;
	if (!irqs) { return; }  // TODO why was this interrupt triggered if no unmasked interrupts are triggered??? NEEDED??

	//const uint16_t	frame =	(DEV->DSTS >> USB_OTG_DSTS_FNSOF_Pos) & 0x3FFFUL;
	uint8_t			ep_num;
	uint16_t		ep_gint;
	USB_IEP_t*		iep;
	USB_OEP_t*		oep;


	// TODO: verify (HAL_PCD_IRQHandler @1085-1115)

	/* receive packet interrupt */
	if (irqs & USB_OTG_GINTSTS_RXFLVL) {
		USB->GINTMSK &= ~USB_OTG_GINTMSK_RXFLVLM_Msk;	// note that RXFLVL is masked instead of cleared since that is not possible
		const GRXSTSR_t			RX_status; *((uint32_t*)&RX_status) = USB->GRXSTSR;
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

			// TODO: other cases ????
		}
		USB->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM_Msk;	// unmask interrupt
	}

	/* IN endpoints interrupt */
	if (USB->GINTSTS & USB_OTG_GINTSTS_IEPINT) {
		ep_num = 0U;
		ep_gint = DEV->DAINT & DEV->DAINTMSK & 0xFFFFU;	// get interrupt bits for each IN endpoint
		while (ep_gint) {
			if (ep_gint & 0b1UL) { OTG_common_iep_handler(handle, ep_num); }
			ep_num++; ep_gint >>= 1UL;
		}
	}

	/* OUT endpoints interrupt */
	if (USB->GINTSTS & USB_OTG_GINTSTS_OEPINT) {
		ep_num = 0U;
		ep_gint = (DEV->DAINT & DEV->DAINTMSK) >> 0x10UL;	// get interrupt bits for each OUT endpoint
		while (ep_gint) {
			if (ep_gint & 0b1UL) { OTG_common_oep_handler(handle, ep_num); }
			ep_num++; ep_gint >>= 1UL;
		}
	}

	/* reset interrupt */
	if (USB->GINTSTS & USB_OTG_GINTSTS_USBRST) {
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

		start_OEP0(handle);
		USB->GINTSTS |= USB_OTG_GINTSTS_USBRST;
	}

	/* enumeration done interrupt */
	if (USB->GINTSTS & USB_OTG_GINTSTS_ENUMDNE) {
		iep = handle->IEP[0];

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
		USB->GINTSTS |= USB_OTG_GINTSTS_ENUMDNE;
	}

	/* start of frame interrupt */
	if (USB->GINTSTS & USB_OTG_GINTSTS_SOF) {
		// HAL_PCD_SOFCallback (not needed for HID) TODO!!!!!!!!!!!!!!!!!!!!!!!
		USB->GINTSTS |= USB_OTG_GINTSTS_SOF;
	}

	/* global OUT NAK effective interrupt */
	if (USB->GINTSTS & USB_OTG_GINTSTS_BOUTNAKEFF) {
		USB->GINTMSK &= ~USB_OTG_GINTMSK_GONAKEFFM;  // mask global OUT NAK effective interrupt
		// TODO: ISO?			[stm32h7xx_hal_pcd.c] @1422
	}

	/* connection event interrupt */
	if (USB->GINTSTS & USB_OTG_GINTSTS_SRQINT) {
		// HAL_PCD_ConnectCallback (not needed for HID) TODO!!!!!!!!!!!!!!!!!!!
		USB->GINTSTS |= USB_OTG_GINTSTS_SRQINT;
	}

	/* disconnection event interrupt */
	if (USB->GINTSTS & USB_OTG_GINTSTS_OTGINT) {
		uint32_t tmp = USB->GOTGINT;
		if (tmp & USB_OTG_GOTGINT_SEDET) {
			// HAL_PCD_DisconnectCallback (not needed for HID) TODO!!!!!!!!!!!!
		}
		USB->GOTGINT |= tmp;  // restore GOTGINT
	}

	// TODO: other interrupts??

	// TODO: (HAL_PCD_IRQHandler @1085-1115)
	/* TODO: ISO?				[stm32h7xx_hal_pcd.c]
	/* incomplete isochronous IN interrupt * /
	if (USB->GINTSTS & USB_OTG_GINTSTS_IISOIXFR) {
		// TODO: @1433
	}
	/* incomplete isochronous OUT interrupt * /
	if (USB->GINTSTS & USB_OTG_GINTSTS_PXFR_INCOMPISOOUT) {
		// TODO: @1453
	}
	*/

	/* TODO: LPM?				[stm32h7xx_hal_pcd.c]
	/* suspend interrupt * /
	if (USB->GINTSTS & USB_OTG_GINTSTS_USBSUSP) {
		// TODO: @1297
	}
	/* wake-up interrupt * /
	if (USB->GINTSTS & USB_OTG_GINTSTS_WKUINT) {
		// TODO: @1269
	}
	/* LPM interrupt * /
	if (USB->GINTSTS & USB_OTG_GINTSTS_LPMINT) {
		// TODO: @1311
	}
	*/
}


/*!<
 * interrupts
 * */
extern void OTG_HS_IRQHandler(void) { OTG_common_handler(USB_handle[0]); }
extern void OTG_FS_IRQHandler(void) { OTG_common_handler(USB_handle[1]); }



/// NOTES
// from OTG_common_iep_handler -> XFRC
// TODO: EP0!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// TODO: EP0 should never have a XFRC interrupt because EP0_IN is only used as a response to a setup token not an IN token
// TODO: it the code for it in USBD_LL_DataInStage will be called from USBD_CtlSendData which will eventually be called from PCD_EP_OutSetupPacket_int
