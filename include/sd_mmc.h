//
// Created by marijn on 7/19/23.
//

#ifndef STM32H_CMSIS_SD_MMC_H
#define STM32H_CMSIS_SD_MMC_H
#include "main.h"
#include "sys.h"


/*!<
 * types
 * */
typedef enum {
	SDMMC_CLK_SRC_PLL1_Q = 			0b0,	//R
	SDMMC_CLK_SRC_PLL2_R = 			0b1
} SDMMC_CLK_SRC_t;


/*!<
 * variables
 * */
extern uint32_t SDMMC_kernel_frequency;


/*!<
 * init
 * */
void config_SDMMC_kernel_clocks(SDMMC_CLK_SRC_t src);


#endif //STM32H_CMSIS_SD_MMC_H
