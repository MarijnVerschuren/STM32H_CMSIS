//
// Created by marijn on 2/13/23.
//
#ifndef STM32F_MAIN_H
#define STM32F_MAIN_H
#include <malloc.h>
#include <stdint.h>

#if defined(STM32H7xx)
#include "stm32h7xx.h"
#else
#error invalid board selected
#endif

#endif //STM32F_MAIN_H