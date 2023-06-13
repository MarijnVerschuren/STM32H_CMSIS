//
// Created by marijn on 4/2/23.
//
#include "base.h"


/*!< device */
dev_id_t dev_to_id(void* dev) {
	if ((uint32_t)dev < APB1PERIPH_BASE)  { return (dev_id_t){0, DEV_CLOCK_INVALID}; };
	// AHB2 does not feature any peripherals on the STM32F4 boards
	if ((uint32_t)dev >= AHB2PERIPH_BASE) { return (dev_id_t){((uint32_t)dev - AHB2PERIPH_BASE) >> 10u, DEV_CLOCK_ID_AHB2}; }
	if ((uint32_t)dev >= AHB1PERIPH_BASE) { return (dev_id_t){((uint32_t)dev - AHB1PERIPH_BASE) >> 10u, DEV_CLOCK_ID_AHB1}; }
	if ((uint32_t)dev >= APB2PERIPH_BASE) { return (dev_id_t){((uint32_t)dev - APB2PERIPH_BASE) >> 10u, DEV_CLOCK_ID_APB2}; }
	return (dev_id_t){((uint32_t)dev - APB1PERIPH_BASE) >> 10u, DEV_CLOCK_ID_APB1};
}
void* id_to_dev(dev_id_t id) {
	if (id.reg == DEV_CLOCK_INVALID) { return NULL; }
	if (id.reg == DEV_CLOCK_ID_AHB2) { return (void*)((id.num << 10) + AHB2PERIPH_BASE); }
	if (id.reg == DEV_CLOCK_ID_AHB1) { return (void*)((id.num << 10) + AHB1PERIPH_BASE); }
	if (id.reg == DEV_CLOCK_ID_APB2) { return (void*)((id.num << 10) + APB2PERIPH_BASE); }
	return (void*)((id.num << 10) + APB1PERIPH_BASE);
}


/*!< buffer */
io_buffer_t* new_buffer(uint32_t size) {
	io_buffer_t* buf = malloc(sizeof(io_buffer_t));
	if (!buf) { return NULL; }		// struct allocation error
	buf->ptr = malloc(size);
	if (!buf->ptr) { return NULL; }	// buffer allocation error
	buf->size = size;
	buf->i = 0;
	buf->o = 0;
	return buf;
}
void free_buffer(io_buffer_t* buf) { free(buf->ptr); free(buf); }