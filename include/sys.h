//
// Created by marijn on 6/26/23.
//

#ifndef STM32H_CMSIS_SYS_H
#define STM32H_CMSIS_SYS_H

#include "main.h"


/*!< types */
typedef struct {
	uint32_t ID0;
	uint32_t ID1;
	uint32_t ID2;
} ID;

#define UID ((ID *)UID_BASE)
typedef void(*sys_tick_t)(void);

typedef enum {
	//================================================================================================================|
	// flash access latency | (* non documented)                      AXI_freq (MHz)                                  |
	//     (table 17)       |                   VOS3               VOS2               VOS1            VOS0 (boot)     |
	//    in AXI cycles     |__________    0.95V - 1.05V      1.05V - 1.15V      1.15V - 1.26V      1.26V - 1.40V     |
	FLASH_LATENCY0 =         0b000,  //     0 < AXI ≤ 45       0 < AXI ≤ 55       0 < AXI ≤ 70       0 < AXI ≤ 70     |
	FLASH_LATENCY1 =         0b001,  //    45 < AXI ≤ 90      55 < AXI ≤ 110     70 < AXI ≤ 140     70 < AXI ≤ 140    |
	FLASH_LATENCY2 =         0b010,  //    90 < AXI ≤ 135    110 < AXI ≤ 165    140 < AXI ≤ 185    140 < AXI ≤ 185    |
	FLASH_LATENCY3 =         0b011,  //   135 < AXI ≤ 180    165 < AXI ≤ 225    210 < AXI ≤ 225    210 < AXI ≤ 225    |
	FLASH_LATENCY4 =         0b100,  //   180 < AXI ≤ 225  * 225 < AXI ≤ 240    225 < AXI ≤ 240    225 < AXI ≤ 240    |
	//================================================================================================================|
} FLASH_LATENCY_t;

typedef enum {
	//================================================================================================================|
	// flash access latency |                                         AXI_freq (MHz)                                  |
	//     (table 17)       |                   VOS3               VOS2               VOS1            VOS0 (boot)     |
	//    in AXI cycles     |__________    0.95V - 1.05V      1.05V - 1.15V      1.15V - 1.26V      1.26V - 1.40V     |
	FLASH_PROG_DELAY0 =      0b000,  //     0 < AXI ≤ 45       0 < AXI ≤ 55       0 < AXI ≤ 70       0 < AXI ≤ 70     |
	FLASH_PROG_DELAY1 =      0b001,  //    45 < AXI ≤ 135     55 < AXI ≤ 165     70 < AXI ≤ 185     70 < AXI ≤ 185    |
	FLASH_PROG_DELAY2 =      0b010,  //   135 < AXI ≤ 225    165 < AXI ≤ 225    185 < AXI ≤ 225    185 < AXI ≤ 240    |
	//================================================================================================================|
} FLASH_PROG_DELAY_t;

typedef enum {
	PLL_SRC_HSI =		0b00,	//*
	PLL_SRC_CSI =		0b01,
	PLL_SRC_HSE =		0b10,
	PLL_SRC_DISABLED =	0b11,
} PLL_SRC_t;		// 2 bit

typedef enum {
	PLL_IN_1MHz_2MHz =	0b00,	//*	// 1 MHz < CLK_IN < 2 MHz
	PLL_IN_2MHz_4MHz =	0b01,		// 2 MHz < CLK_IN < 4 MHz
	PLL_IN_4MHz_8MHz =	0b10,		// 4 MHz < CLK_IN < 8 MHz
	PLL_IN_8MHz_16MHz =	0b11		// 8 MHz < CLK_IN < 16 MHz
} PLL_IN_t;			// 2 bit

typedef enum {
	PLL_VCO_WIDE =		0b00,	//*	// 192 MHz < PLL_CLK < 960 MHz
	PLL_VCO_MEDIUM =	0b01,		// 150 MHz < PLL_CLK < 420 MHz
} PLL_VCO_t;		// 2 bit

typedef enum {
	SYS_CLK_SRC_HSI =	0b00,	//*
	SYS_CLK_SRC_CSI =	0b01,
	SYS_CLK_SRC_HSE =	0b10,
	SYS_CLK_SRC_PLL1 =	0b11,		// PLL1_P
} SYS_CLK_SRC_t;	// 2 bit



typedef struct {
	// RCC_CR
	uint64_t	enable		: 1;
	// RCC_PLLCKSELR
	uint64_t	M_factor	: 5;
	// RCC_PLLCFGR
	uint64_t	P_enable	: 1;
	uint64_t	Q_enable	: 1;
	uint64_t	R_enable	: 1;
	uint64_t	input_range	: 2;	// PLL_IN_t
	uint64_t	VCO_range	: 1;	// PLL_VCO_t
	uint64_t	frac_enable	: 1;
	// RCC_PLLxDIVR
	uint64_t	P_factor	: 7;	// (+1)
	uint64_t	Q_factor	: 7;	// (+1)
	uint64_t	R_factor	: 7;	// (+1)
	uint64_t	N_factor	: 9;	// (+1)
	// RCC_PLLxDIVR
	uint64_t	N_fraction	: 13;	// PLL_IN * (N_factor (+1) + (N_fraction / 2^13))
} PLL_CLK_Config_t;	// 56 bit

typedef struct {
	double				HSE_freq;				// [4, 48] MHz
	PLL_CLK_Config_t	PLL1_config;
	PLL_CLK_Config_t	PLL2_config;
	PLL_CLK_Config_t	PLL3_config;
	uint64_t			PLL_src			: 2;	// PLL_SRC_t
	uint64_t			SYS_CLK_src		: 2;	// SYS_CLK_SRC_t
} SYS_CLK_Config_t;


/*!< variables */   // values are updated when calling sys_clock_init
extern uint32_t PLL1_clock_frequency;
extern uint32_t PLL2_clock_frequency;
extern uint32_t PLL3_clock_frequency;
extern uint32_t RTC_clock_frequency;

extern uint32_t SYS_clock_frequency;

extern volatile uint64_t tick;  // updated sys_tick


/*!< interrupts */
void SysTick_Handler(void);

/*!< init / enable / disable */
SYS_CLK_Config_t* new_SYS_CLK_config(void);
void sys_clock_init(SYS_CLK_Config_t* config);

/*!< misc */
void delay_ms(uint64_t ms);


#endif //STM32H_CMSIS_SYS_H
