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
// TODO: setup code will probably need its own functions

/*!< IRQ handleing */
static inline void OTG_common_iep_handler(USB_handle_t* handle, uint8_t ep_num) {
	USB_OTG_DeviceTypeDef		*device =	(void*)((uint32_t)handle + USB_OTG_DEVICE_BASE);
	USB_IEP_t					*iep	=	handle->IEP[ep_num];

	uint32_t ep_int =			(
		I_EP->DIEPINT & device->DOEPMSK &											// get all triggered interrupts
		((device->DIEPEMPMSK >> ep_num) & 0b1UL) << USB_OTG_DIEPINT_TXFE_Pos	// include TXFE interrupt if enabled
	);

	/* transfer complete interrupt */
	if (ep_int & USB_OTG_DIEPINT_XFRC) {
		device->DIEPEMPMSK &= ~(0b1UL << ep_num);	// mask interrupt
		I_EP->DIEPINT |= USB_OTG_DIEPINT_XFRC;
		// TODO: DMA? @1209
		if (
			ep_num && iep->class->data_in &&		// make sure the class has a data_in function
			handle->state == USB_configured		// make sure the device is configured
		) {
			iep->class->data_in(handle->usb, ep_num);
		}
	}

	/* endpoint disabled interrupt */
	if (ep_int & USB_OTG_DIEPINT_EPDISD) {
		flush_TX_FIFO(handle->usb, ep_num);
		// TODO: ISO? @1245
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
		// TODO: @1260
		// PCD_WriteEmptyTxFifo TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}
}

static inline void OTG_common_oep_handler(USB_handle_t* handle, uint8_t ep_num) {
	USB_OTG_DeviceTypeDef		*device =	(void*)((uint32_t)handle->usb + USB_OTG_DEVICE_BASE);
	USB_OEP_t					*oep	=	handle->OEP[ep_num];

	uint32_t ep_int =			O_EP->DOEPINT & device->DOEPMSK;					// get all triggered interrupts

	/* transfer complete interrupt */
	if (ep_int & USB_OTG_DOEPINT_XFRC) {
		O_EP->DOEPINT |= USB_OTG_DOEPINT_XFRC;
		// TODO: @1133
		// PCD_EP_OutXfrComplete_int TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	/* endpoint disabled interrupt */
	if (ep_int & USB_OTG_DOEPINT_EPDISD) {
		if (handle->usb->GINTSTS & USB_OTG_GINTSTS_BOUTNAKEFF) {
			device->DCTL |= USB_OTG_DCTL_CGONAK;
		}
		// TODO: ISO? @1158
		O_EP->DOEPINT |= USB_OTG_DOEPINT_EPDISD;
	}

	/* SETUP phase done */
	if (ep_int & USB_OTG_DOEPINT_STUP) {
		O_EP->DOEPINT |= USB_OTG_DOEPINT_STUP;
		// TODO: @1140
		// PCD_EP_OutSetupPacket_int TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
	USB->GINTSTS = USB_OTG_GINTSTS_MMIS;	// clear mode mismatch regardless (it does not have any effect)
	if ((USB->GINTSTS) & 0b1UL) { return; }  // exit in host mode  TODO: handle host (low priority)
	
	const uint16_t				frame =		(DEV->DSTS >> USB_OTG_DSTS_FNSOF_Pos) & 0x3FFFUL;
	uint8_t						ep_num;
	uint16_t					ep_gint;
	USB_OEP_t*					oep;


	// TODO: verify (HAL_PCD_IRQHandler @1085-1115)
	/* receive packet interrupt */
	if (USB->GINTSTS & USB_OTG_GINTSTS_RXFLVL) {
		USB->GINTMSK &= ~USB_OTG_GINTMSK_RXFLVLM_Msk;	// note that RXFLVL is masked instead of cleared since that is not possible
		const GRXSTSR_t			RX_status; *((uint32_t*)&RX_status) = USB->GRXSTSR;
		volatile uint32_t*		FIFO = (void*)(((uint32_t)USB) + USB_OTG_FIFO_BASE);
		oep = handle->OEP[RX_status.EPNUM];

		switch (RX_status.PKTSTS) {
			case 0b0010U:  // OUT data packet
				if (!RX_status.BCNT) { break; }
				const uint32_t	words = (RX_status.BCNT >> 2UL) + ((RX_status.BCNT & 0x3UL) != 0x0UL);
				uint32_t*		dest = oep->buffer;
				for (uint32_t i = 0UL; i < words; i++) {
					__UNALIGNED_UINT32_WRITE(oep->buffer, *FIFO);
					dest += 4UL;
				} oep->count += ((uint32_t)dest) - ((uint32_t)oep->buffer);
				break;
			case 0b0110U:  // SETUP data packet
				__UNALIGNED_UINT32_WRITE(handle->setup, *FIFO);
				__UNALIGNED_UINT32_WRITE(handle->setup, *FIFO);
				oep->count += 8UL;
				break;
			// TODO: other cases ????
		}
		USB->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM_Msk;	// unmask the interrupt
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
		// TODO: general core reset func (code used in init)
		// TODO: @1342

		DEV->DCFG &= ~USB_OTG_DCFG_DAD;			// reset DEV address
		// USB_EP0_OutStart TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		USB->GINTSTS |= USB_OTG_GINTSTS_USBRST;
	}

	/* enumeration done interrupt */
	if (USB->GINTSTS & USB_OTG_GINTSTS_ENUMDNE) {
		// TODO: @1386
		// USB_ActivateSetup TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// hpcd->Init.speed = USB_GetDevSpeed(hpcd->Instance);
		// USB_SetTurnaroundTime TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// HAL_PCD_ResetCallback TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		USB->GINTSTS |= USB_OTG_GINTSTS_ENUMDNE;
	}

	/* start of frame interrupt */
	if (USB->GINTSTS & USB_OTG_GINTSTS_SOF) {
		// TODO: @1406
		// HAL_PCD_SOFCallback TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		USB->GINTSTS |= USB_OTG_GINTSTS_SOF;
	}

	/* global OUT NAK effective interrupt */
	if (USB->GINTSTS & USB_OTG_GINTSTS_BOUTNAKEFF) {
		USB->GINTMSK &= ~USB_OTG_GINTMSK_GONAKEFFM;  // mask global OUT NAK effective interrupt
		// TODO: ISO? @1422
	}

	/* connection event interrupt */
	if (USB->GINTSTS & USB_OTG_GINTSTS_SRQINT) {
		// TODO: @1479
		// HAL_PCD_ConnectCallback TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		USB->GINTSTS |= USB_OTG_GINTSTS_SRQINT;
	}

	/* disconnection event interrupt */
	if (USB->GINTSTS & USB_OTG_GINTSTS_OTGINT) {
		uint32_t tmp = USB->GOTGINT;
		if (tmp & USB_OTG_GOTGINT_SEDET) {
			// TODO: @1500
			// HAL_PCD_DisconnectCallback TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		}
		USB->GOTGINT |= tmp;  // restore GOTGINT
	}

	// TODO: other interrupts??

	// TODO: (HAL_PCD_IRQHandler @1085-1115)
	/* TODO: ISO?
	/* incomplete isochronous IN interrupt * /
	if (USB->GINTSTS & USB_OTG_GINTSTS_IISOIXFR) {
		// TODO: @1433
	}
	/* incomplete isochronous OUT interrupt * /
	if (USB->GINTSTS & USB_OTG_GINTSTS_PXFR_INCOMPISOOUT) {
		// TODO: @1453
	}
	*/

	/* TODO: LPM?
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
