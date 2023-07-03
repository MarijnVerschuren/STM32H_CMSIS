#include "main.h"
#include "sys.h"
#include "gpio.h"


#if defined(STM32H7xx)
SYS_CLK_Config_t* sys_config;


int main(void) {
	sys_config = new_SYS_CLK_config();
	set_PLL_config(
		&sys_config->PLL1_config, 1, 1, 1, 0, 0,
		PLL_IN_4MHz_8MHz, PLL_VCO_WIDE, 5, 2,
		16, 2, 160, 0
	);  // 25MHz / 5 * 160 / (2, 16, 2)
	set_PLL_config(
		&sys_config->PLL2_config, 1, 1, 0, 1, 0,
		PLL_IN_4MHz_8MHz, PLL_VCO_WIDE, 5, 4,
		2, 4, 160, 0
	);  // 25MHz / 5 * 160 / (4, 2, 4)
	set_PLL_config(
		&sys_config->PLL3_config, 0, 0, 0, 0, 0,
		PLL_IN_4MHz_8MHz, PLL_VCO_WIDE, 5, 2,
		2, 2, 160, 0
	);  // 25MHz / 5 * 160 / (2, 2, 2)

	sys_clock_init(sys_config);
}

/*
Error during pretty printers setup: Undefined info command: "pretty-printer". Try "help info".
Some features and performance optimizations will not be available.
*/

#endif