//
// Created by marijn on 2/13/23.
//
#ifndef STM32F_MAIN_H
#define STM32F_MAIN_H
#include <malloc.h>
#include <stdint.h>

#if defined(STM32H7xx)
#include "stm32h7xx.h"

// clock redefinitions (0x40000000UL)
#define APB1_BASE D2_APB1PERIPH_BASE	/* 0x00000000UL */
#define AHB1_BASE D2_AHB1PERIPH_BASE	/* 0x00020000UL */
#define APB2_BASE D2_APB2PERIPH_BASE	/* 0x00010000UL */
#define AHB2_BASE D2_AHB2PERIPH_BASE	/* 0x08020000UL */
#define APB3_BASE D1_APB1PERIPH_BASE	/* 0x10000000UL */
#define AHB3_BASE D1_AHB1PERIPH_BASE	/* 0x12000000UL */
#define APB4_BASE D3_APB1PERIPH_BASE	/* 0x18000000UL */
#define AHB4_BASE D3_AHB1PERIPH_BASE	/* 0x18020000UL */
#else
#error invalid board selected
#endif

#include "base.h"

#endif //STM32F_MAIN_H