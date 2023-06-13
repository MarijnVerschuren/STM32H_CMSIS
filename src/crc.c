//
// Created by marijn on 4/5/23.
//

#include "crc.h"

#ifdef STM32F4xx
/*!< enable / disable */
void enable_CRC(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;	// enable crc clock
    reset_CRC();
}

void disable_CRC(void) {
    RCC->AHB1ENR &= ~RCC_AHB1ENR_CRCEN;	// disable crc clock
}

#elif defined(STM32F3xx)
/*!< enable / disable */
void enable_CRC(void) {
    RCC->AHBENR |= RCC_AHBENR_CRCEN;    // enable crc clock
    reset_CRC();
}

void disable_CRC(void) {
    RCC->AHBENR &= ~RCC_AHBENR_CRCEN;   // disable crc clock
}

#endif

/*!< usage */
void reset_CRC(void) {
	CRC->CR |= CRC_CR_RESET;			// reset crc
	while (CRC->CR & CRC_CR_RESET);		// wait until done
}