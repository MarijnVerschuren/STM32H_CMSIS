#include "main.h"
#include "sys.h"
#include "gpio.h"


#if defined(STM32H7xx)
SYS_CLK_Config_t* sys_config;

void sys_init(void);


int main(void) {
	sys_init();  // clock config

	for(;;) {

	}
}


void sys_init(void) {
	sys_config = new_SYS_CLK_config();
	set_PLL_config(
			&sys_config->PLL1_config, 1, 1, 1, 0, 0,		// enable PLL1 (P, Q)
			PLL_IN_4MHz_8MHz, PLL_VCO_WIDE,					// 5MHz in, 192MHz < VCO < 960MHz
			5, 2, 16, 2, 160, 0								// M = 5, P = 2, Q = 16, R = 2, N = 160, N_frac = 0
	);  // 25MHz / 5 * 160 / (2, 16, 2)
	set_PLL_config(
			&sys_config->PLL2_config, 1, 1, 0, 1, 0,		// enable PLL2 (P, R)
			PLL_IN_4MHz_8MHz, PLL_VCO_WIDE,					// 5MHz in, 192MHz < VCO < 960MHz
			5, 4, 2, 4, 160, 0								// M = 5, P = 4, Q = 2, R = 4, N = 160, N_frac = 0
	);  // 25MHz / 5 * 160 / (4, 2, 4)
	set_PLL_config(
			&sys_config->PLL3_config, 0, 0, 0, 0, 0,		// disable PLL3
			PLL_IN_4MHz_8MHz, PLL_VCO_WIDE,
			5, 2, 2, 2, 160, 0
	);  // 25MHz / 5 * 160 / (2, 2, 2)
	set_RTC_config(sys_config, 0, RCC_SRC_DISABLED, 0);	// disable RTC
	set_clock_config(
			sys_config, 0, 1, 0, 0, 0, 0,					// enable HSE
			0, 0, 1, 0, HSI_DIV_1, 25000000					// enable HSE_CSS, HSE_freq = 25MHz
	);
	set_SYS_config(
			sys_config, SYS_CLK_SRC_PLL1_P, SYS_CLK_DIV_1,	// SYS_CLK = PLL1_P
			FLASH_LATENCY4, FLASH_PROG_DELAY2				// FLASH settings for AXI_freq = 200MHz
	);
	set_domain_config(
			sys_config, AHB_CLK_DIV_2, APB_CLK_DIV_2,		// AHB and AXI = 200MHz
			APB_CLK_DIV_2, APB_CLK_DIV_2, APB_CLK_DIV_2		// APB1-4 = 100MHz
	);
	set_systick_config(
			sys_config, 1, 1, SYSTICK_CLK_SRC_CPU_DIV_1		// SysTick (IRQ) enable at 400MHz
	);
	set_MCO_config(
			sys_config, MCO1_CLK_SRC_PLL1_Q, 1,				// output PLL1_Q	(50MHz)
			MCO2_CLK_SRC_SYS, 1								// output SYS_CLK	(400MHz)
	);
	sys_clock_init(sys_config);
}
#endif