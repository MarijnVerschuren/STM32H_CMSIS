//
// Created by marijn on 6/26/23.
//

#include "base.h"


/*!<
 * device
 * */
void enable_id(dev_id_t id) {
	if (id.clk == DEV_CLOCK_NONE) { return; }
	uint32_t mask = (0b1UL << (id.num & 0b11111UL));
	volatile uint32_t* reg; switch (id.clk) {
		case DEV_CLOCK_ID_APB1: reg = ((&RCC->APB1LENR) + ((id.num >> 5) & 0b1UL)); break;
		case DEV_CLOCK_ID_APB2: reg = &RCC->APB2ENR; break;
		case DEV_CLOCK_ID_AHB1: reg = &RCC->AHB1ENR; break;
		case DEV_CLOCK_ID_AHB2: reg = &RCC->AHB2ENR; break;
		case DEV_CLOCK_ID_APB3: reg = &RCC->APB3ENR; break;
		case DEV_CLOCK_ID_AHB3: reg = &RCC->AHB3ENR; break;
		case DEV_CLOCK_ID_APB4: reg = &RCC->APB4ENR; break;
		case DEV_CLOCK_ID_AHB4: reg = &RCC->AHB4ENR; break;
	} do { (*reg) |= mask; } while (!((*reg) & mask));
}
void enable_dev(void* dev) {
	dev_id_t id = dev_to_id(dev);
	enable_id(id);
}
void disable_id(dev_id_t id) {
	if (id.clk == DEV_CLOCK_NONE) { return; }
	uint32_t mask = (0b1UL << (id.num & 0b11111UL));
	volatile uint32_t* reg; switch (id.clk) {
		case DEV_CLOCK_ID_APB1: reg = ((&RCC->APB1LRSTR) + ((id.num >> 5) & 0b1UL)); break;
		case DEV_CLOCK_ID_APB2: reg = &RCC->APB2RSTR; break;
		case DEV_CLOCK_ID_AHB1: reg = &RCC->AHB1RSTR; break;
		case DEV_CLOCK_ID_AHB2: reg = &RCC->AHB2RSTR; break;
		case DEV_CLOCK_ID_APB3: reg = &RCC->APB3RSTR; break;
		case DEV_CLOCK_ID_AHB3: reg = &RCC->AHB3RSTR; break;
		case DEV_CLOCK_ID_APB4: reg = &RCC->APB4RSTR; break;
		case DEV_CLOCK_ID_AHB4: reg = &RCC->AHB4RSTR; break;
	} (*reg) |= mask;
}
void disable_dev(void* dev) {
	dev_id_t id = dev_to_id(dev);
	disable_id(id);
}

dev_id_t dev_to_id(void* dev) {
	if ((uint32_t)dev < APB1_BASE)		{ return (dev_id_t){0, DEV_CLOCK_NONE}; }
	if ((uint32_t)dev < APB2_BASE)		{ return (dev_id_t){((uint32_t)dev - APB1_BASE) >> 10u, DEV_CLOCK_ID_APB1}; }
	if ((uint32_t)dev < AHB1_BASE)		{ return (dev_id_t){((uint32_t)dev - APB2_BASE) >> 10u, DEV_CLOCK_ID_APB2}; }
	if ((uint32_t)dev < AHB2_BASE)		{ return (dev_id_t){((uint32_t)dev - AHB1_BASE) >> 10u, DEV_CLOCK_ID_AHB1}; }
	if ((uint32_t)dev < APB3_BASE)		{ return (dev_id_t){((uint32_t)dev - AHB2_BASE) >> 10u, DEV_CLOCK_ID_AHB2}; }
	if ((uint32_t)dev < AHB3_BASE)		{ return (dev_id_t){((uint32_t)dev - APB3_BASE) >> 10u, DEV_CLOCK_ID_APB3}; }
	if ((uint32_t)dev < APB4_BASE)		{ return (dev_id_t){((uint32_t)dev - AHB3_BASE) >> 10u, DEV_CLOCK_ID_AHB3}; }
	if ((uint32_t)dev < AHB4_BASE)		{ return (dev_id_t){((uint32_t)dev - APB4_BASE) >> 10u, DEV_CLOCK_ID_APB4}; }
	return (dev_id_t){((uint32_t)dev - AHB4_BASE) >> 10u, DEV_CLOCK_ID_AHB4};
}
void* id_to_dev(dev_id_t id) {
	if (id.clk == DEV_CLOCK_NONE)		{ return NULL; }
	if (id.clk == DEV_CLOCK_ID_APB1)	{ return (void*)((id.num << 10) + APB1_BASE); }
	if (id.clk == DEV_CLOCK_ID_APB2)	{ return (void*)((id.num << 10) + APB2_BASE); }
	if (id.clk == DEV_CLOCK_ID_AHB1)	{ return (void*)((id.num << 10) + AHB1_BASE); }
	if (id.clk == DEV_CLOCK_ID_AHB2)	{ return (void*)((id.num << 10) + AHB2_BASE); }
	if (id.clk == DEV_CLOCK_ID_APB3)	{ return (void*)((id.num << 10) + APB3_BASE); }
	if (id.clk == DEV_CLOCK_ID_AHB3)	{ return (void*)((id.num << 10) + AHB3_BASE); }
	if (id.clk == DEV_CLOCK_ID_APB4)	{ return (void*)((id.num << 10) + APB4_BASE); }
	return (void*)((id.num << 10) + AHB4_BASE);
}
void* pin_to_dev(dev_pin_t pin) { return id_to_dev(pin.id); }

uint8_t dev_to_int(void* dev) {
	if ((uint32_t)dev < APB1_BASE)		{ return -1; }
	if ((uint32_t)dev < APB2_BASE)		{ return ((uint32_t)dev - APB1_BASE) >> 10u; }
	if ((uint32_t)dev < AHB1_BASE)		{ return ((uint32_t)dev - APB2_BASE) >> 10u; }
	if ((uint32_t)dev < AHB2_BASE)		{ return ((uint32_t)dev - AHB1_BASE) >> 10u; }
	if ((uint32_t)dev < APB3_BASE)		{ return ((uint32_t)dev - AHB2_BASE) >> 10u; }
	if ((uint32_t)dev < AHB3_BASE)		{ return ((uint32_t)dev - APB3_BASE) >> 10u; }
	if ((uint32_t)dev < APB4_BASE)		{ return ((uint32_t)dev - AHB3_BASE) >> 10u; }
	if ((uint32_t)dev < AHB4_BASE)		{ return ((uint32_t)dev - APB4_BASE) >> 10u; }
	return ((uint32_t)dev - AHB4_BASE) >> 10u;
}


/*!<
 * buffer
 * */
io_buffer_t* new_buffer(uint32_t size) {
	io_buffer_t* buf = malloc(sizeof(io_buffer_t));
	if (!buf) { return NULL; }		// struct allocation error
	buf->ptr = malloc(size);
	if (!buf->ptr) { return NULL; }	// buffer allocation error
	buf->size = size; buf->i = 0; buf->o = 0;
	return buf;
}
void free_buffer(io_buffer_t* buf) { free(buf->ptr); free(buf); }


/*!<
 * string
 * */
uint32_t strlen(const char* str) {
	register uint32_t len = 0;
	while (*str++) { len++; }
	return len;
}
