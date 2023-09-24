#include "gpio.h"
#include "exti.h"
#include "tim.h"
#include "pwm.h"
#include "encoder.h"
#include "usart.h"
#include "i2c.h"
#include "usb.h"
#include "crc.h"
#include "rng.h"
#include "watchdog.h"
#include "mco.h"


#if defined(STM32H7xx)
SYS_CLK_Config_t* sys_config;


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
			2/*M*/, 2/*P*/, 4/*Q*/, 2/*R*/, 64/*N*/, 0		// M = 2, P = 2, Q = 4, R = 2, N = 64, N_frac = 0
	);  // 25MHz / 2 * 64 / (2, 4, 2)
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
			sys_config, 0, 1, 0, 0, 0, 1,					// disable HSI, enable HSE, enable HSI48
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
	sys_clock_init(sys_config);

	// TIM config
	config_TIM_kernel_clocks(
			TIM_MUL_2, HRTIM_SRC_CPU, LPTIM_CLK_SRC_APBx,
			LPTIM_CLK_SRC_APBx, LPTIM_CLK_SRC_APBx
	);
	config_TIM(TIM8, TIM_APB2_kernel_frequency / 10000, 10000);  // 1 Hz
	start_TIM_update_irq(TIM8);
	start_TIM(TIM8);

	// GPIO config
	config_GPIO(GPIOE, 1, GPIO_output, GPIO_no_pull, GPIO_push_pull);
	config_GPIO(GPIOD, 9, GPIO_input, GPIO_pull_up, GPIO_open_drain);

	// EXTI config
	config_EXTI(9, GPIOD, 1, 1);
	start_EXTI(9);

	// MCO config
	config_MCO(MCO2_C9, MCO2_SRC_PLL1_P, 1);  // 400 MHz

	// PWM config
	config_PWM(TIM1_CH1_A8, TIM_APB2_kernel_frequency / 1000000, 20000);  // 50Hz

	// CRC config
	config_CRC();

	// HASH config
	// TODO: [3]

	// CRYP config
	// TODO: [4]

	// RNG config
	config_RNG_kernel_clock(RNG_CLK_SRC_PLL1_Q);  // 200 MHz
	// TODO: [5]

	// UART config
	config_USART_kernel_clocks(USART_CLK_SRC_APBx, USART_CLK_SRC_APBx, USART_CLK_SRC_APBx);
	config_UART(USART1_TX_A9, USART1_RX_A10, 115200, 1);

	// I2C config
	config_I2C_kernel_clocks(I2C_CLK_SRC_APBx, I2C_CLK_SRC_APBx);
	I2C_setting_t I2C_setting = {  // 100 KHz
			APB1_clock_frequency / 4000000,
			0x13U, 0x0FU, 2, 4
	};
	config_I2C(I2C1_SCL_B6, I2C1_SDA_B7, I2C_setting, 0x50);

	// USB config
	config_USB_kernel_clock(USB_CLK_SRC_HSI48);  // HSI48 is solely used for USB
	// TODO: [2]
	volatile dev_id_t id1 = dev_to_id(USB1_OTG_HS);
	volatile dev_id_t id2 = dev_to_id(USB2_OTG_FS);
	(void)id1; (void)id2;

	// Watchdog config (32kHz / (4 << prescaler))
	config_watchdog(0, 0xFFFUL);	// 1s
	//start_watchdog();


	// main loop
	for(;;) {
		//TIM1->CCR1 = (TIM1->CCR1 + 100) % 20000;
		//UART_print(USART1, "Hello World!\n", 100);
		//reset_watchdog();
		delay_ms(10);
	}


	/*!< ROM code */
	//uint8_t tx_data[5] = {0x50, 0x21, 0x85, 0xA3, 0x1C};
	//uint8_t rx_data[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
	//I2C_master_write_reg(I2C1, 0x50, 0x1234, I2C_REG_16, tx_data, 5, 100);
	//I2C_master_read_reg(I2C1, 0x50, 0x1234, I2C_REG_16, rx_data, 5, 100);
}
#endif