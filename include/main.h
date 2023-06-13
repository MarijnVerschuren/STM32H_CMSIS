//
// Created by marijn on 2/13/23.
//
#ifndef STM32F_MAIN_H
#define STM32F_MAIN_H
#include <malloc.h>
#include <stdint.h>

#if defined(STM32F4xx)
#include "stm32f4xx.h"
#elif defined(STM32F3xx)
#include "stm32f3xx.h"
#endif

#endif //STM32F_MAIN_H