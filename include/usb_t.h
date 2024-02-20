//
// Created by marijn on 2/13/24.
//

#ifndef USB_T_H
#define USB_T_H
#include "usb.h"

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

typedef struct {
	void (*data_in)(USB_OTG_GlobalTypeDef *usb, uint8_t epnum);
} USB_class_t;

typedef struct {
	USB_OTG_INEndpointTypeDef*	ep;
	USB_class_t*				class;

	uint32_t					packet_size;	// max packet size

	void*						buffer;			// TX buffer
	uint32_t					size;			// to send
	uint32_t					count;			// sent
} USB_IEP_t;  // (host) in endpoint status type

typedef struct {
	USB_OTG_OUTEndpointTypeDef*	ep;
	USB_class_t*				class;

	uint32_t					packet_size;	// max packet size

	void*						buffer;			// RX buffer
	uint32_t					size;			// RX buffer size
	uint32_t					count;			// received
} USB_OEP_t;  // (host) out endpoint status type


typedef struct {
	USB_OTG_GlobalTypeDef*		usb;
	USB_OTG_DeviceTypeDef*		device;
	// TODO: host?
	USB_IEP_t*					IEP[16];
	USB_OEP_t*					OEP[16];
	uint32_t					setup[16];	// max controll stage data size: 64B TODO: HAL has 12???
	// TODO: needed??: USB_class_t*				class[15];	// 16 classes (including SETUP (EP0) functions)
	USB_state_t					state		: 2;
	EP_state_t					EP0_state	: 3;
	// classes / interfaces ??
} USB_handle_t;

// TODO: move these definitions and all functions that use them from usb.c to usb_int.c
// OR: clean it up significantly and expose it to the user
#endif //USB_T_H
