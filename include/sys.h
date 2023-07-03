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

typedef void(*IRQ_callback_t)(void);


/*!< PLL */
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


/*!< RTC */
typedef enum {
	RCC_SRC_DISABLED =				0b00,	//R
	RTC_SRC_LSE =					0b01,
	RTC_SRC_LSI =					0b10,
	RTC_SRC_HSE =					0b11		// RTC_ck = HSE_ck / RTCPRE
} RTC_SRC_t;			// 2 bit


/*!< HSI */
typedef enum {
	HSI_DIV_1 = 					0b00,	//R	// 64 MHz
	HSI_DIV_2 = 					0b01,		// 32 MHz
	HSI_DIV_4 = 					0b10,		// 16 MHz
	HSI_DIV_8 = 					0b11		// 8 MHz
} HSI_DIV_t;			// 2 bit


/*!< SYS */
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


/*!< FLASH */
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

typedef enum {
	//================================================================================================================|
	// flash access latency |                                         AXI_freq (MHz)                                  |
	//     (table 17)       |                   VOS3               VOS2               VOS1            VOS0 (boot)     |
	//    in AXI cycles     |_________     0.95V - 1.05V      1.05V - 1.15V      1.15V - 1.26V      1.26V - 1.40V     |
	FLASH_PROG_DELAY0 =      0b00,  //     0 < AXI ≤ 45       0 < AXI ≤ 55       0 < AXI ≤ 70       0 < AXI ≤ 70      |
	FLASH_PROG_DELAY1 =      0b01,  //    45 < AXI ≤ 135     55 < AXI ≤ 165     70 < AXI ≤ 185     70 < AXI ≤ 185     |
	FLASH_PROG_DELAY2 =      0b10,  //   135 < AXI ≤ 225    165 < AXI ≤ 225    185 < AXI ≤ 225    185 < AXI ≤ 240     |
	//================================================================================================================|
} FLASH_PROG_DELAY_t;	// 2 bit


/*!< AHB/APB */
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


/*!< PERIPHERALS */
typedef enum {
	TIM_DIV_2 =						0b0,	//R	// rcc_hclk1 if D2PPREx is 1 or 2 else 2 x rcc_plckx_d2
	TIM_DIV_4 =						0b1			// rcc_hclk1 if D2PPREx is 1, 2 or 4 else 4 x rcc_plckx_d2
} TIM_PRE_t;			// 1 bit

typedef enum {
	HRTIM_SRC_APB2 =				0b0,	//R
	HRTIM_SRC_CPU =					0b1
} HRTIM_SRC_t;			// 1 bit

typedef enum {
	PER_SRC_HSI_KER =				0b00,	//R
	PER_SRC_CSI_KER =				0b01,
	PER_SRC_HSE =					0b10,
	PER_SRC_DISABLED =				0b11
} PER_SRC_t;			// 2 bit

typedef enum {
	SDMMC_CLK_SRC_PLL1_Q = 			0b0,	//R
	SDMMC_CLK_SRC_PLL2_R = 			0b1
} SDMMC_CLK_SRC_t;		// 1 bit

typedef enum {
	QSPI_CLK_SRC_AHB3 =				0b00,	//R
	QSPI_CLK_SRC_PLL1_Q =			0b01,
	QSPI_CLK_SRC_PLL2_R =			0b10,
	QSPI_CLK_SRC_PER =				0b11
} QSPI_CLK_SRC_t;		// 2 bit

typedef enum {
	FMC_CLK_SRC_AHB3 =				0b00,	//R
	FMC_CLK_SRC_PLL1_Q =			0b01,
	FMC_CLK_SRC_PLL2_R =			0b10,
	FMC_CLK_SRC_PER =				0b11
} FMC_CLK_SRC_t;		// 2 bit

typedef enum {
	SWPMI_CLK_SRC_APB1 =			0b0,	//R
	SWPMI_CLK_SRC_HSI_KER =			0b1
} SWPMI_CLK_SRC_t;		// 1 bit

