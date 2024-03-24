//
// Created by marijn on 2/11/24.
//

#include "usb/usb.h"


#ifdef USB_LIB


/*!<
 * defines
* */
#define DEVICE_FEATURE_WAKE_HOST			0x01U

#define DEVICE_CONFIG_SELF_POWERED			0x01U
#define DEVICE_CONFIG_WAKE_HOST				0x02U

#define inline __attribute__((always_inline))



/*!<
 * types
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
}	GRXSTSR_t;


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
 * exported functions
 * */
void open_IEP(USB_handle_t* handle, uint8_t ep_num, uint16_t mps, EP_type_t type) {
	USB_IEP_t* iep = handle->IEP[ep_num];
	iep->mps =	mps;			// set max packet size
	iep->type =			type;
	iep->used =			0b1U;
	// TODO: consider saving epnum
	DEV->DAINTMSK |= 0b1UL << ep_num;	// unmask IEPx interrupts
	if (!(I_EP->DIEPCTL & USB_OTG_DIEPCTL_USBAEP)) {
		I_EP->DIEPCTL |= (
			(mps & USB_OTG_DIEPCTL_MPSIZ)			|	// set max packet size
			(type << USB_OTG_DIEPCTL_EPTYP_Pos)		|	// set endpoint type
			(ep_num << USB_OTG_DIEPCTL_TXFNUM_Pos)	|	// set TX FIFO number
			USB_OTG_DIEPCTL_SD0PID_SEVNFRM			|	// set DATA0
			USB_OTG_DIEPCTL_USBAEP						// enable endpoint
		);
	}
}
void open_OEP(USB_handle_t* handle, uint8_t ep_num, uint16_t mps, EP_type_t type) {
	USB_OEP_t* oep = handle->OEP[ep_num];
	oep->mps =	mps;		// set max packet size
	oep->type =			type;
	oep->used =			0b1U;
	// TODO: consider saving epnum
	DEV->DAINTMSK |= 0x100UL << ep_num;	// unmask OEPx interrupts
	if (!(O_EP->DOEPCTL & USB_OTG_DOEPCTL_USBAEP)) {
		O_EP->DOEPCTL |= (
			(mps & USB_OTG_DOEPCTL_MPSIZ)			|	// set max packet size
			(type << USB_OTG_DOEPCTL_EPTYP_Pos)		|	// set endpoint type
			USB_OTG_DOEPCTL_SD0PID_SEVNFRM			|	// set DATA0
			USB_OTG_DOEPCTL_USBAEP						// enable endpoint
		);
	}
}
void open_EP(USB_handle_t* handle, uint8_t ep_num, uint16_t mps, EP_type_t type) {
	open_IEP(handle, ep_num, mps, type); open_OEP(handle, ep_num, mps, type);
}
void close_IEP(USB_handle_t* handle, uint8_t ep_num) {
	USB_IEP_t* iep = handle->IEP[ep_num];
	iep->mps =	0U;
	iep->type = 0b00;
	iep->used = 0b0;

	if (I_EP->DIEPCTL & USB_OTG_DIEPCTL_EPENA) {
		I_EP->DIEPCTL |= (
			USB_OTG_DIEPCTL_SNAK		|
			USB_OTG_DIEPCTL_EPDIS
		);
	}
	DEV->DEACHMSK &= ~(0b1UL << ep_num);
	DEV->DAINTMSK &= ~(0b1UL << ep_num);
	I_EP->DIEPCTL &= ~(
		USB_OTG_DIEPCTL_USBAEP			|
		USB_OTG_DIEPCTL_MPSIZ			|
		USB_OTG_DIEPCTL_TXFNUM			|
		USB_OTG_DIEPCTL_SD0PID_SEVNFRM	|
		USB_OTG_DIEPCTL_EPTYP
	);
}
void close_OEP(USB_handle_t* handle, uint8_t ep_num) {
	USB_OEP_t* oep = handle->OEP[ep_num];
	oep->mps =	0U;
	oep->type = 0b00;
	oep->used = 0b0;

	if (O_EP->DOEPCTL & USB_OTG_DOEPCTL_EPENA) {
		O_EP->DOEPCTL |= (
			USB_OTG_DOEPCTL_SNAK		|
			USB_OTG_DOEPCTL_EPDIS
		);
	}
	DEV->DEACHMSK &= ~(0x100UL << ep_num);
	DEV->DAINTMSK &= ~(0x100UL << ep_num);
	O_EP->DOEPCTL &= ~(
		USB_OTG_DOEPCTL_USBAEP			|
		USB_OTG_DOEPCTL_MPSIZ			|
		USB_OTG_DOEPCTL_SD0PID_SEVNFRM	|
		USB_OTG_DOEPCTL_EPTYP
	);
}
void close_EP(USB_handle_t* handle, uint8_t ep_num) {
	close_IEP(handle, ep_num); close_OEP(handle, ep_num);
}


