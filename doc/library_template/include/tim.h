//
// Created by marijn on 2/13/23.
//
#ifndef STM32F_TIM_H
#define STM32F_TIM_H
#include "main.h"
#include "base.h"


/* data layout:
 * struct:
 *		dev_id						: 8;  // MSB
 *			num							: 5;	// MSB
 *			clock						: 3;	// LSB
 *		misc						: 4;
 *			channel 					: 2;	// MSB
 *			reserved					: 2;	// LSB
 * 		alternate_function_number	: 4;
 * 		port_number					: 4;
 * 		pin_number					: 4;  // LSB
 * functions:
 * 		TIMx_ETR -> external trigger (used in external counters)
 * 		TIMx_CHx -> channel output (used in pwm)
 * 		TIMx_CHxN -> channel inverse out
 * 		TIMx_BKIN -> ?
 * */
typedef enum {
	TIM_PIN_DISABLE =	0x000000,
	// TIM1
	TIM1_BKIN_A6 =		0x600106,	TIM1_CH1N_A7 =		0x600107,
	TIM1_CH1_A8 =		0x600108,	TIM1_CH2_A9 =		0x604109,
	TIM1_CH3_A10 =		0x60810A,	TIM1_CH4_A11 =		0x60C10B,
	TIM1_ETR_A12 =		0x60010C,	TIM1_CH2N_B0 =		0x604110,
	TIM1_CH3N_B1 =		0x608111,	TIM1_BKIN_B12 =		0x60011C,
	TIM1_CH1N_B13 =		0x60011D,	TIM1_CH2N_B14 =		0x60411E,
	TIM1_CH3N_B15 =		0x60811F,	TIM1_ETR_E7 =		0x600147,
	TIM1_CH1N_E8 =		0x600148,	TIM1_CH1_E9 =		0x600149,
	TIM1_CH2N_E10 =		0x60414A,	TIM1_CH2_E11 =		0x60414B,
	TIM1_CH3N_E12 =		0x60814C,	TIM1_CH3_E13 =		0x60814D,
	TIM1_CH4_E14 =		0x60C14E,	TIM1_BKIN_E15 =		0x60014F,
	// TIM2
	TIM2_CH1_A0 =		0x400100,	TIM2_ETR_A0 =		0x400100,
	TIM2_CH2_A1 =		0x404101,	TIM2_CH3_A2 =		0x408102,
	TIM2_CH4_A3 =		0x40C103,	TIM2_CH1_A5 =		0x400105,
	TIM2_ETR_A5 =		0x400105,	TIM2_CH1_A15 =		0x40010F,
	TIM2_ETR_A15 =		0x40010F,	TIM2_CH2N_B0 =		0x404110,
	TIM2_CH3N_B1 =		0x408111,	TIM2_CH2_B3 =		0x404113,
	TIM2_CH3_B10 =		0x40811A,	TIM2_CH4_B11 =		0x40C11B,
	// TIM3
	TIM3_CH1_A6 =		0x410206,	TIM3_CH2_A7 =		0x414207,
	TIM3_CH3_B0 =		0x418210,	TIM3_CH4_B1 =		0x41C211,
	TIM3_CH1_B4 =		0x410214,	TIM3_CH2_B5 =		0x414215,
	TIM3_CH1_C6 =		0x410226,	TIM3_CH2_C7 =		0x414227,
	TIM3_CH3_C8 =		0x418228,	TIM3_CH4_C9 =		0x41C229,
	TIM3_ETR_D2 =		0x410232,
	// TIM4
	TIM4_CH1_B6 =		0x420216,	TIM4_CH2_B7 =		0x424217,
	TIM4_CH3_B8 =		0x428218,	TIM4_CH4_B9 =		0x42C219,
	TIM4_CH1_D12 =		0x42023C,	TIM4_CH2_D13 =		0x42423D,
	TIM4_CH3_D14 =		0x42823E,	TIM4_CH4_D15 =		0x42C23F,
	TIM4_ETR_E0 =		0x420240,
	// TIM5
	TIM5_CH1_A0 =		0x430200,	TIM5_CH2_A1 =		0x434201,
	TIM5_CH3_A2 =		0x438202,	TIM5_CH4_A3 =		0x43C203,
	// TIM9
	TIM9_CH1_A2 =		0x700302,	TIM9_CH2_A3 =		0x704303,
	TIM9_CH1_E5 =		0x700345,	TIM9_CH2_E6 =		0x704346,
	// TIM10
	TIM10_CH1_B8 =		0x710318,
	// TIM11
	TIM11_CH1_B9 =		0x720319
} TIM_GPIO_t;


/*!< init / enable / disable */
void config_TIM(TIM_TypeDef* tim, uint32_t prescaler, uint32_t limit);
void disable_TIM(TIM_TypeDef* tim);
/*!< actions */
void start_TIM(TIM_TypeDef* tim);
void stop_TIM(TIM_TypeDef* tim);
/*!< irq */
void start_TIM_update_irq(TIM_TypeDef* tim);
void stop_TIM_update_irq(TIM_TypeDef* tim);
void start_TIM_capture_compare_irq(TIM_TypeDef* tim);
void stop_TIM_capture_compare_irq(TIM_TypeDef* tim);
void start_TIM_break_irq(TIM_TypeDef* tim);
void stop_TIM_break_irq(TIM_TypeDef* tim);
void start_TIM_trigger_commutation_irq(TIM_TypeDef* tim);
void stop_TIM_trigger_commutation_irq(TIM_TypeDef* tim);


#endif //STM32F_TIM_H