typedef enum {
	FDCAN_CLK_SRC_HSE =				0b00,	//R
	FDCAN_CLK_SRC_PLL1_Q =			0b01,
	FDCAN_CLK_SRC_PLL2_Q =			0b10
} FDCAN_CLK_SRC_t;		// 2 bit

typedef enum {
	DFSDM1_CLK_SRC_APB2 =			0b0,	//R
	DFSDM1_CLK_SRC_SYS =			0b1
} DFSDM1_CLK_SRC_t;		// 1 bit

typedef enum {
	SPDIFRX_CLK_SRC_PLL1_Q =		0b00,	//R
	SPDIFRX_CLK_SRC_PLL2_R =		0b01,
	SPDIFRX_CLK_SRC_PLL3_R =		0b10,
	SPDIFRX_CLK_SRC_HSI_KER =		0b11
} SPDIFRX_CLK_SRC_t;	// 2 bit

typedef enum {
	SPI45_CLK_SRC_APB2 =			0b000,	//R
	SPI45_CLK_SRC_PLL2_Q =			0b001,
	SPI45_CLK_SRC_PLL3_Q =			0b010,
	SPI45_CLK_SRC_HSI_KER =			0b011,
	SPI45_CLK_SRC_CSI_KER =			0b100,
	SPI45_CLK_SRC_HSE =				0b101
} SPI45_CLK_SRC_t;		// 3 bit

typedef enum {
	SPI123_CLK_SRC_PLL1_Q =			0b000,	//R
	SPI123_CLK_SRC_PLL2_P =			0b001,
	SPI123_CLK_SRC_PLL3_P =			0b010,
	SPI123_CLK_SRC_I2S_CKIN =		0b011,
	SPI123_CLK_SRC_PER =			0b100
} SPI123_CLK_SRC_t;		// 3 bit

typedef enum {
	SAI_CLK_SRC_PLL1_Q =			0b000,	//R
	SAI_CLK_SRC_PLL2_P =			0b001,
	SAI_CLK_SRC_PLL3_P =			0b010,
	SAI_CLK_SRC_I2S_CKIN =			0b011,
	SAI_CLK_SRC_PER =				0b100
} SAI_CLK_SRC_t;		// 3 bit

typedef enum {
	LPTIM1_CLK_SRC_APB1 =			0b000,	//R
	LPTIM1_CLK_SRC_PLL2_P =			0b001,
	LPTIM1_CLK_SRC_PLL3_R =			0b010,
	LPTIM1_CLK_SRC_LSE =			0b011,
	LPTIM1_CLK_SRC_LSI =			0b100,
	LPTIM1_CLK_SRC_PER =			0b101
} LPTIM1_CLK_SRC_t;		// 3 bit

typedef enum {
	HDMI_CEC_CLK_SRC_LSE =			0b00,	//R
	HDMI_CEC_CLK_SRC_LSI =			0b01,
	HDMI_CEC_CLK_SRC_CSI_KER =		0b10,		// csi_ker_ck / 122
} HDMI_CEC_CLK_SRC_t;	// 2 bit

typedef enum {
	USB_OTG12_CLK_SRC_DISABLED =	0b00,	//R
	USB_OTG12_CLK_SRC_PLL1_Q =		0b01,
	USB_OTG12_CLK_SRC_PLL3_Q =		0b10,
	USB_OTG12_CLK_SRC_HSI48	=		0b11
} USB_OTG12_CLK_SRC_t;	// 2 bit

typedef enum {
	I2C123_CLK_SRC_APB1 =			0b00,	//R
	I2C123_CLK_SRC_PLL3_R =			0b01,
	I2C123_CLK_SRC_HSI_KER =		0b10,
	I2C123_CLK_SRC_CSI_KER =		0b11
} I2C123_CLK_SRC_t;		// 2 bit

typedef enum {
	RNG_CLK_SRC_HSI48 =				0b00,	//R
	RNG_CLK_SRC_PLL1_Q =			0b01,
	RNG_CLK_SRC_LSE =				0b10,
	RNG_CLK_SRC_LSI =				0b11
} RNG_CLK_SRC_t;		// 2 bit