/*!<
 * static
* */
/*!< setup / init */
static inline void /**/ start_OEP0(USB_handle_t* handle, USB_OEP_t* oep) {
	// TODO: CORE VERISON?	[stm32h7xx_ll_usb.c] @2263
	if (O_EP->DOEPCTL & USB_OTG_DOEPCTL_EPENA) { return; }
	O_EP->DOEPTSIZ = (
		0x18UL									|	// expect 24 bytes to be received
		(0b1UL << USB_OTG_DOEPTSIZ_PKTCNT_Pos)	|	// set bit (will be reset when packet is received)
		(0x3UL << USB_OTG_DOEPTSIZ_STUPCNT_Pos)		// set max number of back to back packets to 3
	);
	// TODO: DMA?			[stm32h7xx_ll_usb.c] @1427
}
static inline void /**/ stall_IEP(USB_handle_t* handle, uint8_t ep_num) {
	USB_IEP_t* iep = handle->IEP[ep_num];
	iep->stall = 0b1U;
	if (ep_num && !(I_EP->DIEPCTL & USB_OTG_DIEPCTL_EPENA)) {
		I_EP->DIEPCTL &= ~(USB_OTG_DIEPCTL_EPDIS);		// TODO: EPDIS is rs (no effect????)
	}
    I_EP->DIEPCTL |= USB_OTG_DIEPCTL_STALL;
}
static inline void /**/ stall_OEP(USB_handle_t* handle, uint8_t ep_num) {
	USB_OEP_t* oep = handle->OEP[ep_num];
	oep->stall = 0b1U;
	if (ep_num && !(O_EP->DOEPCTL & USB_OTG_DOEPCTL_EPENA)) {
		O_EP->DOEPCTL &= ~(USB_OTG_DOEPCTL_EPDIS);		// TODO: EPDIS is rs (no effect????)
	}
	O_EP->DOEPCTL |= USB_OTG_DOEPCTL_STALL;
	start_OEP0(handle, oep);
}
static inline void /**/ stall_EP(USB_handle_t* handle, uint8_t ep_num) {
	stall_IEP(handle, ep_num); stall_OEP(handle, ep_num);
}
static inline void /**/ unstall_IEP(USB_handle_t* handle, uint8_t ep_num) {
	USB_IEP_t* iep = handle->IEP[ep_num];
	iep->stall = 0b0U;
	I_EP->DIEPCTL &= ~USB_OTG_DIEPCTL_STALL;
	if (iep->type & 0b10U) {  // BULK or INTERRUPT
		I_EP->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;	// set DATA0
	}
}
static inline void /**/ unstall_OEP(USB_handle_t* handle, uint8_t ep_num) {
	USB_OEP_t* oep = handle->OEP[ep_num];
	oep->stall = 0b0U;
	O_EP->DOEPCTL &= ~USB_OTG_DOEPCTL_STALL;
	if (oep->type & 0b10U) {  // BULK or INTERRUPT
		O_EP->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM;	// set DATA0
	}
}

/*!< IO logic */
static inline void /**/ IN_transfer(USB_handle_t* handle, uint8_t ep_num, void* buffer, uint32_t size) {
	USB_IEP_t* iep = handle->IEP[ep_num];
	iep->src = buffer;
	iep->size = size;
	iep->count = 0U;
	// TODO: DMA? [stm32h7xx_hal_pcd.c] @1908

	I_EP->DIEPTSIZ &= ~USB_OTG_DIEPTSIZ_PKTCNT;
	I_EP->DIEPTSIZ &= ~USB_OTG_DIEPTSIZ_XFRSIZ;

	if (ep_num) {
		if (!size) {
			I_EP->DIEPTSIZ |= (1UL << USB_OTG_DIEPTSIZ_PKTCNT_Pos);
		} else {
			I_EP->DIEPTSIZ |= (
				((iep->size + iep->mps - 1U) / iep->mps) << USB_OTG_DIEPTSIZ_PKTCNT_Pos
				& USB_OTG_DIEPTSIZ_PKTCNT
			);
			I_EP->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_XFRSIZ & iep->size);
			// TODO: ISO? [stm32h7xx_ll_usb.c] @776
		}
		// TODO: DMA? [stm32h7xx_ll_usb.c] @783
		// TODO: ISO? [stm32h7xx_ll_usb.c] @810
	} else {
		I_EP->DIEPTSIZ |= (1UL << USB_OTG_DIEPTSIZ_PKTCNT_Pos);
		if (size > iep->mps) { size = iep->mps; }  // reuse 'size' as next transfer size
		I_EP->DIEPTSIZ |= (size & USB_OTG_DIEPTSIZ_XFRSIZ);
		// TODO: DMA? [stm32h7xx_ll_usb.c] @925
	}

	I_EP->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);	// enable EP
	if (size) { DEV->DIEPEMPMSK |= 0b1UL << ep_num; }					// unmask TX FIFO empty interrupt
}
static inline void /**/ OUT_transfer(USB_handle_t* handle, uint8_t ep_num, void* buffer, uint32_t size) {
	USB_OEP_t* oep = handle->OEP[ep_num];
	oep->dest = buffer;
	oep->size = size;
	oep->count = 0U;
	// TODO: DMA? [stm32h7xx_hal_pcd.c] @1860

	O_EP->DOEPTSIZ &= ~USB_OTG_DOEPTSIZ_PKTCNT;
	O_EP->DOEPTSIZ &= ~USB_OTG_DOEPTSIZ_XFRSIZ;

	if (ep_num) {
		if (!size) {
			O_EP->DOEPTSIZ |= (1UL << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
			O_EP->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_XFRSIZ & oep->mps);
		} else {
			O_EP->DOEPTSIZ |= (
				((oep->size + oep->mps - 1U) / oep->mps) << USB_OTG_DOEPTSIZ_PKTCNT_Pos
				& USB_OTG_DOEPTSIZ_PKTCNT
			);
			O_EP->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_XFRSIZ & oep->size);
		}
		// TODO: DMA? [stm32h7xx_ll_usb.c] @856
		// TODO: ISO? [stm32h7xx_ll_usb.c] @864
	} else {
		oep->size = oep->mps;	// TODO @956 ????????
		O_EP->DOEPTSIZ |= (1UL << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
		O_EP->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_XFRSIZ & oep->mps);
		// TODO: DMA? [stm32h7xx_ll_usb.c] @967
	}

	O_EP->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);	// enable EP
}

