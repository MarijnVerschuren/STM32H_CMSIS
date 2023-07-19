//
// Created by marijn on 7/19/23.
//

#ifndef STM32H_CMSIS_HDMI_H
#define STM32H_CMSIS_HDMI_H
#include "main.h"
#include "sys.h"


/*!<
 * types
 * */
typedef enum {
	CEC_CLK_SRC_LSE =	0b00,	//R
	CEC_CLK_SRC_LSI =	0b01,
	CEC_CLK_SRC_CSI =	0b10,	// csi_ker_ck / 122
} CEC_CLK_SRC_t;


/*!<
 * variables
 * */
extern uint32_t CEC_kernel_frequency;


/*!<
 * init
 * */
void config_CEC_kernel_clock(CEC_CLK_SRC_t src);


#endif //STM32H_CMSIS_HDMI_H
