//
// Created by marijn on 7/15/23.
//

#include "i2c.h"


/*!<
 * variables
 * */
uint32_t I2C123_kernel_frequency =	0;
uint32_t I2C4_kernel_frequency =	0;


/*!<
 * static
 * */
static inline uint8_t I2C_freq(dev_clock_id_t clk, uint32_t freq) {
	uint32_t ker_clk_freq = 0;
	switch (clk) {
		case DEV_CLOCK_ID_APB1:	ker_clk_freq = I2C123_kernel_frequency;
		case DEV_CLOCK_ID_APB4: ker_clk_freq = I2C4_kernel_frequency;
	}
	// TODO: add freq and scl H/L arguments to config func
	// p.g. 1983

	return 0;
}


/*!<
 * init
 * */
void config_I2C_kernel_clocks(I2C_CLK_SRC_t i2c123_src, I2C_CLK_SRC_t i2c4_src) {
	RCC->D2CCIP2R &= ~RCC_D2CCIP2R_I2C123SEL;
	RCC->D2CCIP2R |= i2c123_src << RCC_D2CCIP2R_I2C123SEL_Pos;
	RCC->D3CCIPR &= ~RCC_D3CCIPR_I2C4SEL;
	RCC->D3CCIPR |= i2c4_src << RCC_D3CCIPR_I2C4SEL_Pos;
	switch (i2c123_src) {
		case I2C_CLK_SRC_APBx:		I2C123_kernel_frequency = APB1_clock_frequency; break;	// APB1
		case I2C_CLK_SRC_PLL3_R:	I2C123_kernel_frequency = PLL3_R_clock_frequency; break;
		case I2C_CLK_SRC_HSI:		I2C123_kernel_frequency = HSI_clock_frequency; break;
		case I2C_CLK_SRC_CSI:		I2C123_kernel_frequency = CSI_clock_frequency; break;
	}	switch (i2c4_src) {
		case I2C_CLK_SRC_APBx:		I2C4_kernel_frequency = APB4_clock_frequency; return;	// APB4
		case I2C_CLK_SRC_PLL3_R:	I2C4_kernel_frequency = PLL3_R_clock_frequency; return;
		case I2C_CLK_SRC_HSI:		I2C4_kernel_frequency = HSI_clock_frequency; return;
		case I2C_CLK_SRC_CSI:		I2C4_kernel_frequency = CSI_clock_frequency; return;
	}
}
void fconfig_I2C(I2C_GPIO_t scl, I2C_GPIO_t sda, uint16_t own_address, I2C_address_t address_type, uint8_t dual_address) {
	if (scl == I2C_PIN_DISABLE || sda == I2C_PIN_DISABLE) { return; }
	dev_pin_t	scl_dev = *((dev_pin_t*)&scl),			sda_dev = *((dev_pin_t*)&sda);
	I2C_TypeDef	*scl_i2c = id_to_dev(scl_dev.dev_id),	*sda_i2c = id_to_dev(sda_dev.dev_id),	*i2c = NULL;
	if (scl_i2c != sda_i2c) { return; }
	i2c = scl_i2c;


}

void config_I2C(I2C_GPIO_t scl, I2C_GPIO_t sda, uint8_t own_address) {
	fconfig_I2C(scl, sda, own_address, I2C_ADDR_7BIT, 0);
}