/* IO stage logic*/ // TODO: check "control_IN/OUT_stage"
static inline void /*check*/ control_IN_stage(USB_handle_t* handle) {
	if (handle->EP0_state != EP_DATA_IN) { return; }
	USB_IEP_t* iep = handle->IEP[0];
	uint32_t len = iep->size - iep->count;
	// TODO: check code because 'pep->rem_length' in HAL is implemented strangely
	if (len) {
		// continue sending data (iep->src and iep->count incremented!)
		IN_transfer(handle, 0, iep->src, len);	// continue transfer
		OUT_transfer(handle, 0, NULL, 0UL);	// prepare for premature end of transfer
		return;
	} if (  // end: send ZLP packet
		iep->size >= iep->mps &&
		iep->size < handle->EP0_data
	) {
		handle->EP0_data = 0UL;
		IN_transfer(handle, 0, NULL, 0UL);	// stop transfer
		OUT_transfer(handle, 0, NULL, 0UL);	// prepare for premature end of transfer
		return;
	} if (
		handle->state == USB_CONFIGURED &&
		handle->class->IEP0_sent
	) {
		handle->class->IEP0_sent(handle);		// TODO: errors??
	}
	stall_IEP(handle, 0);
	handle->EP0_state = EP_STATUS_OUT;
	OUT_transfer(handle, 0, NULL, 0UL);
}
static inline void /**/ data_IN_stage(USB_handle_t* handle, uint8_t ep_num) {
	if (!ep_num) { return control_IN_stage(handle); }
	if (
		handle->class->data_in &&		// make sure the class has a data_in function
		handle->state == USB_CONFIGURED	// make sure the device is configured
	) {
		handle->class->data_in(handle->usb, ep_num);	// TODO: errors??
	}
}
static inline void /*check*/ control_OUT_stage(USB_handle_t* handle) {
	if (handle->EP0_state != EP_DATA_OUT) { return; }
	USB_OEP_t* oep = handle->OEP[0];
	uint32_t len = oep->size - oep->count;
	if (len) {
		len = len > oep->mps ? oep->mps : len;
		OUT_transfer(handle, 0, oep->dest, len);
		return;
	} if (
		handle->state == USB_CONFIGURED &&
		handle->class->OEP0_ready
	) {
		handle->class->OEP0_ready(handle);		// TODO: errors??
	}
	handle->EP0_state = EP_STATUS_IN;
	IN_transfer(handle, 0, NULL, 0U);
}
static inline void /**/ data_OUT_stage(USB_handle_t* handle, uint8_t ep_num) {
	if (!ep_num) { return control_OUT_stage(handle); }
	if (
		handle->class->data_out &&		// make sure the class has a data_in function
		handle->state == USB_CONFIGURED	// make sure the device is configured
	) {
		handle->class->data_out(handle->usb, ep_num);	// TODO: errors??
	}
}

