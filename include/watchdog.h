//
// Created by marijn on 7/15/23.
//

#ifndef STM32H_CMSIS_WATCHDOG_H
#define STM32H_CMSIS_WATCHDOG_H
#include "main.h"


/*!<
 * init
 * */
void config_watchdog(uint8_t prescaler, uint16_t reload);
/*!<
 * actions
 * */
void start_watchdog();
void reset_watchdog();


#endif //STM32H_CMSIS_WATCHDOG_H
