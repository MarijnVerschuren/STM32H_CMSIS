//
// Created by marijn on 3/27/23.
//

#ifndef STM32F_CMSIS_PWM_H
#define STM32F_CMSIS_PWM_H

#include "main.h"
#include "gpio.h"
#include "tim.h"


/*!< init / enable / disable */
void config_PWM(TIM_GPIO_t pwm_pin, uint32_t prescaler, uint32_t period);


#endif //STM32F_CMSIS_PWM_H