/* setup stage logic */  // TODO: check functions
static inline void /**/ get_device_descriptor(USB_handle_t* handle, setup_header_t* req) {
	uint8_t* ptr = NULL;

	switch (req->value >> 8) {
	// TODO: LPM / BOS?
	case USB_device_descriptor_type:
		ptr = handle->descriptor->device; break;
	case USB_config_descriptor_type:
		handle->EP0_state = EP_DATA_IN;				// control data IN
		IN_transfer(handle, 0, handle->class->descriptor, handle->class->descriptor_size);
		break;
	case USB_string_descriptor_type:
		switch (req->value & 0xFFU) {
		case USB_language_ID_string_descriptor_type:
			ptr = handle->descriptor->lang_ID_string; break;
		case USB_manufacturer_string_descriptor_type:
			ptr = handle->descriptor->manufacturer_string; break;
		case USB_product_string_descriptor_type:
			ptr = handle->descriptor->product_string; break;
		case USB_serial_string_descriptor_type:
			ptr = handle->descriptor->serial_string; break;
		case USB_config_string_descriptor_type:
			ptr = handle->descriptor->configuration_string; break;
		case USB_interface_string_descriptor_type:
			ptr = handle->descriptor->interface_string; break;
		default:  // TODO: USER STR? @494 [usbd_ctlreq.c]
			stall_EP(handle, 0);			// error
			return;
		} break;
	case USB_qualifier_descriptor_type:		// TODO: HS?
	case USB_other_speed_descriptor_type:	// TODO: HS?
	default:
		stall_EP(handle, 0);					// error
		return;
	}
	if (!req->length) {
		handle->EP0_state = EP_STATUS_IN;				// control status IN
		IN_transfer(handle, 0, NULL, 0U);
		return;
	}
	if (!ptr) { stall_EP(handle, 0); return; }	// error
	handle->EP0_state = EP_DATA_IN;						// control data IN
	IN_transfer(handle, 0, ptr, *ptr);
}
static inline void /**/ device_setup_request(USB_handle_t* handle) {
	setup_header_t* req = &handle->request;
	uint8_t index;

	switch (req->type) {
	case STANDARD_REQUEST:
		switch (req->command) {
		case GET_STATUS:
			if (
				req->length != 2U ||
				handle->state > USB_CONFIGURED
			) { break; }										// error
			handle->EP0_state = EP_DATA_IN;						// control data IN
			IN_transfer(handle, 0, &handle->device_config, 2U);
			return;
		case CLEAR_FEATURE:
			if (handle->state > USB_CONFIGURED) { break; }		// error
			if (req->value != DEVICE_FEATURE_WAKE_HOST) { return; }
			handle->device_config &= ~DEVICE_CONFIG_WAKE_HOST;
			handle->EP0_state = EP_STATUS_IN;					// control status IN
			IN_transfer(handle, 0, NULL, 0U);
			return;
		case SET_FEATURE:
			if (req->value != DEVICE_FEATURE_WAKE_HOST) { return; }
			handle->device_config |= DEVICE_CONFIG_WAKE_HOST;
			handle->EP0_state = EP_STATUS_IN;					// control status IN
			IN_transfer(handle, 0, NULL, 0U);
			return;
		case SET_ADDRESS:
			if (
				req->value || req->length || (req->value >= 128U) ||
				handle->state == USB_CONFIGURED
			) { break; }	// error
			handle->address = req->value;

			DEV->DCFG &= ~(USB_OTG_DCFG_DAD);					// update device address
			DEV->DCFG |= req->value << USB_OTG_DCFG_DAD_Pos;

			handle->EP0_state = EP_STATUS_IN;					// control status IN
			IN_transfer(handle, 0, NULL, 0U);

			if (req->value) { handle->state = USB_ADDRESSED; }
			else			{ handle->state = USB_DEFAULT; }
			return;
		case GET_DESCRIPTOR:
			get_device_descriptor(handle, req);
			return;
		case SET_DESCRIPTOR: break;								// TODO: NOT IMPLEMENTD???????????
		case GET_CONFIGURATION:
			if (req->length != 0x1U) { break; }					// error
			switch (handle->state) {
			case USB_DEFAULT:
			case USB_ADDRESSED:
				handle->EP0_state = EP_DATA_IN;					// control data IN
				IN_transfer(handle, 0, "\0", 1U);	// send 0x00
				return;
			case USB_CONFIGURED:
				handle->EP0_state = EP_DATA_IN;					// control data IN
				IN_transfer(handle, 0, &handle->device_config, 1U);
				return;
			default: break;
			} break;
		case SET_CONFIGURATION:
			index = req->value & 0xFFU;
			if (index > USB_OTG_MAX_CONFIG_COUNT) { break; }	// error
			switch (handle->state) {
			case USB_ADDRESSED:
				if (index) {
					handle->class_init = 0b1U;
					handle->class->init(handle);				// TODO: errors?
					handle->state = USB_CONFIGURED;
				}
				handle->EP0_state = EP_STATUS_IN;				// control status IN
				IN_transfer(handle, 0, NULL, 0U);
				return;
			case USB_CONFIGURED:
				if (!index) {
					handle->class_init = 0b0U;
					handle->state = USB_ADDRESSED;
					handle->class->de_init(handle);				// TODO: errors?
				}
				handle->EP0_state = EP_STATUS_IN;				// control status IN
				IN_transfer(handle, 0, NULL, 0U);
				return;  // TODO: multi config??
			default: break;
			} break;
		default: break;
		} break;
	case CLASS_REQUEST:
	case VENDOR_REQUEST:
		handle->class->setup(handle);		// TODO: errors?
		return;
	default: break;
	}

	// catch all errors
	stall_EP(handle, 0);
}
static inline void /**/ interface_setup_request(USB_handle_t* handle) {
	setup_header_t* req = &handle->request;
	switch (req->type) {
	case STANDARD_REQUEST:
	case CLASS_REQUEST:
	case VENDOR_REQUEST:
		switch (handle->state) {
		case USB_DEFAULT:
		case USB_ADDRESSED:
		case USB_CONFIGURED:
			if ((req->index & 0xFFU) > USB_OTG_MAX_INTERFACE_COUNT) { break; }  // error
			handle->class->setup(handle);  // TODO: errors??
			if (req->length) { return; }
			handle->EP0_state = EP_STATUS_IN;					// control status IN
			IN_transfer(handle, 0, NULL, 0U);
			return;
		default: break;
		} break;
	default: break;
	}

	// catch all errors
	stall_EP(handle, 0);
}
static inline void /**/ endpoint_setup_request(USB_handle_t* handle) {
	setup_header_t* req = &handle->request;
	uint8_t ep_num = req->index & 0x7FU;
	uint8_t in = (req->index & 0x80U) == 0x80U;
	uint16_t status = 0x0000U;

	switch (req->type) {
	case STANDARD_REQUEST:
		switch (req->command) {
		case GET_STATUS:							// TODO: validate @298 [usbd_ctlreq.c]
			switch (handle->state) {
			case USB_ADDRESSED:
				if (ep_num)	{ break; }				// error
				handle->EP0_state = EP_DATA_IN;
				IN_transfer(handle, 0, &status, 2U);
				return;
			case USB_CONFIGURED:
				if (in) {
					USB_IEP_t* iep = handle->IEP[ep_num];
					if (!iep->used) { break; }		// error
					if (ep_num) { status |= iep->stall; }
				} else {
					USB_OEP_t* oep = handle->OEP[ep_num];
					if (!oep->used) { break; }		// error
					if (ep_num) { status |= oep->stall; }
				}
				handle->EP0_state = EP_DATA_IN;
				IN_transfer(handle, 0, &status, 2U);
				return;
			default: break;
			} break;
		case CLEAR_FEATURE:							// TODO: validate @264 [usbd_ctlreq.c]
			switch (handle->state) {
			case USB_ADDRESSED:						// TODO: same as SET_FEATURE, USB_ADDRESSED valid????
				if (!ep_num)	{ break; }			// error
				if (in)			{ stall_IEP(handle, ep_num); }
				else			{ stall_OEP(handle, ep_num); }
				stall_IEP(handle, 0);		// TODO: why stall IEP0??
				return;
			case USB_CONFIGURED:
				if (req->value) { return; }
				if (ep_num) {						// value == 0 -> clear: FEATURE_EP_HALT
					if (in)	{ unstall_IEP(handle, ep_num); }
					else	{ unstall_OEP(handle, ep_num); }
				}
				handle->EP0_state = EP_STATUS_IN;	// control status IN
				IN_transfer(handle, 0, NULL, 0U);
				handle->class->setup(handle);		// TODO: errors??
				return;
			default: break;
			} break;
		case SET_FEATURE:							// TODO: validate @231 [usbd_ctlreq.c]
			switch (handle->state) {
			case USB_ADDRESSED:						// TODO: same as CLEAR_FEATURE, USB_ADDRESSED valid????
				if (!ep_num)	{ break; }			// error
				if (in)			{ stall_IEP(handle, ep_num); }
				else			{ stall_OEP(handle, ep_num); }
				stall_IEP(handle, 0);		// TODO: why stall IEP0??
				return;
			case USB_CONFIGURED:
				if (req->value) { return; }
				if (ep_num && !req->length) {		// value == 0 -> set: FEATURE_EP_HALT
					if (in)	{ stall_IEP(handle, ep_num); }
					else	{ stall_OEP(handle, ep_num); }
				}
				handle->EP0_state = EP_STATUS_IN;	// control status IN
				IN_transfer(handle, 0, NULL, 0U);
				return;
			default: break;
			} break;
		default: break;
		} break;
	case CLASS_REQUEST:
	case VENDOR_REQUEST:
		handle->class->setup(handle);				// TODO: errors??
		return;
	default: break;
	}

	// catch all errors
	stall_EP(handle, 0);
}

