//
// Created by marijn on 7/9/23.
//

#include "crc.h"


/*!<
 * init
 * */
void fconfig_CRC(uint8_t reverse_out, CRC_REV_IN_t reverse_in, CRC_POLY_SIZE_t poly_size) {
	do { RCC->AHB4ENR |= RCC_AHB4ENR_CRCEN; } while (!(RCC->AHB4ENR & RCC_AHB4ENR_CRCEN));
	CRC->CR = (
			(reverse_out << CRC_CR_REV_OUT_Pos)		|
			(reverse_in << CRC_CR_REV_IN_Pos)		|
			(poly_size << CRC_CR_POLYSIZE_Pos)
	); reset_CRC();
}
void config_CRC(void) { fconfig_CRC(0, CRC_REV_IN_BONA, CRC_POLY_SIZE_32); }


/*!<
 * actions
 * */
void reset_CRC(void) {
	CRC->CR |= CRC_CR_RESET;			// reset crc
	while (CRC->CR & CRC_CR_RESET);		// wait until done
}