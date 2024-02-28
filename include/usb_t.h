//
// Created by marijn on 2/13/24.
//

#ifndef USB_T_H
#define USB_T_H
#include "usb.h"


/*!<
 * defines
 * */
#define DEVICE_FEATURE_WAKE_HOST			0x01U

#define DEVICE_CONFIG_SELF_POWERED			0x01U
#define DEVICE_CONFIG_WAKE_HOST				0x02U

#define USB_DEVICE_DESCRIPTOR				0x01U
#define USB_CONFIGURATION_DESCRIPTOR		0x02U
#define USB_STRING_DESCRIPTOR				0x03U
#define USB_INTERFACE_DESCRIPTOR			0x04U
#define USB_ENDPOINT_DESCRIPTOR				0x05U
#define USB_QUALIFIER_DESCRIPTOR			0x06U
#define USB_OTHER_SPEED_DESCRIPTOR			0x07U
// #define USB_DEBUG_DESCRIPTOR				0x0AU
#define USB_IAD_DESCRIPTOR					0x0BU
#define USB_BOS_DESCRIPTOR					0x0FU

#define USB_LANG_ID_STRING_DESCRIPTOR		0x00U
#define USB_MANUFACTURER_STRING_DESCRIPTOR	0x01U
#define USB_PRODUCT_STRING_DESCRIPTOR		0x02U
#define USB_SERIAL_STRING_DESCRIPTOR		0x03U
#define USB_CONFIG_STRING_DESCRIPTOR		0x04U
#define USB_INTERFACE_STRING_DESCRIPTOR		0x05U

/*!<
 * types
 * */
typedef enum {
	USB_DEFAULT =			0b00,
	USB_ADDRESSED =			0b01,
	USB_CONFIGURED =		0b10,
	USB_SUSPENDED =			0b11
} USB_state_t;

typedef enum {
	EP_IDLE =				0b000,
	EP_SETUP =				0b001,
	EP_DATA_IN =			0b010,
	EP_DATA_OUT =			0b011,
	EP_STATUS_IN =			0b100,
	EP_STATUS_OUT =			0b101
} EP_state_t;

typedef enum {
	EP_TYPE_CONTROL =		0b00,
	EP_TYPE_ISOCHRONOUS =	0b01,
	EP_TYPE_BULK =			0b10,
	EP_TYPE_INTERRUPT =		0b11
} EP_type_t;

typedef enum {
	STANDARD_REQUEST =		0b00,
	CLASS_REQUEST =			0b01,
	VENDOR_REQUEST =		0b10
} SETUP_request_type_t;

typedef enum {
	RECIPIANT_DEVICE =		0b00000,
	RECIPIANT_INTERFACE =	0b00001,
	RECIPIANT_ENDPOINT =	0b00010
} SETUP_recipiant_t;

typedef enum {
	GET_STATUS =			0x00,						// 2B read
	CLEAR_FEATURE =			0x01,						// clear feature addressed by "value"
	SET_FEATURE =			0x03,						// set feature addressed by "value"
	SET_ADDRESS =			0x05,						// set address in "value"
	GET_DESCRIPTOR =		0x06,						// [length]B read, descriptor type and index in "value"
	SET_DESCRIPTOR =		0x07,						// [length]B write, descriptor type and index in "value"
	GET_CONFIGURATION =		0x08,						// 1B read
	SET_CONFIGURATION =		0x09,						// config addressed by "value"
} SETUP_command_t;

typedef struct {
	SETUP_recipiant_t			recipiant	: 5;		// |
	SETUP_request_type_t		type		: 2;		// | bmRequest
	uint8_t						direction	: 1;		// |  // TODO: MSB?!!!!!!!!!!!!!!!!!!
	SETUP_command_t				command;				// bRequest
	uint16_t					value;					// wValue
	uint16_t					index;					// wIndex
	uint16_t					length;					// wLength
} setup_header_t;

typedef struct {
	void (*init)		(void* handle);					// TODO: init
	void (*de_init)		(void* handle);					// TODO: init
	void (*setup)		(void* handle);					// TODO: init
	void (*data_in)		(void* handle, uint8_t epnum);	// TODO: init
	void (*data_out)	(void* handle, uint8_t epnum);	// TODO: init
	void (*SOF)			(void* handle);					// TODO: needed??
	void (*IEP0_sent)	(void* handle);					// TODO: needed??
	void (*OEP0_ready)	(void* handle);					// TODO: needed??

	uint8_t*					descriptor;				// TODO: init
	uint16_t					descriptor_size;		// TODO: init (variable size)
	//uint8_t*					qualifier;				// qualifier descriptor TODO: HS?
} USB_class_t;  // always composite!

typedef struct {
	USB_OTG_INEndpointTypeDef*	ep;

	uint32_t					mps;					// max packet size

	void*						src;					// TX buffer source (buffer + count)
	uint32_t					size;					// to send
	uint32_t					count;					// sent

	// TODO: consider saving epnum
	uint8_t						type	: 2;			// endpoint type
	uint8_t						stall	: 1;			// stall condition
	uint8_t						used	: 1;
} USB_IEP_t;  // (host) in endpoint status type
// TODO: consider combining types (or abstract type)
typedef struct {
	USB_OTG_OUTEndpointTypeDef*	ep;

	uint32_t					mps;					// max packet size

	void*						dest;					// RX buffer destination (buffer + count)
	uint32_t					size;					// RX buffer size
	uint32_t					count;					// received

	// TODO: consider saving epnum
	uint8_t						type	: 2;			// endpoint type
	uint8_t						stall	: 1;			// stall condition
	uint8_t						used	: 1;
} USB_OEP_t;  // (host) out endpoint status type
// TODO: consider combining types (or abstract type)

typedef struct {
	// physical device handles
	USB_OTG_GlobalTypeDef*		usb;
	USB_OTG_DeviceTypeDef*		device;
	// class and EP structs
	USB_class_t*				class;					// TODO: init
	USB_IEP_t*					IEP[16];				// TODO: init
	USB_OEP_t*					OEP[16];				// TODO: init
	// setup variables
	uint32_t					setup[16];				// max controll stage data size: 64B TODO: HAL has 12???
	setup_header_t				request;				// TODO: in struct??
	// states and (volatile) settings
	USB_state_t					state		: 2;
	EP_state_t					EP0_state	: 3;
	uint16_t					address		: 7;
	uint16_t					class_init	: 1;
	uint32_t					EP0_data;				// TODO: USE! (or needed??)
	uint8_t						device_config;			// response sent on GET_STATUS request
	// descriptors
	uint8_t*					descriptor;				// TODO: init
	uint8_t*					lang_ID_string;			// TODO: init (see: USBD_GetString)
	uint8_t*					manufacturer_string;	// TODO: init
	uint8_t*					product_string;			// TODO: init
	uint8_t*					serial_string;			// TODO: init
	uint8_t*					configuration_string;	// TODO: init
	uint8_t*					user_string;			// TODO: init
	uint8_t*					interface_string;		// TODO: init
	// config TODO:
	// * IEP, OEP
	// * status_config

	// TODO: USBD_Desc
	// TODO always use!!!: USBD_COMPOSITE
	// classes / interfaces ??
} USB_handle_t;

// TODO: move these definitions and all functions that use them from usb.c to usb_int.c
// OR: clean it up significantly and expose it to the user
#endif //USB_T_H