/*!< usb IRQ logic */
static inline void /**/ USB_OTG_IRQ(USB_handle_t* handle) {
	uint32_t tmp = USB->GOTGINT;
	if (tmp & USB_OTG_GOTGINT_SEDET) {
		handle->state = USB_DEFAULT;
		handle->class->de_init(handle);
	}
	USB->GOTGINT |= tmp;  // restore GOTGINT
}
static inline void /**/ USB_SOF_IRQ(USB_handle_t* handle) {
	USB->GINTSTS = USB_OTG_GINTSTS_SOF;
	if (
		handle->state == USB_CONFIGURED &&
		handle->class->SOF
	) { handle->class->SOF(handle); }
}
static inline void /**/ USB_receive_packet_IRQ(USB_handle_t* handle) {
	USB_OEP_t*		oep;

	USB->GINTMSK &= ~USB_OTG_GINTMSK_RXFLVLM_Msk;	// note that RXFLVL is masked instead of cleared since that is not possible
	uint32_t a = USB->GRXSTSR;
	if (a) { GPIO_write(GPIOA, 1, 0); }
	GRXSTSR_t RX_status; *((uint32_t*)&RX_status) = a;
	volatile uint32_t* FIFO = (void*)(((uint32_t)USB) + USB_OTG_FIFO_BASE);
	oep = handle->OEP[RX_status.EPNUM];

	switch (RX_status.PKTSTS) {  // TODO: PKTSTS: 12?
	case 0b0010U:  // OUT data packet
		if (!RX_status.BCNT) { break; }
		const uint32_t	words = RX_status.BCNT >> 2U;
		const uint8_t	bytes = RX_status.BCNT & 0b11UL;
		uint32_t		buffer = (uint32_t)oep->dest;
		for (uint32_t i = 0UL; i < words; i++) {
			__UNALIGNED_UINT32_WRITE(oep->dest, *FIFO);
			// 4x inc is faster than an iadd due to pipelining
			oep->dest++; oep->dest++;
			oep->dest++; oep->dest++;
		} if (bytes) {
			uint32_t tmp; __UNALIGNED_UINT32_WRITE(&tmp, *FIFO);
			for (uint8_t i = 0; i < bytes; i++) {
				*(uint8_t*)oep->dest++ = (uint8_t)(tmp >> (8U * i));
			}
		} oep->count += ((uint32_t)oep->dest) - buffer;
		break;

	case 0b0110U:  // SETUP data packet
		__UNALIGNED_UINT32_WRITE(&handle->setup[0], *FIFO);
		__UNALIGNED_UINT32_WRITE(&handle->setup[1], *FIFO);
		oep->count += 8UL;  // TODO: is BCNT also 8???
		break;

	default: break;  // TODO: other cases ????
	}
	USB->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM_Msk;	// unmask interrupt
}
static inline void /**/ USB_reset_IRQ(USB_handle_t* handle) {
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
	DEV->DCFG &= ~USB_OTG_DCFG_DAD;				// reset device address

	start_OEP0(handle, *handle->OEP);
	USB->GINTSTS = USB_OTG_GINTSTS_USBRST;
}
static inline void /**/ USB_enumeration_done_IRQ(USB_handle_t* handle, USB_IEP_t* iep) {
	I_EP->DIEPCTL &= ~USB_OTG_DIEPCTL_MPSIZ;	// reset IEP0 maximum packet size
	DEV->DCTL |= USB_OTG_DCTL_CGINAK;			// clear global IN NAK

	// TODO: HS?				[stm32h7xx_ll_usb.c] @205
	USB->GUSBCFG &= ~USB_OTG_GUSBCFG_TRDT;  // reset turnaround time
	if		(AHB_clock_frequency < 15000000UL)	{ USB->GUSBCFG |= 0xFU << USB_OTG_GUSBCFG_TRDT_Pos; }
	else if	(AHB_clock_frequency < 16000000UL)	{ USB->GUSBCFG |= 0xEU << USB_OTG_GUSBCFG_TRDT_Pos; }
	else if	(AHB_clock_frequency < 17200000UL)	{ USB->GUSBCFG |= 0xDU << USB_OTG_GUSBCFG_TRDT_Pos; }
	else if	(AHB_clock_frequency < 18500000UL)	{ USB->GUSBCFG |= 0xCU << USB_OTG_GUSBCFG_TRDT_Pos; }
	else if	(AHB_clock_frequency < 20000000UL)	{ USB->GUSBCFG |= 0xBU << USB_OTG_GUSBCFG_TRDT_Pos; }
	else if	(AHB_clock_frequency < 21800000UL)	{ USB->GUSBCFG |= 0xAU << USB_OTG_GUSBCFG_TRDT_Pos; }
	else if	(AHB_clock_frequency < 24000000UL)	{ USB->GUSBCFG |= 0x9U << USB_OTG_GUSBCFG_TRDT_Pos; }
	else if	(AHB_clock_frequency < 27700000UL)	{ USB->GUSBCFG |= 0x8U << USB_OTG_GUSBCFG_TRDT_Pos; }
	else if	(AHB_clock_frequency < 32000000UL)	{ USB->GUSBCFG |= 0x7U << USB_OTG_GUSBCFG_TRDT_Pos; }
	else										{ USB->GUSBCFG |= 0x6U << USB_OTG_GUSBCFG_TRDT_Pos; }
	// TODO: HS? (see: HAL_PCD_ResetCallback)

	handle->state = USB_DEFAULT;
	handle->EP0_state = EP_IDLE;
	handle->device_config = 0x0000U;
	handle->class->de_init(handle);
	open_EP(handle, 0, 0x40UL, EP_TYPE_CONTROL);

	USB->GINTSTS = USB_OTG_GINTSTS_ENUMDNE;		// clear interrupt
}

