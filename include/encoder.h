//
// Created by marijn on 7/10/23.
//

#ifndef STM32H_CMSIS_ENCODER_H
#define STM32H_CMSIS_ENCODER_H
#include "main.h"
#include "gpio.h"
#include "tim.h"


/*!<
 * init
 * */
void config_encoder_S0S90(TIM_GPIO_t encoder_pin_a, TIM_GPIO_t encoder_pin_b);
/*!<
 * actions
 * */
void(*start_encoder_S0S90)(TIM_TypeDef* tim);
void(*stop_encoder_S0S90)(TIM_TypeDef* tim);


#endif //STM32H_CMSIS_ENCODER_H
