//
// Created by marijn on 6/26/23.
//

#ifndef STM32H_CMSIS_BASE_H
#define STM32H_CMSIS_BASE_H
#include "main.h"


/*!<
 * defines
 */
#ifdef DEBUG
/*<! debug barrier:
 * used as a recognizable marker for places where the software waits on the hardware when emulating
 * the emulator will search for the compiled version code of the barrier:
 * thumb: "\xff\x46\xff\x46", arm: "\x0f\xf0\xa0\xe1\x0f\xf0\xa0\xe1"
 * and enact the appropriate actions at the found location
 */
#define DBAR() asm volatile("mov pc, pc\n\tmov pc, pc")
#else
#define DBAR()
#endif

/*!<
 * device types
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
	uint16_t	num: 5;		// 1024
	uint16_t	clk: 5;		// dev_clock_id_t
	uint16_t	sub: 6;		// sub device info
} dev_id_t;  // 16 bit

typedef struct {
	dev_id_t	id;
	uint8_t		alt		: 4;
	uint8_t		port	: 4;
	uint8_t		num		: 4;
	uint8_t		_		: 4;
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
 * device
 * */
void enable_id(dev_id_t id);
void enable_dev(void* dev);
void disable_id(dev_id_t id);
void disable_dev(void* dev);

dev_id_t dev_to_id(void* dev);
void* id_to_dev(dev_id_t id);
void* pin_to_dev(dev_pin_t pin);

uint8_t dev_to_int(void* dev);


/*!<
 * buffer
 * */
io_buffer_t* new_buffer(uint32_t size);
void free_buffer(io_buffer_t* buf);


/*!<
 * string
 * */
uint32_t strlen(const char* str);


#endif //STM32H_CMSIS_BASE_H
