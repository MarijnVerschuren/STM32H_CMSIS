//
// Created by marijn on 4/26/23.
//

#ifndef STM32F_CMSIS_WATCHDOG_H
#define STM32F_CMSIS_WATCHDOG_H
#include "main.h"


/*!< init / enable */
void config_watchdog(uint8_t prescaler, uint16_t reload);
void config_window_watchdog(uint8_t prescaler, uint16_t reload);  // TODO
/*!< actions */
void start_watchdog();
void start_window_watchdog();  // TODO
void stop_window_watchdog();  // TODO
void reset_watchdog();
void reset_window_watchdog();  // TODO

#endif //STM32F_CMSIS_WATCHDOG_H
