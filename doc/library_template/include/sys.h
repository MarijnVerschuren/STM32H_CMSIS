//
// Created by marijn on 3/1/23.
//

#ifndef STM32F_CLOCK_H
#define STM32F_CLOCK_H

#include "main.h"

typedef struct {
	uint32_t ID0;
	uint32_t ID1;
	uint32_t ID2;
} ID;
#if defined(STM32F4xx)
#define UID_BASE 0x1FFF7A10UL
#elif defined(STM32F3xx)
#define UID_BASE 0x1FFFF7ACUL
#endif
#define UID ((ID *)UID_BASE)
typedef void(*sys_tick_t)(void);

typedef enum {
	PLL_P_DIV2 = 0b00,
	PLL_P_DIV4 = 0b01,
	PLL_P_DIV6 = 0b10,
	PLL_P_DIV8 = 0b11
} PLL_P_t;

typedef enum {
	PLL_SRC_HSI = 0b0,
	PLL_SRC_HSE = 0b1
} PLL_Source_t;

typedef enum {
	//================================================================================================================|
	// flash access latency |                                            H_CLK (MHz)                                  |
	//      (table 6)       |              voltage range      voltage range      voltage range      voltage range     |
	//    in CPU cycles     |__________      2V7 - 3V6          2V4 - 2V7          2V1 - 2V4          1V71 - 2V1      |
	FLASH_LATENCY1 =         0b000,  //    0 < H_CLK ≤ 30     0 < H_CLK ≤ 24     0 < H_CLK ≤ 18     0 < H_CLK ≤ 16    |
	FLASH_LATENCY2 =         0b001,  //    30 < H_CLK ≤ 60    24 < H_CLK ≤ 48    18 < H_CLK ≤ 36    16 < H_CLK ≤ 32   |
	FLASH_LATENCY3 =         0b010,  //    60 < H_CLK ≤ 90    48 < H_CLK ≤ 72    36 < H_CLK ≤ 54    32 < H_CLK ≤ 48   |
	FLASH_LATENCY4 =         0b011,  //    90 < H_CLK ≤ 120   72 < H_CLK ≤ 96    54 < H_CLK ≤ 72    48 < H_CLK ≤ 64   |
	FLASH_LATENCY5 =         0b100,  //    120 < H_CLK ≤ 150  96 < H_CLK ≤ 120   72 < H_CLK ≤ 90    64 < H_CLK ≤ 80   |
	FLASH_LATENCY6 =         0b101,  //    150 < H_CLK ≤ 168  120 < H_CLK ≤ 144  90 < H_CLK ≤ 108   80 < H_CLK ≤ 96   |
	FLASH_LATENCY7 =         0b110,  //           -           144 < H_CLK ≤ 168  108 < H_CLK ≤ 120  96 < H_CLK ≤ 112  |
	FLASH_LATENCY8 =         0b111   //           -                  -           120 < H_CLK ≤ 138  112 < H_CLK ≤ 128 |
	//================================================================================================================|
} FLASH_LATENCY_t;

typedef enum {
	SYS_CLK_SRC_HSI = 0b00,
	SYS_CLK_SRC_HSE = 0b01,
	SYS_CLK_SRC_PLL = 0b10
} SYS_CLK_Source_t;

typedef enum {
	AHB_CLK_NO_DIV =	0b0000,
	AHB_CLK_DIV2 =		0b1000,
	AHB_CLK_DIV4 =		0b1001,
	AHB_CLK_DIV8 =		0b1010,
	AHB_CLK_DIV16 =		0b1011,
	AHB_CLK_DIV64 =		0b1100,
	AHB_CLK_DIV128 =	0b1101,
	AHB_CLK_DIV256 =	0b1110,
	AHB_CLK_DIV512 =	0b1111
} AHB_CLK_Prescaler_t;

typedef enum {
	APBx_CLK_NO_DIV =	0b000,
	APBx_CLK_DIV2 =		0b100,
	APBx_CLK_DIV4 =		0b101,
	APBx_CLK_DIV8 =		0b110,
	APBx_CLK_DIV16 =	0b111
} APBx_CLK_Prescaler_t;

typedef enum {
	MCO1_CLK_SRC_HSI = 0b00,
	MCO1_CLK_SRC_LSE = 0b01,
	MCO1_CLK_SRC_HSE = 0b10,
	MCO1_CLK_SRC_PLL = 0b11
} MCO1_CLK_Source_t;

typedef enum {
	MCO2_CLK_SRC_SYS_CLK =	0b00,
	MCO2_CLK_SRC_PLL_I2S =	0b01,
	MCO2_CLK_SRC_HSE =		0b10,
	MCO2_CLK_SRC_PLL =		0b11
} MCO2_CLK_Source_t;