/*!< IEP IRQ logic */  // TODO: check "IEP_FIFO_empty_IRQ" for out of bounds read possibility
static inline void /**/ IEP_transfer_complete_IRQ(USB_handle_t* handle, USB_IEP_t* iep, uint8_t ep_num) {
	DEV->DIEPEMPMSK &= ~(0b1UL << ep_num);	// mask interrupt
	I_EP->DIEPINT = USB_OTG_DIEPINT_XFRC;
	// TODO: DMA?				[stm32h7xx_hal_pcd.c] @1209
	data_IN_stage(handle, ep_num);
}
static inline void /**/ IEP_disabled_IRQ(USB_handle_t* handle, USB_IEP_t* iep, uint8_t ep_num) {
	flush_TX_FIFO(handle->usb, ep_num);
	// TODO: ISO?				[stm32h7xx_hal_pcd.c] @1245
	I_EP->DIEPINT = USB_OTG_DIEPINT_EPDISD;
}
static inline void /*check*/ IEP_FIFO_empty_IRQ(USB_handle_t* handle, USB_IEP_t* iep, uint8_t ep_num) {
	if (iep->count > iep->size) { return; }
	uint32_t	len, len32, i, buffer;
	uint8_t		*FIFO = (uint8_t*)((uint32_t)USB + (0x1000UL * (ep_num + 1U)));
	while (iep->size && iep->count < iep->size) {
		len =	iep->size - iep->count;
		len32 =	((len > iep->mps ? iep->mps : len) + 3U) >> 2U;
		if ((I_EP->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) < len32) { break; }
		buffer = (uint32_t)iep->src;							// save start ptr
		for (i = 0UL; i < len32; i++) {							// write words
			*FIFO = __UNALIGNED_UINT32_READ(iep->src);
			iep->src++; iep->src++; iep->src++; iep->src++;		// 4x inc is faster than an add due to pipelining
		}  // TODO: out of bounds read??
		iep->count += buffer - ((uint32_t)iep->src);			// write ptr difference
	}
	if (iep->size <= iep->count) {
		// mask TX FIFO empty interrupt after all data is sent
		DEV->DIEPEMPMSK &= ~(0b1UL << ep_num);
	}
}

