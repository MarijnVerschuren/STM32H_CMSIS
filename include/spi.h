//
// Created by marijn on 7/19/23.
//

#ifndef STM32H_CMSIS_SPI_H
#define STM32H_CMSIS_SPI_H
#include "main.h"
#include "sys.h"


/*!<
 * types
 * */
typedef enum {
	QSPI_CLK_SRC_AHB =			0b00,	//R
	QSPI_CLK_SRC_PLL1_Q =		0b01,
	QSPI_CLK_SRC_PLL2_R =		0b10,
	QSPI_CLK_SRC_PER =			0b11
} QSPI_CLK_SRC_t;

typedef enum {
	SPI456_CLK_SRC_APBx =		0b000,	//R
	SPI456_CLK_SRC_PLL2_Q =		0b001,
	SPI456_CLK_SRC_PLL3_Q =		0b010,
	SPI456_CLK_SRC_HSI =		0b011,
	SPI456_CLK_SRC_CSI =		0b100,
	SPI456_CLK_SRC_HSE =		0b101
} SPI456_CLK_SRC_t;

typedef enum {
	SPI123_CLK_SRC_PLL1_Q =		0b000,	//R
	SPI123_CLK_SRC_PLL2_P =		0b001,
	SPI123_CLK_SRC_PLL3_P =		0b010,
	SPI123_CLK_SRC_I2S_CKIN =	0b011,
	SPI123_CLK_SRC_PER =		0b100
} SPI123_CLK_SRC_t;


/*!<
 * variables
 * */
extern uint32_t SPI123_kernel_frequency;
extern uint32_t SPI45_kernel_frequency;
extern uint32_t SPI6_kernel_frequency;
extern uint32_t QSPI_kernel_frequency;


/*!<
 * init
 * */
void config__kernel_clocks(
		SPI123_CLK_SRC_t spi123_src, SPI456_CLK_SRC_t spi45_src,
		SPI456_CLK_SRC_t spi6_src, QSPI_CLK_SRC_t qspi_src
);


#endif //STM32H_CMSIS_SPI_H
