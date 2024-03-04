//
// Created by marijn on 3/4/24.
//

#include "usb/usb.h"


#ifdef USB_LIB

/*!<
 * types
 * */
typedef enum {
	USB_CLASS_COMPOSITE =		0x0U,
	USB_CLASS_HID_KEYBOARD =	0x1U,
	USB_CLASS_HID_MOUSE =		0x2U,
	USB_CLASS_HID_CUSTOM =		0x4U,
	USB_CLASS_MSC =				0x8U
} USB_class_type_t;


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
#define HID_KEYBOARD_MPS				0x08U
#define HID_KEYBOARD_DESCRIPTOR_SIZE	0x22U

/*!< variables */
USB_class_t*	HID_keyboard = NULL;
uint8_t			HID_keyboard_descriptor[HID_KEYBOARD_DESCRIPTOR_SIZE] = {
	// TODO!!!!!!!
};

/*
__ALIGN_BEGIN static uint8_t USBD_HID_KEYBOARD_CfgFSDesc[HID_KEYBOARD_CONFIG_DESC_SIZE] __ALIGN_END =
{
	0x09,                          /* bLength: Configuration Descriptor size
	USB_DESC_TYPE_CONFIGURATION,   /* bDescriptorType: Configuration
	HID_KEYBOARD_CONFIG_DESC_SIZE, /* wTotalLength: Bytes returned
	0x00,
	0x01, /* bNumInterfaces: 1 interface
	0x01, /* bConfigurationValue: Configuration value
	0x00, /* iConfiguration: Index of string descriptor describing the configuration
	#if (USBD_SELF_POWERED == 1U)
	0xE0, /* bmAttributes: Bus Powered according to user configuration
	#else
	0xA0, /* bmAttributes: Bus Powered according to user configuration
	#endif
	USBD_MAX_POWER, /* MaxPower 100 mA: this current is used for detecting Vbus

	/************** Descriptor of Keyboard interface ***************
	/* 09
	0x09,                       /* bLength: Interface Descriptor size
	USB_DESC_TYPE_INTERFACE,    /* bDescriptorType: Interface descriptor type
	_HID_KEYBOARD_ITF_NBR,      /* bInterfaceNumber: Number of Interface
	0x00,                       /* bAlternateSetting: Alternate setting
	0x01,                       /* bNumEndpoints
	0x03,                       /* bInterfaceClass: HID
	0x01,                       /* bInterfaceSubClass : 1=BOOT, 0=no boot
	0x01,                       /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse
	_HID_KEYBOARD_STR_DESC_IDX, /* iInterface: Index of string descriptor
	/******************** Descriptor of Keyboard HID *******************
	/* 18
	0x09,                         /* bLength: HID Descriptor size
	HID_KEYBOARD_DESCRIPTOR_TYPE, /* bDescriptorType: HID
	0x11,                         /* bcdHID: HID Class Spec release number
	0x01,
	0x00,                          /* bCountryCode: Hardware target country
	0x01,                          /* bNumDescriptors: Number of HID class descriptors to follow
	0x22,                          /* bDescriptorType
	HID_KEYBOARD_REPORT_DESC_SIZE, /* wItemLength: Total length of Report descriptor
	0x00,
	/******************** Descriptor of Keyboard endpoint *******************
	/* 27
	0x07,                   /* bLength: Endpoint Descriptor size
	USB_DESC_TYPE_ENDPOINT, /* bDescriptorType:
	_HID_KEYBOARD_IN_EP,    /* bEndpointAddress: Endpoint Address (IN)
	0x03,                   /* bmAttributes: Interrupt endpoint
	HID_KEYBOARD_EPIN_SIZE, /* wMaxPacketSize: 4 Byte max
	0x00,
	HID_KEYBOARD_FS_BINTERVAL, /* bInterval: Polling Interval
	                       /* 34
};
*/


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