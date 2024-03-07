//
// Created by marijn on 3/4/24.
//

#include "usb/usb.h"


#ifdef USB_LIB
/*!<
 * imported functions
 * */
extern void open_IEP(USB_handle_t* handle, uint8_t ep_num, uint16_t mps, EP_type_t type);
extern void open_OEP(USB_handle_t* handle, uint8_t ep_num, uint16_t mps, EP_type_t type);
extern void open_EP(USB_handle_t* handle, uint8_t ep_num, uint16_t mps, EP_type_t type);
extern void close_IEP(USB_handle_t* handle, uint8_t ep_num);
extern void close_OEP(USB_handle_t* handle, uint8_t ep_num);
extern void close_EP(USB_handle_t* handle, uint8_t ep_num);


/*!<
 * variables
 * */
extern USB_handle_t* USB_handle[2];


/*!<
 * HID classes
 * */
/*!< variables */
USB_class_t*	HID_keyboard = NULL;


/*!< functions */
void HID_keyboard_init(USB_handle_t* handle) {
	// TODO: state? @381 [usbd_hid_keyboard.c]
	open_IEP(handle, HID_keyboard->iep_num, HID_KEYBOARD_MPS, EP_TYPE_INTERRUPT);
	// TODO: state? @406 [usbd_hid_keyboard.c]
}
void HID_keyboard_de_init(USB_handle_t* handle) {
	close_IEP(handle, HID_keyboard->iep_num);
}
void HID_keyboard_setup(USB_handle_t* handle) {
	// TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}
void HID_keyboard_IN(USB_handle_t* handle, uint8_t epnum) {
	// TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}


/*!< TODO
 * mouse
 */

/*!< TODO
 * custom HID
 * */


/*!<
 * HID common functions
 */
void* write_HID_descriptor(void* ptr, uint16_t release_number, uint8_t country_code, uint16_t HID_report_size) {
	*((uint8_t*)ptr) = HID_DESCRIPTOR_SIZE;			ptr++;	// descriptor_size
	*((uint8_t*)ptr) = HID_DESCRIPTOR_TYPE;			ptr++;	// descriptor_type
	*((uint8_t*)ptr) = release_number & 0xFFU;		ptr++;	// release_number (LS)
	*((uint8_t*)ptr) = release_number >> 0x8U;		ptr++;	// release_number (MS)
	*((uint8_t*)ptr) = country_code;				ptr++;	// country_code
	*((uint8_t*)ptr) = 0x01U;						ptr++;	// following HID classes
	*((uint8_t*)ptr) = HID_REPORT_DESCRIPTOR_TYPE;	ptr++;	// descriptor_type
	*((uint8_t*)ptr) = HID_report_size & 0xFFU;		ptr++;	// power_type
	*((uint8_t*)ptr) = HID_report_size >> 0x8U;		ptr++;	// power
	return ptr;
}



/*!< TODO
 * mass storage device
 * */


/*!< TODO
 * composite
 * */


/*!<
 * common
 * */

void* create_string_descriptor(void* str) {
	uint8_t len = strlen(str);
	len = ((len > 0x7EU ? 0x7EU : len) << 0b1U) + 0x2U;		// set max str len to 126
	void* desc =	malloc(len); void* ptr =	desc;
	*((uint8_t*)ptr) = len;							ptr++;	// descriptor_size
	*((uint8_t*)ptr) = USB_string_descriptor_type;	ptr++;	// descriptor_type
	while (*((uint8_t*)str)) {
		*((uint8_t*)ptr) = *((uint8_t*)str++);		ptr++;
		*((uint8_t*)ptr) = 0x00;					ptr++;
	}
	return desc;
}

/*!< write_descriptor
 *	type -> USB_device_descriptor_type:		... -> (
 *		uint16_t			USB_version,
 *		uint8_t				device_class,				// TODO: define??
 *		uint8_t				device_sub_class,			// TODO: define??
 *		uint8_t				device_protocol,
 *		uint8_t				EP0_max_packet_size,
 *		uint16_t			vendor_ID,
 *		uint16_t			product_ID,
 *		uint16_t			device_version,
 *		uint8_t				manufacturer_string_index,
 *		uint8_t				product_string_index,
 *		uint8_t				serial_string_index,
 *		uint8_t				configuration_count
 *	)
 *	type -> USB_config_descriptor_type:		... -> (
 *		uint16_t			total_size,
 *		uint8_t				interface_count,
 *		uint8_t				configuration_value,		// TODO?
 *		uint8_t				string_descriptor_index,
 *		USB_power_type_t	power_type,
 *		uint8_t				power						// in increments of 2mA
 *	)
 *	type -> USB_string_descriptor_type:		... -> (
 *		void*				str,
 *		uint8_t				size						// manually set size to allow UTF-16 strings
 *	) ->	// keep in mind that size = (strlen(str) * 2 + 2)
 *	type -> USB_interface_descriptor_type:	... -> (
 *		uint8_t				interface_number,
 *		uint8_t				alternate_setting,			// TODO?
 *		uint8_t				endpoint_count,
 *		uint8_t				interface_class,			// TODO: define!!!!!!!!!!!!!!!!!!
 *		uint8_t				interface_sub_class,		// 0x00 -> no boot, 0x01 -> boot TODO?
 *		uint8_t				interface_protocol,			// TODO: define!!!!!!!!!!!!!!!!!!
 *		uint8_t				string_descriptor_index
 *	)
 *	type -> USB_endpoint_descriptor_type:	... -> (
 *		uint8_t				endpoint,
 *		EP_type_t			type,
 *		uint16_t			max_packet_size,
 *		uint8_t				polling_interval
 *	)
 * TODO: USB_qualifier_descriptor_type:		... -> ()
 * TODO: USB_other_speed_descriptor_type:	... -> ()
 * TODO: USB_IAD_descriptor_type:			... -> ()
 * TODO: USB_BOS_descriptor_type:			... -> ()
 */
void* write_descriptor(void* ptr, USB_descriptor_type_t type, ...) {
	uint32_t* args = &type + sizeof(void*);

	switch (type) {
	case USB_device_descriptor_type:;
		uint16_t USB_version = *((uint16_t*)args);			args++;
		*((uint8_t*)ptr) = USB_config_descriptor_size;		ptr++;	// descriptor_size
		*((uint8_t*)ptr) = USB_config_descriptor_type;		ptr++;	// descriptor_type
		*((uint8_t*)ptr) = USB_version & 0xFFU;				ptr++;	// USB_version (LS)
		*((uint8_t*)ptr) = USB_version >> 0x8U;				ptr++;	// USB_version (MS)
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// device_class
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// device_sub_class
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// device_protocol
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// EP0_max_packet_size
		uint16_t vendor_ID = *((uint16_t*)args);			args++;
		uint16_t product_ID = *((uint16_t*)args);			args++;
		uint16_t device_version = *((uint16_t*)args);		args++;
		*((uint8_t*)ptr) = vendor_ID & 0xFFU;				ptr++;	// vendor_ID (LS)
		*((uint8_t*)ptr) = vendor_ID >> 0x8U;				ptr++;	// vendor_ID (MS)
		*((uint8_t*)ptr) = product_ID & 0xFFU;				ptr++;	// product_ID (LS)
		*((uint8_t*)ptr) = product_ID >> 0x8U;				ptr++;	// product_ID (MS)
		*((uint8_t*)ptr) = device_version & 0xFFU;			ptr++;	// device_version (LS)
		*((uint8_t*)ptr) = device_version >> 0x8U;			ptr++;	// device_version (MS)
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// manufacturer_string_index
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// product_string_index
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// serial_string_index
		*((uint8_t*)ptr) = *((uint8_t*)args);				ptr++;	// configuration_count
		return ptr;
	case USB_config_descriptor_type:;
		uint16_t total_size = *((uint16_t*)args);			args++;
		*((uint8_t*)ptr) = USB_config_descriptor_size;		ptr++;	// descriptor_size
		*((uint8_t*)ptr) = USB_config_descriptor_type;		ptr++;	// descriptor_type
		*((uint8_t*)ptr) = total_size & 0xFFU;				ptr++;	// total_size (LS)
		*((uint8_t*)ptr) = total_size >> 0x8U;				ptr++;	// total_size (MS)
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// interface_count
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// configuration_value
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// string_descriptor_index
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// power_type
		*((uint8_t*)ptr) = *((uint8_t*)args);				ptr++;	// power
		return ptr;
	case USB_interface_descriptor_type:
		*((uint8_t*)ptr) = USB_interface_descriptor_size;	ptr++;	// descriptor_size
		*((uint8_t*)ptr) = USB_interface_descriptor_type;	ptr++;	// descriptor_type
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// interface_number
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// alternate_setting
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// endpoint_count
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// interface_class
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// interface_sub_class
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// interface_protocol
		*((uint8_t*)ptr) = *((uint8_t*)args);				ptr++;	// string_descriptor_index
		return ptr;
	case USB_endpoint_descriptor_type:;
		*((uint8_t*)ptr) = USB_endpoint_descriptor_size;	ptr++;	// descriptor_size
		*((uint8_t*)ptr) = USB_endpoint_descriptor_type;	ptr++;	// descriptor_type
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// endpoint
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;		ptr++;	// type
		uint16_t mps = *((uint16_t*)args);		args++;
		*((uint8_t*)ptr) = mps & 0xFFU;						ptr++;	// max_packet_size (LS)
		*((uint8_t*)ptr) = mps >> 0x8U;						ptr++;	// max_packet_size (MS)
		*((uint8_t*)ptr) = *((uint8_t*)args);				ptr++;	// polling_interval
		return ptr;
	case USB_qualifier_descriptor_type:		// TODO: HS?
	case USB_other_speed_descriptor_type:	// TODO: HS?
	case USB_IAD_descriptor_type:			// TODO: ISO?
	case USB_BOS_descriptor_type:			// TODO: LPM?
	default: return NULL;  // error
	}
}

USB_handle_t* fconfig_USB_handle(USB_OTG_GlobalTypeDef* usb, USB_class_type_t class_type, uint8_t iep_num, uint8_t oep_num, uint32_t descriptor_size) {
	uint8_t						handle_index =	((uint32_t)usb >> 0x13UL) & 0b1UL;
	USB_handle_t				*handle =		USB_handle[handle_index];
	USB_OTG_DeviceTypeDef		*device =		(void*)((uint32_t)usb + 0x800);
	USB_OTG_INEndpointTypeDef	*in =			(void*)((uint32_t)usb + 0x900);
	USB_OTG_OUTEndpointTypeDef	*out =			(void*)((uint32_t)usb + 0xB00);
	if (!handle) {  // create status if not available
		USB_handle[handle_index] = handle = malloc(sizeof(USB_handle_t));
	} else {
		if (handle->descriptor) {
			if(handle->descriptor->device)					{ free(handle->descriptor->device); }
			if(handle->descriptor->lang_ID_string)			{ free(handle->descriptor->lang_ID_string); }
			if(handle->descriptor->manufacturer_string)		{ free(handle->descriptor->manufacturer_string); }
			if(handle->descriptor->product_string)			{ free(handle->descriptor->product_string); }
			if(handle->descriptor->serial_string)			{ free(handle->descriptor->serial_string); }
			if(handle->descriptor->configuration_string)	{ free(handle->descriptor->configuration_string); }
			if(handle->descriptor->interface_string)		{ free(handle->descriptor->interface_string); }
			free(handle->descriptor);
		} if (handle->class) {
			if (handle->class->descriptor)					{ free(handle->class->descriptor); }
			free(handle->class);
		}
		for (uint8_t i = 0; i < 16U; i++) {
			if (handle->IEP[i]) { free(handle->IEP[i]); }
			if (handle->OEP[i]) { free(handle->OEP[i]); }
		}
	}
	USB_class_t*			class =			malloc(sizeof(USB_class_t));
	USB_descriptor_t*		descriptor =	malloc(sizeof(USB_descriptor_t));
	handle->class =			class;
	handle->descriptor =	descriptor;

	handle->usb =			usb;
	handle->device =		device;
	USB_IEP_t* iep; USB_OEP_t* oep;
 	for (uint8_t i = 0; i < USB_OTG_ENDPOINT_COUNT; i++) {
		handle->IEP[i] = iep = malloc(sizeof(USB_IEP_t));
 		iep->mps = iep->size = iep->count =	0x00000000UL;
 		iep->type = iep->stall = iep->used =		0b0U;
 		iep->ep =		&in[i]; iep->src =			NULL;
 		handle->OEP[i] = oep = malloc(sizeof(USB_OEP_t));
 		oep->mps = oep->size = oep->count =	0x00000000UL;
 		oep->type = oep->stall = oep->used =		0b0U;
 		oep->ep =		&out[i]; oep->dest =		NULL;
 	}

	// init handle / ep config  TODO: redo!!!!!!
	class->handle =		handle;
	class->iep_num =	iep_num;
	class->oep_num =	oep_num;
	// init class functions and descriptors
	switch (class_type) {
		case USB_CLASS_HID_KEYBOARD:
			HID_keyboard =				class;
			class->init =				HID_keyboard_init;
			class->de_init =			HID_keyboard_de_init;
			class->setup =				HID_keyboard_setup;
			class->IEP0_sent =			NULL;
			class->OEP0_ready =			NULL;
			class->data_in =			HID_keyboard_IN;
			class->data_out =			NULL;
			class->SOF =				NULL;

			class->descriptor =			malloc(descriptor_size);
			class->descriptor_size =	descriptor_size;
			break;
		// TODO: other classes!!!!!!!!!!!!!!!!!!!!!!!!!!
		default:
			free(class);
			handle->class = NULL;
	}

	descriptor->device =				malloc(USB_device_descriptor_size);
	descriptor->lang_ID_string =		NULL;
	descriptor->manufacturer_string =	NULL;
	descriptor->product_string =		NULL;
	descriptor->serial_string =			NULL;
	descriptor->configuration_string =	NULL;
	descriptor->interface_string =		NULL;

	return handle;
}


#endif // USB_LIB