typedef enum {
	USART16_CLK_SRC_APBx =			0b000,	//R
	USART16_CLK_SRC_PLL2_Q =		0b001,
	USART16_CLK_SRC_PLL3_Q =		0b010,
	USART16_CLK_SRC_HSI_KER =		0b011,
	USART16_CLK_SRC_CSI_KER =		0b100,
	USART16_CLK_SRC_LSE =			0b101
} USART_CLK_SRC_t;		// 3 bit

typedef enum {
	SPI6_CLK_SRC_APB4 =				0b000,	//R
	SPI6_CLK_SRC_PLL2_Q =			0b001,
	SPI6_CLK_SRC_PLL3_Q =			0b010,
	SPI6_CLK_SRC_HSI_KER =			0b011,
	SPI6_CLK_SRC_CSI_KER =			0b100,
	SPI6_CLK_SRC_HSE =				0b101
} SPI6_CLK_SRC_t;		// 3 bit

typedef enum {
	SAI4B_CLK_SRC_PLL1_Q =			0b000,	//R
	SAI4B_CLK_SRC_PLL2_P =			0b001,
	SAI4B_CLK_SRC_PLL3_P =			0b010,
	SAI4B_CLK_SRC_I2S_CKIN =		0b011,
	SAI4B_CLK_SRC_PER =				0b100
} SAI4_CLK_SRC_t;		// 3 bit

typedef enum {
	SAR_ADC_CLK_SRC_PLL2_P =		0b00,	//R
	SAR_ADC_CLK_SRC_PLL3_R =		0b01,
	SAR_ADC_CLK_SRC_PER =			0b10
} SAR_ADC_CLK_SRC_t;	// 2 bit

typedef enum {
	LPTIM2345_CLK_SRC_APB4 =		0b000,	//R
	LPTIM2345_CLK_SRC_PLL2_P =		0b001,
	LPTIM2345_CLK_SRC_PLL3_R =		0b010,
	LPTIM2345_CLK_SRC_LSE =			0b011,
	LPTIM2345_CLK_SRC_LSI =			0b100,
	LPTIM2345_CLK_SRC_PER =			0b101
} LPTIM2345_CLK_SRC_t;	// 3 bit

typedef enum {
	I2C4_CLK_SRC_APB4 =				0b00,	//R
	I2C4_CLK_SRC_PLL3_R =			0b01,
	I2C4_CLK_SRC_HSI_KER =			0b10,
	I2C4_CLK_SRC_CSI_KER =			0b11
} I2C4_CLK_SRC_t;		// 2 bit

typedef enum {
	LPUART1_CLK_SRC_APB4 =			0b000,	//R
	LPUART1_CLK_SRC_PLL2_Q =		0b001,
	LPUART1_CLK_SRC_PLL3_Q =		0b010,
	LPUART1_CLK_SRC_HSI_KER =		0b011,
	LPUART1_CLK_SRC_CSI_KER =		0b100,
	LPUART1_CLK_SRC_LSE =			0b101
} LPUART1_CLK_SRC_t;	// 3 bit


/*!< Systick */
typedef enum {	// R?
	SYSTICK_CLK_SRC_CPU_DIV_1 =		0b0,
	SYSTICK_CLK_SRC_CPU_DIV_8 =		0b1
} SYSTICK_CLK_SRC_t;	// 1 bit


/*!< MCO */
typedef enum {
	MCO2_CLK_SRC_SYS =				0b000,	//R
	MCO2_CLK_SRC_PLL2_P =			0b001,
	MCO2_CLK_SRC_HSE =				0b010,
	MCO2_CLK_SRC_PLL1_P =			0b011,
	MCO2_CLK_SRC_CSI =				0b100,
	MCO2_CLK_SRC_LSI =				0b101
} MCO2_CLK_SRC_t;		// 3 bit

typedef enum {
	MCO1_CLK_SRC_HSI =				0b000,	//R
	MCO1_CLK_SRC_LSE =				0b001,
	MCO1_CLK_SRC_HSE =				0b010,
	MCO1_CLK_SRC_PLL1_Q =			0b011,
	MCO1_CLK_SRC_HSI48 =			0b100,
} MCO1_CLK_SRC_t;		// 3 bit


