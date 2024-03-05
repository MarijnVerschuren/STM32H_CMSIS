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
 * keyboard
 * */
#ifdef USB_CLASS_HID_KEYBOARD_LIB
/*!< defines */
#define HID_KEYBOARD_MPS						0x08U
#define HID_KEYBOARD_INTERVAL					0x0AU

#define HID_KEYBOARD_DESCRIPTOR_SIZE			0x22U	/* 34 */
#define HID_KEYBOARD_REPORT_DESCRIPTOR_SIZE		0xBBU	/* 187 */

#define HID_KEYBOARD_DESCRIPTOR_TYPE			0x21U
#define HID_KEYBOARD_REPORT_DESCRIPTOR_TYPE		0x22U

/*!< variables */
USB_class_t*	HID_keyboard = NULL;
uint8_t			HID_keyboard_descriptor[HID_KEYBOARD_DESCRIPTOR_SIZE] = {
	/*!< configuration descriptor */			// base + 0x00U
	0x09U,								    // configuration descriptor size
	USB_config_descriptor_type,				// set descriptor type to configuration
	HID_KEYBOARD_DESCRIPTOR_SIZE,		    // | total size (LS byte)
	0x00U,								    // |			(MS byte)
	0x01U,								    // interface count
	0x01U,								    // configuration value
	0x00U,								    // corresponding string descriptor index TODO
	0xA0U,	/* 0xE0 -> self powered */	    // bus powered
	0x32U,									// power: 100mA TODO stm draws more!!!!
	/*!< keyboard interface descriptor */		// base + 0x09U
	0x09U,								    // interface descriptor size
	USB_interface_descriptor_type,		    // set descriptor type to interface
	0x00U,								    // interface number TODO
	0x00U,								    // alternate setting TODO?
	0x01U,								    // endpoint count
	0x03U,								    // set interface class to HID TODO: define?
	0x01U,	/* 0x00 -> NO BOOT */		    // set interface sub-class to BOOT TODO?
	0x01U,	/* 0 -> none, 2 -> mouse*/	    // set interface protocol to keyboard TODO: define?
	0x00U,								    // corresponding interface string descriptor index
	/*!< HID keyboard class descriptor */		// base + 0x12U
	0x09,								    // HID keyboard class descriptor size
	HID_KEYBOARD_DESCRIPTOR_TYPE,		    // set descriptor type to HID keyboard
	0x11,								    // HID class specification release number TODO
	0x01,								    // TODO?
	0x00,								    // country code
	0x01,								    // number of HID class descriptors to follow TODO multi HID function?????
	HID_KEYBOARD_REPORT_DESCRIPTOR_TYPE,	// set descriptor type to HID keyboard report
	HID_KEYBOARD_REPORT_DESCRIPTOR_SIZE,	// set HID keyboard report descriptor size
	0x00,									// TODO?
	/*!< endpoint descriptor */					// base + 0x1BU
	0x07,									// endpoint descriptor size
	USB_endpoint_descriptor_type,			// set descriptor type to endpoint
	0x81,									// IN endpoint 1		// TODO: settings (make functions for each descriptor type that build a descriptor)
	EP_TYPE_INTERRUPT,						// set endpoint type to interrupt
	HID_KEYBOARD_MPS,						// | max packet size	(LS byte)
	0x00,									// |					(HS byte)
	HID_KEYBOARD_INTERVAL					// set polling interval
};


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
#endif // USB_CLASS_HID_KEYBOARD_LIB


/*!< TODO
 * mouse
 */

/*!< TODO
 * custom HID
 * */

/*!< TODO
 * mass storage device
 * */


/*!< TODO
 * composite
 * */


/*!<
 * common
 * */
/*
0x12,                       /*bLength  *
USB_DESC_TYPE_DEVICE,       /*bDescriptorType *
0x00,                       /*bcdUSB  *
0x02,
0xEF,                       /*bDeviceClass *
0x02,                       /*bDeviceSubClass *
0x01,                       /*bDeviceProtocol *
USB_MAX_EP0_SIZE,           /*bMaxPacketSize *
LOBYTE(USBD_VID),           /*idVendor *
HIBYTE(USBD_VID),           /*idVendor *
LOBYTE(USBD_PID),        	  /*idProduct *
HIBYTE(USBD_PID),           /*idProduct *
0x00,                       /*bcdDevice rel. 2.00 *
0x02,
USBD_IDX_MFC_STR,           /*Index of manufacturer  string *
USBD_IDX_PRODUCT_STR,       /*Index of product string *
USBD_IDX_SERIAL_STR,        /*Index of serial number string *
USBD_MAX_NUM_CONFIGURATION  /*bNumConfigurations *
*/

