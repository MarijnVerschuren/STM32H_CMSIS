//
// Created by marijn on 6/26/23.
//

#ifndef STM32H_CMSIS_SYS_H
#define STM32H_CMSIS_SYS_H
#include "main.h"


/*!<
 * types
 * */
typedef struct {
	uint32_t ID0;
	uint32_t ID1;
	uint32_t ID2;
} ID;
#define UID ((ID *)UID_BASE)

typedef void(*IRQ_callback_t)(void);


//!< PLL
typedef enum {
	PLL_IN_1MHz_2MHz =				0b00,	//R	// 1 MHz < CLK_IN < 2 MHz
	PLL_IN_2MHz_4MHz =				0b01,		// 2 MHz < CLK_IN < 4 MHz
	PLL_IN_4MHz_8MHz =				0b10,		// 4 MHz < CLK_IN < 8 MHz
	PLL_IN_8MHz_16MHz =				0b11		// 8 MHz < CLK_IN < 16 MHz
} PLL_IN_t;				// 2 bit

typedef enum {
	PLL_VCO_WIDE =					0b00,	//R	// 192 MHz < PLL_CLK < 960 MHz
	PLL_VCO_MEDIUM =				0b01		// 150 MHz < PLL_CLK < 420 MHz
} PLL_VCO_t;			// 2 bit

typedef enum {
	PLL_SRC_HSI =					0b00,	//R
	PLL_SRC_CSI =					0b01,
	PLL_SRC_HSE =					0b10
} PLL_SRC_t;			// 2 bit

typedef struct {
	uint64_t	enable		: 1;
	uint64_t	P_enable	: 1;
	uint64_t	Q_enable	: 1;
	uint64_t	R_enable	: 1;
	uint64_t	frac_enable	: 1;
	uint64_t	input_range	: 2;	// PLL_IN_t
	uint64_t	VCO_range	: 1;	// PLL_VCO_t
	uint64_t	M_factor	: 6;
	uint64_t	P_factor	: 7;	// (+1)
	uint64_t	Q_factor	: 7;	// (+1)
	uint64_t	R_factor	: 7;	// (+1)
	uint64_t	N_factor	: 9;	// (+1)
	uint64_t	N_fraction	: 13;	// PLL_IN * (N_factor (+1) + (N_fraction / 2^13))
	uint64_t	_			: 7;
} PLL_CLK_Config_t;		// 64 bit (8 bit reserved)


//!< RTC
typedef enum {
	RCC_SRC_DISABLED =				0b00,	//R
	RTC_SRC_LSE =					0b01,
	RTC_SRC_LSI =					0b10,
	RTC_SRC_HSE =					0b11		// RTC_ck = HSE_ck / RTCPRE
} RTC_SRC_t;			// 2 bit


//!< HSI
typedef enum {
	HSI_DIV_1 = 					0b00,	//R	// 64 MHz
	HSI_DIV_2 = 					0b01,		// 32 MHz
	HSI_DIV_4 = 					0b10,		// 16 MHz
	HSI_DIV_8 = 					0b11		// 8 MHz
} HSI_DIV_t;			// 2 bit


//!< SYS
typedef enum {
	SYS_CLK_SRC_HSI =				0b00,	//R
	SYS_CLK_SRC_CSI =				0b01,
	SYS_CLK_SRC_HSE =				0b10,
	SYS_CLK_SRC_PLL1_P =			0b11
} SYS_CLK_SRC_t;		// 2 bit

typedef enum {
	SYS_CLK_DIV_1 =					0b0000,	//R	// 0xxx
	SYS_CLK_DIV_2 =					0b1000,
	SYS_CLK_DIV_4 =					0b1001,
	SYS_CLK_DIV_8 =					0b1010,
	SYS_CLK_DIV_16 =				0b1011,
	SYS_CLK_DIV_64 =				0b1100,
	SYS_CLK_DIV_128 =				0b1101,
	SYS_CLK_DIV_256 =				0b1110,
	SYS_CLK_DIV_512 =				0b1111
} SYS_CLK_PRE_t;		// 4 bit


//!< PWR
typedef enum {
	CORE_VOS_3 =					0b01,	//R
	CORE_VOS_2 =					0b10,
	CORE_VOS_1 =					0b11
} CORE_VOS_t;


