//
// Created by marijn on 6/26/23.
//

#ifndef STM32H_CMSIS_BASE_H
#define STM32H_CMSIS_BASE_H
#include "main.h"


/*!<
 * device_id types
 * */
typedef enum {
	DEV_CLOCK_ID_APB1 =	0x0,
	DEV_CLOCK_ID_AHB1 =	0x1,
	DEV_CLOCK_ID_APB2 =	0x2,
	DEV_CLOCK_ID_AHB2 =	0x3,
	DEV_CLOCK_ID_APB3 =	0x4,
	DEV_CLOCK_ID_AHB3 =	0x5,
	DEV_CLOCK_ID_APB4 =	0x6,
	DEV_CLOCK_ID_AHB4 =	0x7,
	DEV_CLOCK_NONE =	0x8
} dev_clock_id_t;

typedef struct {
	uint16_t	dev: 5;		// 1024
	uint16_t	clk: 5;		// dev_clock_id_t
	uint16_t	sub: 6;		// sub device info
} dev_id_t;  // 16 bit

typedef struct {
	dev_id_t	dev_id;
	uint8_t		alt_func	: 4;
	uint8_t		port_num	: 4;
	uint8_t		pin_num		: 4;
	uint8_t		_			: 4;
} dev_pin_t;  // 32 bit


/*!<
 * buffer types
 * */
typedef struct {
	volatile void*		ptr;
	uint32_t			size;
	volatile uint32_t	i;	// write
	volatile uint32_t	o;	// read
} io_buffer_t;


/*!<
 * device_id
 * */
dev_id_t dev_to_id(void* dev);
void* id_to_dev(dev_id_t id);
/*!<
 * buffer
 * */
io_buffer_t* new_buffer(uint32_t size);
void free_buffer(io_buffer_t* buf);


#endif //STM32H_CMSIS_BASE_H
