//
// Created by marijn on 2/11/24.
//

#include "usb.h"


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
extern USB_status_t* USB_status[2];

/*!<
 * static
* */
static inline void OTG_common_iep_handler(USB_OTG_GlobalTypeDef* usb, uint8_t ep_num) {
	USB_OTG_DeviceTypeDef		*device =	(void*)((uint32_t)usb + USB_OTG_DEVICE_BASE);
	USB_OTG_INEndpointTypeDef	*ep =		(void*)((uint32_t)usb + USB_OTG_IN_ENDPOINT_BASE + ep_num * USB_OTG_EP_REG_SIZE);

	uint32_t ep_int =			(
		ep->DIEPINT & device->DOEPMSK &											// get all triggered interrupts
		((device->DIEPEMPMSK >> ep_num) & 0b1UL) << USB_OTG_DIEPINT_TXFE_Pos	// include TXFE interrupt if enabled
	);

	// TODO: @1202
	if (ep_int & USB_OTG_DIEPINT_XFRC) {

	}
	if (ep_int & USB_OTG_DIEPINT_TOC) {

	}
	if (ep_int & USB_OTG_DIEPINT_ITTXFE) {

	}
	if (ep_int & USB_OTG_DIEPINT_INEPNE) {

	}
	if (ep_int & USB_OTG_DIEPINT_EPDISD) {

	}
	if (ep_int & USB_OTG_DIEPINT_TXFE) {

	}
}
static inline void OTG_common_oep_handler(USB_OTG_GlobalTypeDef* usb, uint8_t ep_num) {
	USB_OTG_DeviceTypeDef		*device =	(void*)((uint32_t)usb + USB_OTG_DEVICE_BASE);
	USB_OTG_OUTEndpointTypeDef	*ep =		(void*)((uint32_t)usb + USB_OTG_OUT_ENDPOINT_BASE + ep_num * USB_OTG_EP_REG_SIZE);

	uint32_t ep_int =			ep->DOEPINT & device->DOEPMSK;					// get all triggered interrupts

	// TODO: @1130
	if (ep_int & USB_OTG_DOEPINT_XFRC) {

	}
	if (ep_int & USB_OTG_DOEPINT_STUP) {

	}
	if (ep_int & USB_OTG_DOEPINT_OTEPDIS) {

	}
	if (ep_int & USB_OTG_DOEPINT_EPDISD) {

	}
	if (ep_int & USB_OTG_DOEPINT_OTEPSPR) {

	}
	if (ep_int & USB_OTG_DOEPINT_NAK) {

	}
}
static inline void OTG_common_handler(USB_OTG_GlobalTypeDef* usb) {
	usb->GINTSTS = USB_OTG_GINTSTS_MMIS;	// clear mode mismatch regardless (it does not have any effect)
	if ((usb->GINTSTS) & 0b1UL) { return; }  // exit in host mode  TODO: handle host (low priority)
	USB_OTG_DeviceTypeDef		*device =	(void*)((uint32_t)usb + USB_OTG_DEVICE_BASE);
	USB_OTG_INEndpointTypeDef	*in =		(void*)((uint32_t)usb + USB_OTG_IN_ENDPOINT_BASE);
	USB_OTG_OUTEndpointTypeDef	*out =		(void*)((uint32_t)usb + USB_OTG_OUT_ENDPOINT_BASE);
	USB_status_t				*status =	USB_status[((uint32_t)usb >> 0x13UL) & 0b1UL];
	const uint16_t				frame =		(device->DSTS >> USB_OTG_DSTS_FNSOF_Pos) & 0x3FFFUL;

	uint8_t						ep_num;
	uint16_t					ep_gint;
	USB_OEP_status_t*			oep;

	// TODO: verify (HAL_PCD_IRQHandler @1085-1115)
	/* receive packet interrupt */
	if (usb->GINTSTS & USB_OTG_GINTSTS_RXFLVL) {
		usb->GINTMSK &= ~USB_OTG_GINTMSK_RXFLVLM_Msk;	// note that RXFLVL is masked instead of cleared since that is not possible
		const GRXSTSR_t			RX_status; *((uint32_t*)&RX_status) = usb->GRXSTSR;
		volatile uint32_t*		FIFO = (void*)(((uint32_t)usb) + USB_OTG_FIFO_BASE);
		oep = &status->OEP[RX_status.EPNUM];

		switch (RX_status.PKTSTS) {
			case 0b0010U:  // OUT data packet
				if (!RX_status.BCNT) { break; }
				const uint32_t	words = (RX_status.BCNT >> 2UL) + ((RX_status.BCNT & 0x3UL) != 0x0UL);
				uint32_t*		dest = oep->buffer;
				for (uint32_t i = 0UL; i < words; i++) {
					__UNALIGNED_UINT32_WRITE(oep->buffer, *FIFO);
					dest += 4UL;
				} oep->count += dest - oep->buffer;
				break;
			case 0b0110U:  // SETUP data packet
				__UNALIGNED_UINT32_WRITE(status->setup, *FIFO);
				__UNALIGNED_UINT32_WRITE(status->setup, *FIFO);
				oep->count += 8UL;
				break;
			// TODO: other cases ????
		}
		usb->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM_Msk;	// unmask the interrupt
	}

	/* IN endpoints interrupt */
	if (usb->GINTSTS & USB_OTG_GINTSTS_IEPINT) {
		ep_num = 0U;
		ep_gint = device->DAINT & device->DAINTMSK & 0xFFFFU;	// get interrupt bits for each IN endpoint
		while (ep_gint) {
			if (ep_gint & 0b1UL) { OTG_common_iep_handler(usb, ep_num); }
			ep_num++; ep_gint >>= 1UL;
		}
	}

	/* OUT endpoints interrupt */
	if (usb->GINTSTS & USB_OTG_GINTSTS_OEPINT) {
		ep_num = 0U;
		ep_gint = (device->DAINT & device->DAINTMSK) >> 0x10UL;	// get interrupt bits for each OUT endpoint
		while (ep_gint) {
			if (ep_gint & 0b1UL) { OTG_common_oep_handler(usb, ep_num); }
			ep_num++; ep_gint >>= 1UL;
		}
	}

	/* suspend interrupt */
	if (usb->GINTSTS & USB_OTG_GINTSTS_USBSUSP) {
		// TODO: @1297
	}

	/* wake-up interrupt */
	if (usb->GINTSTS & USB_OTG_GINTSTS_WKUINT) {
		// TODO: @1269
	}

	/* reset interrupt */
	if (usb->GINTSTS & USB_OTG_GINTSTS_USBRST) {
		// TODO: @1337
	}

	/* enumeration done interrupt */
	if (usb->GINTSTS & USB_OTG_GINTSTS_ENUMDNE) {
		// TODO: @1386
	}

	/* start of frame interrupt */
	if (usb->GINTSTS & USB_OTG_GINTSTS_SOF) {
		// TODO: @1406
	}

	/* global OUT NAK effective interrupt */
	if (usb->GINTSTS & USB_OTG_GINTSTS_BOUTNAKEFF) {
		// TODO: @1418
	}

	/* incomplete isochronous IN interrupt */
	if (usb->GINTSTS & USB_OTG_GINTSTS_IISOIXFR) {
		// TODO: @1433
	}

	/* incomplete isochronous OUT interrupt */
	if (usb->GINTSTS & USB_OTG_GINTSTS_PXFR_INCOMPISOOUT) {
		// TODO: @1453
	}

	/* connection event interrupt */
	if (usb->GINTSTS & USB_OTG_GINTSTS_SRQINT) {
		// TODO: @1479
	}

	/* disconnection event interrupt */
	if (usb->GINTSTS & USB_OTG_GINTSTS_OTGINT) {
		// TODO: @1491
	}

	// TODO: other interrupts??
	// if (usb->GINTSTS & USB_OTG_GINTSTS_LPMINT) {}  // @1311
}


/*!<
 * interrupts
 * */
extern void OTG_HS_IRQHandler(void) { OTG_common_handler(USB_OTG_HS); }
extern void OTG_FS_IRQHandler(void) { OTG_common_handler(USB_OTG_FS); }

