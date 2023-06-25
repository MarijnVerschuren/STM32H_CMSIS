//
// Created by marijn on 6/26/23.
//

#ifndef STM32H_CMSIS_BASE_H
#define STM32H_CMSIS_BASE_H
#include "main.h"


/*!< device_id */
typedef enum {
	DEV_CLOCK_ID_APB1 = 0,
	DEV_CLOCK_ID_AHB1 = 1,
	DEV_CLOCK_ID_APB2 = 2,
	DEV_CLOCK_ID_AHB2 = 3,
	DEV_CLOCK_ID_APB3 = 4,
	DEV_CLOCK_ID_AHB3 = 5,
	DEV_CLOCK_ID_APB4 = 6,
	DEV_CLOCK_ID_AHB4 = 7,
	DEV_CLOCK_NONE = 8
} dev_clock_id_t;

typedef struct {
	uint16_t	dev: 10;	// 1024
	uint16_t	clk: 6;		// dev_clock_id_t (overhead for different MCUs)
} dev_id_t;

typedef struct {
	dev_id_t	dev_id;		// 16 bit
	uint8_t		dev_info	: 4;
	uint8_t		alt_func	: 4;
	uint8_t		port_num	: 4;
	uint8_t		pin_num		: 4;
} dev_pin_t;

/*!< buffer */
typedef struct {
	volatile void*		ptr;
	uint32_t			size;
	volatile uint32_t	i;	// write
	volatile uint32_t	o;	// read
} io_buffer_t;


/*!< device_id */
dev_id_t dev_to_id(void* dev);
void* id_to_dev(dev_id_t id);
/*!< buffer */
io_buffer_t* new_buffer(uint32_t size);
void free_buffer(io_buffer_t* buf);


#endif //STM32H_CMSIS_BASE_H
