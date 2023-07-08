//
// Created by marijn on 7/6/23.
//

#ifndef STM32H_CMSIS_TIM_H
#define STM32H_CMSIS_TIM_H
#include "main.h"
#include "gpio.h"


/*!<
 * types
 * */
typedef enum {  // TODO
	TIM_GPIO_TODO = 1
} TIM_GPIO_t;


/*!<
 * misc
 * */
uint8_t TIM_to_IRQn(TIM_TypeDef* tim);
/*!<
 * init
 * */
void fconfig_TIM(
		TIM_TypeDef* tim,				uint16_t prescaler,		uint32_t limit,
		uint8_t auto_reload_preload,	uint8_t down_count,		uint8_t one_pulse
);
void config_TIM(TIM_TypeDef* tim, uint16_t prescaler, uint32_t limit);
void disable_TIM(TIM_TypeDef* tim);
/*!<
 * actions
 * */
void start_TIM(TIM_TypeDef* tim);
void stop_TIM(TIM_TypeDef* tim);
/*!<  TODO
 * irq
 * */
void start_TIM_update_irq(TIM_TypeDef* tim);
void stop_TIM_update_irq(TIM_TypeDef* tim);


#endif //STM32H_CMSIS_TIM_H
