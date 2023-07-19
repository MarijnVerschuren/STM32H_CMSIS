//
// Created by marijn on 7/19/23.
//

#ifndef STM32H_CMSIS_DFSDM_H
#define STM32H_CMSIS_DFSDM_H
#include "main.h"
#include "sys.h"


/*!<
 * types
 * */
typedef enum {
	DFSDM_CLK_SRC_APB2 =	0b0,	//R
	DFSDM_CLK_SRC_SYS =		0b1
} DFSDM_CLK_SRC_t;


/*!<
 * variables
 * */
extern uint32_t DFSDM_kernel_frequency;


/*!<
 * init
 * */
void config_DFSDM_kernel_clock(DFSDM_CLK_SRC_t src);


#endif //STM32H_CMSIS_DFSDM_H