/*!< config struct */
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
	uint64_t			HSI_div					: 2;	// HSI_DIV_t
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
	// FLASH
	uint64_t			FLASH_program_delay		: 2;	// FLASH_PROG_DELAY_t
	uint64_t			FLASH_latency			: 4;	// FLASH_LATENCY_t
	// Domains
	uint64_t			APB3_prescaler			: 3;	// APB_CLK_PRE_t	domain 1
	uint64_t			AHB_prescaler			: 4;	// AHB_CLK_PRE_t	AHB/AXI clocks
	uint64_t			APB2_prescaler			: 3;	// APB_CLK_PRE_t	domain 2
	uint64_t			APB1_prescaler			: 3;	// APB_CLK_PRE_t	domain 2
	uint64_t			APB4_prescaler			: 3;	// APB_CLK_PRE_t	domain 3
	// Peripherals
	uint64_t			TIM_prescaler			: 1;	// TIM_PRE_t
	uint64_t			HRTIM_src				: 1;	// HRTIM_SRC_t
	uint64_t			PER_src					: 2;	// PER_SRC_t
	uint64_t			SDMMC_CLK_src			: 1;	// SDMMC_CLK_SRC_t
	uint64_t			QSPI_CLK_src			: 2;	// QSPI_CLK_SRC_t
	uint64_t			FMC_CLK_src				: 2;	// FMC_CLK_SRC_t
	uint64_t			SWPMI_CLK_src			: 1;	// SWPMI_CLK_SRC_t
	uint64_t			FDCAN_CLK_src			: 2;	// FDCAN_CLK_SRC_t
	uint64_t			DFSDM1_CLK_src			: 1;	// DFSDM1_CLK_SRC_t
	uint64_t			SPDIFRX_CLK_src			: 2;	// SPDIFRX_CLK_SRC_t
	uint64_t			SPI45_CLK_src			: 3;	// SPI45_CLK_SRC_t
	uint64_t			SPI123_CLK_src			: 3;	// SPI123_CLK_SRC_t
	uint64_t			SAI23_CLK_src			: 3;	// SAI_CLK_SRC_t
	uint64_t			SAI1_CLK_src			: 3;	// SAI_CLK_SRC_t
	uint64_t			LPTIM1_CLK_src			: 3;	// LPTIM1_CLK_SRC_t
	uint64_t			HDMI_CEC_CLK_src		: 2;	// HDMI_CEC_CLK_SRC_t
	uint64_t			USB_OTG12_CLK_src		: 2;	// USB_OTG12_CLK_SRC_t
	uint64_t			I2C123_CLK_src			: 2;	// I2C123_CLK_SRC_t
	uint64_t			RNG_CLK_src				: 2;	// RNG_CLK_SRC_t
	uint64_t			USART16_CLK_src			: 3;	// USART_CLK_SRC_t
	uint64_t			USART234578_CLK_src		: 3;	// USART_CLK_SRC_t
	uint64_t			SPI6_CLK_src			: 3;	// SPI6_CLK_SRC_t
	uint64_t			SAR_ADC_CLK_src			: 2;	// SAR_ADC_CLK_SRC_t
	uint64_t			I2C4_CLK_src			: 2;	// I2C4_CLK_SRC_t
	uint32_t			SAI4B_CLK_src			: 3;	// SAI4_CLK_SRC_t
	uint32_t			SAI4A_CLK_src			: 3;	// SAI4_CLK_SRC_t
	uint32_t			LPTIM345_CLK_src		: 3;	// LPTIM2345_CLK_SRC_t
	uint32_t			LPTIM2_CLK_src			: 3;	// LPTIM2345_CLK_SRC_t
	uint32_t			LPUART1_CLK_src			: 3;	// LPUART1_CLK_SRC_t
	// Systick
	uint32_t			SYSTICK_enable			: 1;
	uint32_t			SYSTICK_IRQ_enable		: 1;
	uint32_t			SYSTICK_CLK_src			: 1;	// SYSTICK_CLK_SRC_t
	// MCO
	uint32_t			MCO2_CLK_src			: 3;	// MCO2_CLK_SRC_t
	uint32_t			MCO2_CLK_prescaler		: 4;
	uint32_t			MCO1_CLK_src			: 3;	// MCO1_CLK_SRC_t
	uint32_t			MCO1_CLK_prescaler		: 4;
} SYS_CLK_Config_t;		// 352 bit -> 44 byte


