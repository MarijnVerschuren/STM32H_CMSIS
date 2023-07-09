//
// Created by marijn on 7/8/23.
//

#include "pwm.h"


/*!<
 * init
 * */
void config_PWM(TIM_GPIO_t pwm_pin, uint32_t prescaler, uint32_t period) {
	dev_pin_t dev = *((dev_pin_t*)&pwm_pin);

}
