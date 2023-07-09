#include "main.h"
#include "sys.h"
#include "gpio.h"
#include "mco.h"
#include "tim.h"
#include "exti.h"
#include "pwm.h"


#if defined(STM32H7xx)
SYS_CLK_Config_t* sys_config;

/*
(void)HSI_clock_frequency;
(void)HSE_clock_frequency;
(void)PLL1_P_clock_frequency;
(void)PLL1_Q_clock_frequency;
(void)PLL1_R_clock_frequency;
(void)PLL2_P_clock_frequency;
(void)PLL2_Q_clock_frequency;
(void)PLL2_R_clock_frequency;
(void)AHB_clock_frequency;
(void)APB1_clock_frequency;
(void)APB2_clock_frequency;
(void)APB3_clock_frequency;
(void)APB4_clock_frequency;
(void)SYS_clock_frequency;
*/


extern void TIM8_UP_TIM13_IRQHandler(void) {
	TIM8->SR &= ~TIM_SR_UIF;  // clear interrupt flag
	GPIO_toggle(GPIOE, 1);
}

extern void EXTI9_5_IRQHandler(void) {
	EXTI->PR1 |= EXTI_PR1_PR9;  // clear interrupt flag
	GPIO_toggle(GPIOE, 1);
}


int main(void) {
	// clock config
	sys_config = new_SYS_CLK_config();
	set_PLL_config(
			&sys_config->PLL1_config, 1, 1, 1, 0, 0,		// enable PLL1 (P, Q)
			PLL_IN_8MHz_16MHz, PLL_VCO_WIDE,				// 12.5MHz in, 192MHz < VCO < 960MHz
			2/*M*/, 2/*P*/, 16/*Q*/, 2/*R*/, 64/*N*/, 0		// M = 2, P = 2, Q = 16, R = 2, N = 64, N_frac = 0
	);  // 25MHz / 2 * 64 / (2, 16, 2)
	set_PLL_config(
			&sys_config->PLL2_config, 1, 1, 0, 1, 0,		// enable PLL2 (P, R)
			PLL_IN_8MHz_16MHz, PLL_VCO_WIDE,				// 12.5MHz in, 192MHz < VCO < 960MHz
			2/*M*/, 4/*P*/, 2/*Q*/, 4/*R*/, 64/*N*/, 0		// M = 2, P = 2, Q = 2, R = 2, N = 64, N_frac = 0
	);  // 25MHz / 2 * 64 / (4, 2, 4)
	set_PLL_config(
			&sys_config->PLL3_config, 0, 0, 0, 0, 0,		// disable PLL3
			PLL_IN_4MHz_8MHz, PLL_VCO_WIDE,
			2/*M*/, 2/*P*/, 2/*Q*/, 2/*R*/, 64/*N*/, 0
	);  // 25MHz / 2 * 64 / (2, 2, 2)
	set_RTC_config(sys_config, 0, RCC_SRC_DISABLED, 0);		// disable RTC
	set_clock_config(
			sys_config, 0, 1, 0, 0, 0, 0,					// disable HSI, enable HSE
			0, 0, 1, 0, HSI_DIV_1, 25000000,				// enable HSE_CSS, HSE_freq = 25MHz
			PLL_SRC_HSE										// set HSE as PLL source clock
	);
	set_SYS_config(
			sys_config, SYS_CLK_SRC_PLL1_P, SYS_CLK_DIV_1,	// SYS_CLK = PLL1_P
			CORE_VOS_1, FLASH_LATENCY3						// FLASH and PWR settings for AXI_freq = 200MHz, CORE_freq = 400MHz
	);
	set_domain_config(
			sys_config, AHB_CLK_DIV_2, APB_CLK_DIV_2,		// AHB and AXI = 200MHz
			APB_CLK_DIV_2, APB_CLK_DIV_2, APB_CLK_DIV_2		// APB1-4 = 100MHz
	);
	set_systick_config(
			sys_config, 1, 1, SYSTICK_CLK_SRC_AXI_CLK_DIV_1	// SysTick (IRQ) enable at 200MHz
	);
	set_MCO_config(
			sys_config, MCO1_CLK_SRC_PLL1_Q, 1,				// output PLL1_Q	(50MHz)
			MCO2_CLK_SRC_PLL1_P, 1							// output PLL1_P	(400MHz)
	);
	sys_clock_init(sys_config);

	// MCO config
	config_MCO(MCO1_A8);
	//config_MCO(MCO2_C9);

	// GPIO config
	config_GPIO(GPIOE, 1, GPIO_output, GPIO_no_pull, GPIO_push_pull);
	config_GPIO(GPIOD, 9, GPIO_input, GPIO_pull_up, GPIO_open_drain);

	// EXTI config
	config_EXTI(9, GPIOD, 1, 1);
	start_EXTI(9);

	// TIM config
	config_TIM(TIM8, 20000, 10000);
	start_TIM_update_irq(TIM8);
	start_TIM(TIM8);

	// PWM config
	config_PWM(TIM1_CH1_A8, 200, 20000);  // 50Hz


	// main loop
	for(;;) {
		TIM1->CCR1 = (TIM1->CCR1 + 100) % 20000;
		delay_ms(100);
	}
}
#endif