//!< FLASH
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
} FLASH_LATENCY_t;		// 3 bit


//!< AHB/APB
typedef enum {
	AHB_CLK_DIV_1 =					0b0000,	//R	// 0xxx
	AHB_CLK_DIV_2 =					0b1000,
	AHB_CLK_DIV_4 =					0b1001,
	AHB_CLK_DIV_8 =					0b1010,
	AHB_CLK_DIV_16 =				0b1011,
	AHB_CLK_DIV_64 =				0b1100,
	AHB_CLK_DIV_128 =				0b1101,
	AHB_CLK_DIV_256 =				0b1110,
	AHB_CLK_DIV_512 =				0b1111
} AHB_CLK_PRE_t;		// 4 bit

typedef enum {
	APB_CLK_DIV_1 =					0b000,	//R	// 0xx
	APB_CLK_DIV_2 =					0b100,
	APB_CLK_DIV_4 =					0b101,
	APB_CLK_DIV_8 =					0b110,
	APB_CLK_DIV_16 =				0b111,
} APB_CLK_PRE_t;		// 3 bit


//!< PERIPHERALS
typedef enum {
	PER_SRC_HSI =					0b00,	//R
	PER_SRC_CSI =					0b01,
	PER_SRC_HSE =					0b10,
	PER_SRC_DISABLED =				0b11
} PER_SRC_t;			// 2 bit


//!< SysTick
typedef enum {
	SYSTICK_CLK_SRC_AXI_CLK_DIV_8 =	0b0,	//R
	SYSTICK_CLK_SRC_AXI_CLK_DIV_1 =	0b1
} SYSTICK_CLK_SRC_t;	// 1 bit


//!< config struct
typedef struct {
	// PLL
	PLL_CLK_Config_t	PLL1_config;
	PLL_CLK_Config_t	PLL2_config;
	PLL_CLK_Config_t	PLL3_config;
	uint64_t			PLL_src					: 2;	// PLL_SRC_t
	// RTC
	uint64_t			RTC_enable				: 1;
	uint64_t 			RTC_src					: 2;	// RTC_SRC_t
	uint64_t			RTC_HSE_prescaler		: 6;
	// HSI
	uint64_t			HSI_enable				: 1;
	uint64_t			HSI_div					: 2;	// HSI_DIV_t  // TODO: this is NOT used??????
	uint64_t			HSI_enable_stop_mode	: 1;
	// HSE
	uint64_t			HSE_enable				: 1;
	uint64_t			HSE_CSS_enable			: 1;
	uint64_t 			HSE_freq				: 26;	// [4, 48] MHz
	// LSI
	uint64_t			LSI_enable				: 1;
	// LSE
	uint64_t			LSE_enable				: 1;
	uint64_t			LSE_CSS_enable			: 1;	// enables interrupt automatically
	// CSI
	uint64_t			CSI_enable				: 1;
	uint64_t			CSI_enable_stop_mode	: 1;
	// HSI48
	uint64_t			HSI48_enable			: 1;
	// SYS
	uint64_t			SYS_CLK_src				: 2;	// SYS_CLK_SRC_t
	uint64_t			SYS_CLK_prescaler		: 4;	// SYS_CLK_PRE_t
	// PWR
	uint64_t			CORE_VOS_level			: 2;	// CORE_VOS_t
	// FLASH
	uint64_t			FLASH_latency			: 4;	// FLASH_LATENCY_t
	// Domains
	uint64_t			APB3_prescaler			: 3;	// APB_CLK_PRE_t	domain 1
	uint32_t			AHB_prescaler			: 4;	// AHB_CLK_PRE_t	AHB/AXI clocks
	uint32_t			APB2_prescaler			: 3;	// APB_CLK_PRE_t	domain 2
	uint32_t			APB1_prescaler			: 3;	// APB_CLK_PRE_t	domain 2
	uint32_t			APB4_prescaler			: 3;	// APB_CLK_PRE_t	domain 3
	uint32_t			PER_src					: 2;	// PER_SRC_t
	// Systick
	uint32_t			SYSTICK_enable			: 1;
	uint32_t			SYSTICK_IRQ_enable		: 1;
	uint32_t			SYSTICK_CLK_src			: 1;	// SYSTICK_CLK_SRC_t
} SYS_CLK_Config_t;		// 288 bit (14 bit reserved)


