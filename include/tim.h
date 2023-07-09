//
// Created by marijn on 7/6/23.
//

#ifndef STM32H_CMSIS_TIM_H
#define STM32H_CMSIS_TIM_H
#include "main.h"


/*!<
 * types
 * */
typedef enum {	// sub: 0bTTTNCC -> T: sub_timer (HRTIM), C: channel_number, N: polarity (0 = default)
	// TIM1
	TIM1_BKIN_A6 =		0x06010040,
	TIM1_CH1N_A7 =		0x07011040,
	TIM1_CH1_A8 =		0x08010040,
	TIM1_CH2_A9 =		0x09010440,
	TIM1_CH3_A10 =		0x0A010840,
	TIM1_CH4_A11 =		0x0B010C40,
	TIM1_ETR_A12 =		0x0C010040,
	// TIM2
	TIM2_CH1_ETR_A0 =	0x00010000,
	TIM2_CH2_A1 =		0x01010400,
	TIM2_CH3_A2 =		0x02010800,
	TIM2_CH4_A3 =		0x03010C00,
	TIM2_CH1_ETR_A5 =	0x05010000,
	TIM2_CH1_ETR_A15 =	0x0F010000,
	// TIM3
	TIM3_CH1_A6 =		0x06020001,
	TIM3_CH2_A7 =		0x07020401,
	// TIM4
	// TIM5
	TIM5_CH1_A0 =		0x00020003,
	TIM5_CH2_A1 =		0x01020403,
	TIM5_CH3_A2 =		0x02020803,
	TIM5_CH4_A3 =		0x03020C03,
	TIM5_ETR_A4 =		0x04020003,
	// TIM6
	// TIM7
	// TIM8
	TIM8_ETR_A0 =		0x00030041,
	TIM8_CH1N_A5 =		0x05031041,
	TIM8_BKIN_A6 =		0x06030041,
	TIM8_CH1N_B7 =		0x07031041,
	TIM8_BKIN2_B8 =		0x08030441,  // CH2
	TIM8_BKIN_COMP12 =	0,  // BKIN_CMP? (two channels in 'sub' section?)
	// TIM12
	// TIM13
	TIM13_CH1_A6 =		0x06090007,
	// TIM14
	TIM14_CH1_A7 =		0x07090008,
	// TIM15
	TIM15_BKIN_A0 =		0x00040030,
	TIM15_CH1N_A1 =		0x01041030,
	TIM15_CH1_A2 =		0x02040030,
	TIM15_CH2_A3 =		0x03040430,
	// TIM16
	// TIM17

	// LPTIM1
	// LPTIM2
	// LPTIM3
	LPTIM3_OUT_A1 =		0x010300CA,
	// LPTIM4
	LPTIM4_OUT_A2 =		0x020300CB,
	// LPTIM5
	LPTIM5_OUT_A3 =		0x030300CC,

	// HRTIM
	HRTIM_CHB2_A8 =		0x0802245D,
	HRTIM_CHC1_A9 =		0x0902405D,
	HRTIM_CHC2_A10 =	0x0A02445D,
	HRTIM_CHD1_A11 =	0x0B02605D,
	HRTIM_CHD2_A12 =	0x0C02645D,
	HRTIM_FLT1_A15 =	0x0F02005D
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
/*!<
 * irq
 * */
void start_TIM_update_irq(TIM_TypeDef* tim);
void stop_TIM_update_irq(TIM_TypeDef* tim);


#endif //STM32H_CMSIS_TIM_H