typedef enum {
	MCOx_CLK_NO_DIV =	0b000,
	MCOx_CLK_DIV2 =		0b100,
	MCOx_CLK_DIV3 =		0b101,
	MCOx_CLK_DIV4 =		0b110,
	MCOx_CLK_DIV5 =		0b111
} MCOx_CLK_Prescaler_t;

typedef enum {
	SYS_power_1v7 = 0,		// 1v71	- 2v1
	SYS_power_2v1 = 1,		// 2v1	- 2v4
	SYS_power_2v4 = 2,		// 2v4	- 2v7
	SYS_power_2v7 = 3,		// 2v7	- 3v6
	SYS_power_nominal = 3,
} SYS_Power_t;


typedef struct {
	// RCC_PLL_CFGR config
	uint32_t PLL_M						: 6;
	uint32_t PLL_N						: 9;
	uint32_t PLL_P						: 2;  // PLL_P_TypeDef
	uint32_t PLL_Q						: 4;  // (0, 1 are invalid)
	uint32_t PLL_source					: 1;  // PLL_Source_TypeDef
	// FLASH_ACR config
	uint32_t FLASH_latency				: 3;  // FLASH_LATENCY_TypeDef
	uint32_t FLASH_prefetch				: 1;
	uint32_t FLASH_instruction_cache	: 1;
	uint32_t FLASH_data_cache			: 1;
	// RCC_CFGR config
	uint32_t SYS_CLK_source				: 2;  // SYS_CLK_Source_TypeDef
	uint32_t AHB_prescaler				: 4;  // AHB_CLK_Prescaler_TypeDef
	uint32_t APB1_prescaler				: 3;  // APBx_CLK_Prescaler_TypeDef
	uint32_t APB2_prescaler				: 3;  // APBx_CLK_Prescaler_TypeDef
	uint32_t RTC_prescaler				: 5;
	uint32_t MCO1_source				: 2;  // MCO1_CLK_Source_TypeDef
	uint32_t MCO1_prescaler				: 3;  // MCOx_CLK_Prescaler_TypeDef
	uint32_t MCO2_prescaler				: 3;  // MCOx_CLK_Prescaler_TypeDef
	uint32_t MCO2_source				: 2;  // MCO2_CLK_Source_TypeDef
	// SYS_TICK config
	uint32_t SYS_tick_enable			: 1;
	uint32_t SYS_tick_interrupt_enable	: 1;
	// power setting (power provided to the MCU)
	uint32_t SYS_power					: 2;  // SYS_Power_TypeDef
} SYS_CLK_Config_t;


/*!< variables */   // values are updated when calling sys_clock_init
extern uint32_t PLL_clock_frequency;
extern uint32_t AHB_clock_frequency;
extern uint32_t APB1_clock_frequency;
extern uint32_t APB2_clock_frequency;
extern uint32_t RTC_clock_frequency;

extern uint32_t SYS_clock_frequency;

extern volatile uint64_t tick;  // updated sys_tick


/*!< interrupts */
void SysTick_Handler(void);
/*!< init / enable / disable */
SYS_CLK_Config_t* new_SYS_CLK_config(void);
void set_SYS_PLL_config(SYS_CLK_Config_t* config, uint8_t M, uint16_t N, PLL_P_t P, uint8_t Q, PLL_Source_t PLL_src);
void set_SYS_FLASH_config(SYS_CLK_Config_t* config, FLASH_LATENCY_t latency, uint8_t prefetch, uint8_t enable_icache, uint8_t enable_dcache);
void set_SYS_CLOCK_config(SYS_CLK_Config_t* config, SYS_CLK_Source_t SYS_src, AHB_CLK_Prescaler_t AHB_prescaler, APBx_CLK_Prescaler_t APB1_prescaler, APBx_CLK_Prescaler_t APB2_prescaler, uint8_t RTC_prescaler);
void set_SYS_MCO_config(SYS_CLK_Config_t* config, MCO1_CLK_Source_t MCO1_src, MCOx_CLK_Prescaler_t MCO1_prescaler, MCO2_CLK_Source_t MCO2_src, MCOx_CLK_Prescaler_t MCO2_prescaler);
void set_SYS_tick_config(SYS_CLK_Config_t* config, uint8_t enable, uint8_t enable_irq, sys_tick_t tick_handler);
void set_SYS_power_config(SYS_CLK_Config_t* config, SYS_Power_t power);
void sys_clock_init(SYS_CLK_Config_t* config);	// the config given might change when a config contradiction was solved

/*!< misc */
void delay_ms(uint64_t ms);

#endif
