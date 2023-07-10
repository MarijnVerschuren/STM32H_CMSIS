//
// Created by marijn on 7/9/23.
//

#ifndef STM32H_CMSIS_CRC_H
#define STM32H_CMSIS_CRC_H
#include "main.h"

#define CRC_POLY 0x4C11DB7


/*!<
 * types
 * */
typedef enum {
	CRC_REV_IN_BONA =	0b00,	//R	// bit order not affected
	CRC_REV_IN_BRBB =	0b01,		// bit reversal by byte
	CRC_REV_IN_BRBH =	0b10,		// bit reversal by half-word
	CRC_REV_IN_BRBW =	0b11,		// bit reversal by word
} CRC_REV_IN_t;
typedef enum {
	CRC_POLY_SIZE_32 =	0b00,	//R
	CRC_POLY_SIZE_16 =	0b01,
	CRC_POLY_SIZE_8 =	0b10,
	CRC_POLY_SIZE_7 =	0b11,
} CRC_POLY_SIZE_t;


/*!<
 * init
 * */
void fconfig_CRC(uint8_t reverse_out, CRC_REV_IN_t reverse_in, CRC_POLY_SIZE_t poly_size);
void config_CRC(void);
/*!<
 * actions
 * */
void reset_CRC(void);


#endif //STM32H_CMSIS_CRC_H