/*!< write_descriptor
 * TODO: USB_device_descriptor_type:		... -> ()
 *	type -> USB_config_descriptor_type:		... -> (
 *		uint16_t			total_size,
 *		uint8_t				interface_count,
 *		uint8_t				configuration_value,		// TODO?
 *		uint8_t				string_descriptor_index,
 *		USB_power_type_t	power_type,
 *		uint8_t				power						// in increments of 2mA
 *	)
 * TODO: USB_string_descriptor_type:		... -> ()
 *	type -> USB_interface_descriptor_type:	... -> (
 *		uint8_t             interface_number,
 *		uint8_t             alternate_setting,			// TODO?
 *		uint8_t             endpoint_count,
 *		uint8_t             interface_class,			// TODO: define!!!!!!!!!!!!!!!!!!
 *		uint8_t             interface_sub_class,		// 0x00 -> no boot, 0x01 -> boot TODO?
 *		uint8_t             interface_protocol,			// TODO: define!!!!!!!!!!!!!!!!!!
 *		uint8_t             string_descriptor_index
 *	)
 *	type -> USB_endpoint_descriptor_type:	... -> (
 *		uint16_t	        max_packet_size,
 *		uint8_t		        endpoint,
 *		EP_type_t	        type,
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
	case USB_device_descriptor_type:
		break;
	case USB_config_descriptor_type:;
		uint16_t total_size = *((uint16_t*)args);		    args++;
		*((uint8_t*)ptr) = USB_config_descriptor_size;	    ptr++;	// descriptor_size
		*((uint8_t*)ptr) = USB_config_descriptor_type;	    ptr++;	// descriptor_type
		*((uint8_t*)ptr) = total_size & 0xFFU;			    ptr++;	// total_size (LS)
		*((uint8_t*)ptr) = total_size >> 0x8U;			    ptr++;	// total_size (MS)
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;	    ptr++;	// interface_count
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;	    ptr++;	// configuration_value
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;	    ptr++;	// string_descriptor_index
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;	    ptr++;	// power_type
		*((uint8_t*)ptr) = *((uint8_t*)args);			    ptr++;	// power
		return ptr;
	case USB_string_descriptor_type:
		break;
	case USB_interface_descriptor_type:
		*((uint8_t*)ptr) = USB_interface_descriptor_size;	ptr++;	// descriptor_size
		*((uint8_t*)ptr) = USB_interface_descriptor_type;	ptr++;	// descriptor_type
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;	    ptr++;	// interface_number
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;	    ptr++;	// alternate_setting
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;	    ptr++;	// endpoint_count
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;	    ptr++;	// interface_class
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;	    ptr++;	// interface_sub_class
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;	    ptr++;	// interface_protocol
		*((uint8_t*)ptr) = *((uint8_t*)args);			    ptr++;	// string_descriptor_index
		return ptr;
	case USB_endpoint_descriptor_type:;
		uint16_t mps = *((uint16_t*)args);				    args++;
		*((uint8_t*)ptr) = USB_endpoint_descriptor_size;	ptr++;	// descriptor_size
		*((uint8_t*)ptr) = USB_endpoint_descriptor_type;	ptr++;	// descriptor_type
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;	    ptr++;	// endpoint
		*((uint8_t*)ptr) = *((uint8_t*)args);	args++;	    ptr++;	// type
		*((uint8_t*)ptr) = mps & 0xFFU;					    ptr++;	// max_packet_size (LS)
		*((uint8_t*)ptr) = mps >> 0x8U;					    ptr++;	// max_packet_size (MS)
		*((uint8_t*)ptr) = *((uint8_t*)args);			    ptr++;	// polling_interval
		return ptr;
	case USB_qualifier_descriptor_type:
	case USB_other_speed_descriptor_type:
	default:
		break;
	}
}

USB_class_t* init_class(USB_handle_t* handle, USB_class_type_t class_type, uint8_t iep_num, uint8_t oep_num) {
	USB_class_t* class = malloc(sizeof(USB_class_t));
	// init handle / ep config
	class->handle =		handle;
	class->iep_num =	iep_num;
	class->oep_num =	oep_num;
	// init class functions and descriptors
	switch (class_type) {
		case USB_CLASS_COMPOSITE:	break;
		case USB_CLASS_HID_KEYBOARD:
		#ifdef USB_CLASS_HID_KEYBOARD_LIB
			HID_keyboard =		        class;
			class->init =		        HID_keyboard_init;
			class->de_init =	        HID_keyboard_de_init;
			class->setup =		        HID_keyboard_setup;
			class->IEP0_sent =	        NULL;
			class->OEP0_ready =	        NULL;
			class->data_in =	        HID_keyboard_IN;
			class->data_out =	        NULL;
			class->SOF =		        NULL;

			class->descriptor =		    HID_keyboard_descriptor;
			class->descriptor_size =	HID_KEYBOARD_DESCRIPTOR_SIZE;
			return class;
		#else // USB_CLASS_HID_KEYBOARD_LIB
			break;
		#endif // USB_CLASS_HID_KEYBOARD_LIB
		case USB_CLASS_HID_MOUSE:	break;
		case USB_CLASS_HID_CUSTOM:	break;
		case USB_CLASS_MSC:			break;
		default:					break;
	}
	// catch errors
	free(class);
	return NULL;
}


#endif // USB_LIB