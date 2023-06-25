//
// Created by marijn on 6/26/23.
//

#include "base.h"


/*!< device */
dev_id_t dev_to_id(void* dev) {
	if ((uint32_t)dev < APB1_BASE)		{ return (dev_id_t){0, DEV_CLOCK_NONE}; }
	if ((uint32_t)dev < APB2_BASE)		{ return (dev_id_t){((uint32_t)dev - APB1_BASE) >> 10u, DEV_CLOCK_ID_APB1}; }
	if ((uint32_t)dev < AHB1_BASE)		{ return (dev_id_t){((uint32_t)dev - APB2_BASE) >> 10u, DEV_CLOCK_ID_APB2}; }
	if ((uint32_t)dev < AHB2_BASE)		{ return (dev_id_t){((uint32_t)dev - AHB1_BASE) >> 10u, DEV_CLOCK_ID_AHB1}; }
	if ((uint32_t)dev < APB3_BASE)		{ return (dev_id_t){((uint32_t)dev - AHB2_BASE) >> 10u, DEV_CLOCK_ID_AHB2}; }
	if ((uint32_t)dev < AHB3_BASE)		{ return (dev_id_t){((uint32_t)dev - APB3_BASE) >> 10u, DEV_CLOCK_ID_APB3}; }
	if ((uint32_t)dev < APB4_BASE)		{ return (dev_id_t){((uint32_t)dev - AHB3_BASE) >> 10u, DEV_CLOCK_ID_AHB3}; }
	if ((uint32_t)dev < AHB4_BASE)		{ return (dev_id_t){((uint32_t)dev - APB4_BASE) >> 10u, DEV_CLOCK_ID_APB4}; }
	return (dev_id_t){((uint32_t)dev - AHB4_BASE) >> 10u, DEV_CLOCK_ID_AHB4};  // TODO: check out of range
}
void* id_to_dev(dev_id_t id) {
	if (id.clk == DEV_CLOCK_NONE)		{ return NULL; }
	if (id.clk == DEV_CLOCK_ID_APB1)	{ return (void*)((id.dev << 10) + APB1_BASE); }
	if (id.clk == DEV_CLOCK_ID_APB2)	{ return (void*)((id.dev << 10) + APB2_BASE); }
	if (id.clk == DEV_CLOCK_ID_AHB1)	{ return (void*)((id.dev << 10) + AHB1_BASE); }
	if (id.clk == DEV_CLOCK_ID_AHB2)	{ return (void*)((id.dev << 10) + AHB2_BASE); }
	if (id.clk == DEV_CLOCK_ID_APB3)	{ return (void*)((id.dev << 10) + APB3_BASE); }
	if (id.clk == DEV_CLOCK_ID_AHB3)	{ return (void*)((id.dev << 10) + AHB3_BASE); }
	if (id.clk == DEV_CLOCK_ID_APB4)	{ return (void*)((id.dev << 10) + APB4_BASE); }
	return (void*)((id.dev << 10) + AHB4_BASE);
}


/*!< buffer */
io_buffer_t* new_buffer(uint32_t size) {
	io_buffer_t* buf = malloc(sizeof(io_buffer_t));
	if (!buf) { return NULL; }		// struct allocation error
	buf->ptr = malloc(size);
	if (!buf->ptr) { return NULL; }	// buffer allocation error
	buf->size = size; buf->i = 0; buf->o = 0;
	return buf;
}
void free_buffer(io_buffer_t* buf) { free(buf->ptr); free(buf); }