//
// Created by marijn on 7/8/23.
//

#ifndef STM32H_CMSIS_PWM_H
#define STM32H_CMSIS_PWM_H
#include "main.h"
#include "gpio.h"
#include "tim.h"


/*!<
 * init
 * */
void config_PWM(TIM_GPIO_t pin, uint32_t prescaler, uint32_t period);


#endif //STM32H_CMSIS_PWM_H
