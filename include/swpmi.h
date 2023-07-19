//
// Created by marijn on 7/19/23.
//

#ifndef STM32H_CMSIS_SWPMI_H
#define STM32H_CMSIS_SWPMI_H
#include "main.h"
#include "sys.h"


/*!<
 * types
 * */
typedef enum {
	SWPMI_CLK_SRC_APB1 =	0b0,	//R
	SWPMI_CLK_SRC_HSI =		0b1
} SWPMI_CLK_SRC_t;


/*!<
 * variables
 * */
extern uint32_t SWPMI_kernel_frequency;


/*!<
 * init
 * */
void config_SWPMI_kernel_clock(SWPMI_CLK_SRC_t src);


#endif //STM32H_CMSIS_SWPMI_H