/*!< OEP IRQ logic */
static inline void /**/ OEP_transfer_complete(USB_handle_t* handle, USB_OEP_t* oep, uint8_t ep_num) {
	O_EP->DOEPINT = USB_OTG_DOEPINT_XFRC;
	// TODO: DMA?				[stm32h7xx_hal_pcd.c] @2217
	// TODO: CORE VERISON?		[stm32h7xx_hal_pcd.c] @2274

	if (!ep_num && !oep->size) { start_OEP0(handle, oep); }
	data_OUT_stage(handle, ep_num);
}
static inline void /**/ OEP_disabled(USB_handle_t* handle, USB_OEP_t* oep) {
	if (handle->usb->GINTSTS & USB_OTG_GINTSTS_BOUTNAKEFF) {
		DEV->DCTL |= USB_OTG_DCTL_CGONAK;
	}
	// TODO: ISO?				[stm32h7xx_hal_pcd.c] @1158
	O_EP->DOEPINT = USB_OTG_DOEPINT_EPDISD;
}
static inline void /**/ OEP_setup_done(USB_handle_t* handle, USB_OEP_t* oep) {
	O_EP->DOEPINT = USB_OTG_DOEPINT_STUP;
	// TODO: CORE VERISON/DMA?	[stm32h7xx_hal_pcd.c] @2328
	uint8_t* header = handle->setup;
	setup_header_t* req = &handle->request;

	*(uint8_t*)req =	*header++;  // request_type
	req->command =		*header++;
	req->value =		(*header++ << 8U) | *header++;
	req->index =		(*header++ << 8U) | *header++;
	req->length =		(*header++ << 8U) | *header;

	handle->EP0_state = EP_SETUP;
	handle->EP0_data = req->length;	// TODO: USE! (or needed??)
	switch (req->recipiant) {
	case RECIPIANT_DEVICE:		device_setup_request(handle);		break;
	case RECIPIANT_INTERFACE:	interface_setup_request(handle);	break;
	case RECIPIANT_ENDPOINT:	endpoint_setup_request(handle);		break;
	default:
		if (req->direction)	{ stall_IEP(handle, 0); }
		else				{ stall_OEP(handle, 0); }
		break;
	}
	// TODO: CORE VERISON/DMA?	[stm32h7xx_hal_pcd.c] @2341
}