/*!< constants */
extern const uint32_t LSI_clock_frequency;
extern const uint32_t LSE_clock_frequency;
extern const uint32_t CSI_clock_frequency;


/*!< variables */  // values are updated when calling sys_clock_init
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

extern uint32_t AHB_clock_frequency;
extern uint32_t APB1_clock_frequency;
extern uint32_t APB2_clock_frequency;
extern uint32_t APB3_clock_frequency;
extern uint32_t APB4_clock_frequency;

extern uint32_t RTC_clock_frequency;

extern uint32_t SYS_clock_frequency;

extern volatile uint64_t tick;  // updated with sys_tick


/*!< interrupts */
extern void SysTick_Handler(void);
extern void RCC_IRQHandler(void);

/*!< config functions */
SYS_CLK_Config_t* new_SYS_CLK_config(void);
void set_PLL_config(
	PLL_CLK_Config_t* config,				uint8_t enable,						uint8_t P_enable,
	uint8_t Q_enable,						uint8_t R_enable,					uint8_t frac_enable,
	PLL_IN_t input_range,					PLL_VCO_t VCO_range,				uint8_t M_factor,
	uint8_t P_factor,						uint8_t Q_factor,					uint8_t R_factor,
	uint16_t N_factor,						uint16_t N_fraction
);
void set_RTC_config(
	SYS_CLK_Config_t* config,				uint8_t RTC_enable,
	RTC_SRC_t RTC_src,						uint8_t RTC_HSE_prescaler
);
void set_clock_config(
	SYS_CLK_Config_t* config,				uint8_t HSI_enable,					uint8_t HSE_enable,
	uint8_t LSI_enable,						uint8_t LSE_enable,					uint8_t CSI_enable,
	uint8_t HSI48_enable,					uint8_t HSI_enable_stop_mode,		uint8_t CSI_enable_stop_mode,
	uint8_t HSE_CSS_enable,					uint8_t LSE_CSS_enable,				HSI_DIV_t HSI_div,
	uint32_t HSE_freq
);
void set_SYS_config(
	SYS_CLK_Config_t* config,				SYS_CLK_SRC_t SYS_CLK_src,			SYS_CLK_PRE_t SYS_CLK_prescaler,
	FLASH_PROG_DELAY_t FLASH_program_delay,	FLASH_LATENCY_t FLASH_latency
);
void set_domain_config(
	SYS_CLK_Config_t* config,				AHB_CLK_PRE_t AHB_prescaler,		APB_CLK_PRE_t APB1_prescaler,
	APB_CLK_PRE_t APB2_prescaler,			APB_CLK_PRE_t APB3_prescaler,		APB_CLK_PRE_t APB4_prescaler
);
void set_systick_config(
	SYS_CLK_Config_t* config,				uint8_t SYSTICK_enable,
	uint8_t SYSTICK_IRQ_enable,				SYSTICK_CLK_SRC_t SYSTICK_CLK_src
);
void set_MCO_config(
	SYS_CLK_Config_t* config,				MCO1_CLK_SRC_t MCO1_CLK_src,			uint8_t MCO1_CLK_prescaler,
	MCO2_CLK_SRC_t MCO2_CLK_src,			uint8_t MCO2_CLK_prescaler
);
// TODO: peripheral kernel clocks
/*!< setup functions */
void IRQ_callback_init(IRQ_callback_t sys_tick_callback, IRQ_callback_t clock_fault_callback);
void sys_clock_init(SYS_CLK_Config_t* config);

/*!< misc */
void delay_ms(uint64_t ms);


#endif //STM32H_CMSIS_SYS_H
