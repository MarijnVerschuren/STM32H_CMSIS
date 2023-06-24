//
// Created by marijn on 2/13/23.
//
#ifndef STM32F_MAIN_H
#define STM32F_MAIN_H
#include <malloc.h>
#include <stdint.h>

#if defined(STM32H7xx)
#include "stm32h7xx.h"

// clock redefinitions
#define APB1_BASE D2_APB1PERIPH_BASE
#define AHB1_BASE D2_AHB1PERIPH_BASE
#define APB2_BASE D2_APB2PERIPH_BASE
#define AHB2_BASE D2_AHB2PERIPH_BASE
#define APB3_BASE D1_APB1PERIPH_BASE
#define AHB3_BASE D1_AHB1PERIPH_BASE
#define APB4_BASE D3_APB1PERIPH_BASE
#define AHB4_BASE D3_AHB1PERIPH_BASE
#else
#error invalid board selected
#endif

#endif //STM32F_MAIN_H