/*!< IRQ handleing */
static inline void /**/ IEP_common_handler(USB_handle_t* handle, uint8_t ep_num) {
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
	I_EP->DIEPINT = USB_OTG_DIEPINT_TOC;				// clear timeout condition interrupt
	/* IN token received when TX FIFO is empty interrupt */
	I_EP->DIEPINT = USB_OTG_DIEPINT_ITTXFE;				// clear IN token received when TX FIFO empty interrupt
	/* IN token recieved with EP mismatch interrupt */
	I_EP->DIEPINT = USB_OTG_DIEPINT_INEPNM;				// clear IN token EP mismatch interrupt
	/* IN enpoint NAK effective interrupt */
	I_EP->DIEPINT = USB_OTG_DIEPINT_INEPNE;				// clear IN endpoint NAK effective interrupt
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
static inline void /**/ OEP_common_handler(USB_handle_t* handle, uint8_t ep_num) {
	USB_OEP_t	*oep =		handle->OEP[ep_num];
	uint32_t	ep_int =	O_EP->DOEPINT & DEV->DOEPMSK;	// get all triggered interrupts

	/* transfer complete interrupt */
	if (ep_int & USB_OTG_DOEPINT_XFRC)					{ OEP_transfer_complete(handle, oep, ep_num); }
	/* endpoint disabled interrupt */
	if (ep_int & USB_OTG_DOEPINT_EPDISD)				{ OEP_disabled(handle, oep); }
	/* AHB error interrupt */							// TODO: DMA?
	O_EP->DOEPINT = USB_OTG_DOEPINT_AHBERR;				// clear AHB error interrupt
	/* SETUP phase done */
	if (ep_int & USB_OTG_DOEPINT_STUP)					{ OEP_setup_done(handle, oep); }
	/* OUT token received when endpoint disabled interrupt */
	O_EP->DOEPINT = USB_OTG_DOEPINT_OTEPDIS;			// clear OUT token received when endpoint disabled interrupt
	/* status phase received for control write */
	O_EP->DOEPINT = USB_OTG_DOEPINT_OTEPSPR;			// clear status phase received for control write
	/* back to back setup packet recived interrupt */
	O_EP->DOEPINT = USB_OTG_DOEPINT_B2BSTUP;			// clear back to back setup packet received interrupt
	/* OUT packet error interrupt */
	O_EP->DOEPINT = USB_OTG_DOEPINT_OUTPKTERR;			// clear OUT packet error interrupt
	/* buffer not available interrupt */				// TODO: DMA?
	O_EP->DOEPINT = USB_OTG_DIEPINT_BNA;				// clear buffer not available interrupt
	/* babble error interrupt */
	O_EP->DOEPINT = USB_OTG_DOEPINT_BERR;				// clear babble error interrupt
	/* NAK interrupt */
	O_EP->DOEPINT = USB_OTG_DOEPINT_NAK;				// clear NAK interrupt
	/* NYET interrupt */								// TODO: HS?
	O_EP->DOEPINT = USB_OTG_DOEPINT_NYET;				// clear NYET interrupt
	/* setup packet received interrupt */				// TODO: DMA?
	O_EP->DOEPINT = USB_OTG_DOEPINT_STPKTRX;			// clear setup packet received interrupt
}
static /*  */ void /**/ USB_common_handler(USB_handle_t* handle) {
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
	if (irqs & USB_OTG_GINTSTS_SRQINT) {
		__NOP();
	}
	//USB->GINTSTS = USB_OTG_GINTSTS_SRQINT;				// clear connection event interrupt TODO: create flag?
	/* wake-up interrupt */								// TODO: LPM?
	USB->GINTSTS = USB_OTG_GINTSTS_WKUINT;				// clear wake-up interrupt
}


/*!<
 * interrupts
 * */
extern void OTG_HS_IRQHandler(void) { USB_common_handler(USB_handle[0]); }
extern void OTG_FS_IRQHandler(void) { USB_common_handler(USB_handle[1]); }

#endif // USB_LIB


/// NOTES
// from OTG_common_iep_handler -> XFRC
// TODO: EP0!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// TODO: EP0 should never have a XFRC interrupt because EP0_IN is only used as a response to a setup token not an IN token
// TODO: it the code for it in USBD_LL_DataInStage will be called from USBD_CtlSendData which will eventually be called from PCD_EP_OutSetupPacket_int
