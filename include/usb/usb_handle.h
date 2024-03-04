//
// Created by marijn on 3/4/24.
//

#ifndef USB_HANDLE_H
#define USB_HANDLE_H
#include "main.h"


#ifdef USB_LIB
/*!<
 * defines
* */
#define USB (handle->usb)
#define DEV (handle->device)
#define I_EP (iep->ep)
#define O_EP (oep->ep)


/*!<
 * types
 * */
typedef enum {
	USB_DEFAULT =			0b00,
	USB_ADDRESSED =			0b01,
	USB_CONFIGURED =		0b10,
	USB_SUSPENDED =			0b11
}	USB_state_t;

typedef enum {
	EP_IDLE =				0b000,
	EP_SETUP =				0b001,
	EP_DATA_IN =			0b010,
	EP_DATA_OUT =			0b011,
	EP_STATUS_IN =			0b100,
	EP_STATUS_OUT =			0b101
}	EP_state_t;

typedef enum {
	EP_TYPE_CONTROL =		0b00,
	EP_TYPE_ISOCHRONOUS =	0b01,
	EP_TYPE_BULK =			0b10,
	EP_TYPE_INTERRUPT =		0b11
}	EP_type_t;

typedef enum {
	STANDARD_REQUEST =		0b00,
	CLASS_REQUEST =			0b01,
	VENDOR_REQUEST =		0b10
}	SETUP_request_type_t;

typedef enum {
	RECIPIANT_DEVICE =		0b00000,
	RECIPIANT_INTERFACE =	0b00001,
	RECIPIANT_ENDPOINT =	0b00010
}	SETUP_recipiant_t;

typedef enum {
	GET_STATUS =			0x00,						// 2B read
	CLEAR_FEATURE =			0x01,						// clear feature addressed by "value"
	SET_FEATURE =			0x03,						// set feature addressed by "value"
	SET_ADDRESS =			0x05,						// set address in "value"
	GET_DESCRIPTOR =		0x06,						// [length]B read, descriptor type and index in "value"
	SET_DESCRIPTOR =		0x07,						// [length]B write, descriptor type and index in "value"
	GET_CONFIGURATION =		0x08,						// 1B read
	SET_CONFIGURATION =		0x09,						// config addressed by "value"
}	SETUP_command_t;

typedef struct {
	/*!< init */
	void (*init)		(void* handle);					// TODO: init
	void (*de_init)		(void* handle);					// TODO: init
	/*!< control */
	void (*setup)		(void* handle);					// TODO: init
	void (*IEP0_sent)	(void* handle);					// TODO: needed??
	void (*OEP0_ready)	(void* handle);					// TODO: needed??
	/*!< usage */
	void (*data_in)		(void* handle, uint8_t epnum);	// TODO: init
	void (*data_out)	(void* handle, uint8_t epnum);	// TODO: init
	void (*SOF)			(void* handle);					// TODO: needed??

	/*!< handle / config */
	void*				handle;							// USB_handle_t*
	uint8_t				iep_num;
	uint8_t				oep_num;

	/*!< descriptors */
	uint8_t*			descriptor;						// TODO: init
	uint16_t			descriptor_size;				// TODO: init (variable size)

	//uint8_t*			(*user_string)(uint8_t index);	// TODO: implement, init
	//uint8_t*			HS_descriptor;					// TODO: HS?
	//uint16_t			HS_descriptor_size;				// TODO: init (variable size)
	//uint8_t*			other_speed;					// other_speed descriptor TODO: HS?
	//uint8_t*			qualifier;						// qualifier descriptor TODO: HS?
}	USB_class_t;

typedef struct {
	uint8_t*					device;					// TODO: init
	uint8_t*					lang_ID_string;			// TODO: init (see: USBD_GetString)
	uint8_t*					manufacturer_string;	// TODO: init
	uint8_t*					product_string;			// TODO: init
	uint8_t*					serial_string;			// TODO: init
	uint8_t*					configuration_string;	// TODO: init
	uint8_t*					interface_string;		// TODO: init

	//uint8_t*					(*user_string)(uint8_t index);	// TODO: implement, init
	//uint8_t*					BOS_string;		// TODO: init

	// TODO: USBD_Desc
}	USB_descriptor_t;

typedef struct {
	SETUP_recipiant_t			recipiant	: 5;		// |
	SETUP_request_type_t		type		: 2;		// | bmRequest
	uint8_t						direction	: 1;		// |  // TODO: MSB?!!!!!!!!!!!!!!!!!!
	SETUP_command_t				command;				// bRequest
	uint16_t					value;					// wValue
	uint16_t					index;					// wIndex
	uint16_t					length;					// wLength
}	setup_header_t;

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
}	USB_IEP_t;

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
}	USB_OEP_t;

typedef struct {
	// physical device handles
	USB_OTG_GlobalTypeDef*		usb;
	USB_OTG_DeviceTypeDef*		device;
	// class and EP structs
	USB_descriptor_t*			descriptor;				// TODO: init
	USB_class_t*				class;					// TODO: init
	USB_IEP_t*					IEP[16];
	USB_OEP_t*					OEP[16];
	// setup variables
	uint32_t					setup[16];				// max controll stage data size: 64B TODO: HAL has 12???
	setup_header_t				request;				// TODO: in struct??
	// states and (volatile) settings
	USB_state_t					state		: 2;
	EP_state_t					EP0_state	: 3;
	uint16_t					address		: 7;
	uint16_t					class_init	: 1;
	uint32_t					EP0_data;
	uint8_t						device_config;			// response sent on GET_STATUS request
}	USB_handle_t;


#endif // USB_LIB
#endif // USB_HANDLE_H