/*!<
 * constants
 * */
extern const uint32_t LSI_clock_frequency;
extern const uint32_t LSE_clock_frequency;
extern const uint32_t CSI_clock_frequency;
extern const uint32_t HSI48_clock_frequency;


/*!<
 * variables
 * */
extern uint32_t HSI_clock_frequency;
extern uint32_t HSE_clock_frequency;
extern uint32_t PLL1_P_clock_frequency;
extern uint32_t PLL1_Q_clock_frequency;
extern uint32_t PLL1_R_clock_frequency;
extern uint32_t PLL2_P_clock_frequency;
extern uint32_t PLL2_Q_clock_frequency;
extern uint32_t PLL2_R_clock_frequency;
extern uint32_t PLL3_P_clock_frequency;
extern uint32_t PLL3_Q_clock_frequency;
extern uint32_t PLL3_R_clock_frequency;
extern uint32_t PER_clock_frequency;
extern uint32_t AHB_clock_frequency;
extern uint32_t APB1_clock_frequency;
extern uint32_t APB2_clock_frequency;
extern uint32_t APB3_clock_frequency;
extern uint32_t APB4_clock_frequency;
extern uint32_t RTC_clock_frequency;
extern uint32_t SYS_clock_frequency;
extern volatile uint64_t tick;  // updated with sys_tick


/*!<
 * interrupts
 * */
extern void SysTick_Handler(void);
extern void RCC_IRQHandler(void);


/*!<
 * config functions
 * */
SYS_CLK_Config_t* new_SYS_CLK_config(void);
void set_PLL_config(
	PLL_CLK_Config_t* config,				uint8_t enable,							uint8_t P_enable,
	uint8_t Q_enable,						uint8_t R_enable,						uint8_t frac_enable,
	PLL_IN_t input_range,					PLL_VCO_t VCO_range,					uint8_t M_factor,
	uint8_t P_factor,						uint8_t Q_factor,						uint8_t R_factor,
	uint16_t N_factor,						uint16_t N_fraction
);
void set_RTC_config(
	SYS_CLK_Config_t* config,				uint8_t RTC_enable,
	RTC_SRC_t RTC_src,						uint8_t RTC_HSE_prescaler
);
void set_clock_config(
	SYS_CLK_Config_t* config,				uint8_t HSI_enable,						uint8_t HSE_enable,
	uint8_t LSI_enable,						uint8_t LSE_enable,						uint8_t CSI_enable,
	uint8_t HSI48_enable,					uint8_t HSI_enable_stop_mode,			uint8_t CSI_enable_stop_mode,
	uint8_t HSE_CSS_enable,					uint8_t LSE_CSS_enable,					HSI_DIV_t HSI_div,
	uint32_t HSE_freq,						PLL_SRC_t PLL_src
);
void set_SYS_config(
	SYS_CLK_Config_t* config,				SYS_CLK_SRC_t SYS_CLK_src,				SYS_CLK_PRE_t SYS_CLK_prescaler,
	CORE_VOS_t CORE_VOS_level,				FLASH_LATENCY_t FLASH_latency
);
void set_domain_config(
	SYS_CLK_Config_t* config,				AHB_CLK_PRE_t AHB_prescaler,			APB_CLK_PRE_t APB1_prescaler,
	APB_CLK_PRE_t APB2_prescaler,			APB_CLK_PRE_t APB3_prescaler,			APB_CLK_PRE_t APB4_prescaler
);
void set_systick_config(
	SYS_CLK_Config_t* config,				uint8_t SYSTICK_enable,
	uint8_t SYSTICK_IRQ_enable,				SYSTICK_CLK_SRC_t SYSTICK_CLK_src
);


/*!<
 * init
 * */
void IRQ_callback_init(IRQ_callback_t sys_tick_callback, IRQ_callback_t clock_fault_callback);
void sys_clock_init(SYS_CLK_Config_t* config);


/*!<
 * misc
 * */
void delay_ms(uint64_t ms);


#endif //STM32H_CMSIS_SYS_H
