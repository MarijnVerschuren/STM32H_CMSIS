//
// Created by marijn on 7/15/23.
//

#include "i2c.h"


/*!<
 * variables
 * */
uint32_t I2C123_kernel_frequency =			0;
uint32_t I2C4_kernel_frequency =			0;
uint32_t I2C_peripheral_frequencies[4] = 	{0, 0, 0, 0};


/*!<
 * misc
 */
uint8_t I2C_num(I2C_TypeDef* i2c) { dev_id_t id = dev_to_id(i2c); return id.clk ? id.num - 4 : id.num - 21; }

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
void fconfig_I2C(I2C_GPIO_t scl, I2C_GPIO_t sda, I2C_frequency_setting_t freq, uint16_t own_address, I2C_address_t address_type, uint8_t dual_address, uint8_t dual_mask) {
	if (scl == I2C_PIN_DISABLE || sda == I2C_PIN_DISABLE) { return; }
	dev_pin_t		scl_dev = *((dev_pin_t*)&scl),				sda_dev = *((dev_pin_t*)&sda);
	I2C_TypeDef		*scl_i2c = id_to_dev(scl_dev.dev_id),		*sda_i2c = id_to_dev(sda_dev.dev_id),		*i2c = NULL;
	GPIO_TypeDef	*scl_port = int_to_GPIO(scl_dev.port_num),	*sda_port = int_to_GPIO(sda_dev.port_num);
	if (scl_i2c != sda_i2c) { return; } i2c = scl_i2c;
	fconfig_GPIO(scl_port, scl_dev.pin_num, GPIO_alt_func, GPIO_no_pull, GPIO_open_drain, GPIO_high_speed, scl_dev.alt_func);
	fconfig_GPIO(sda_port, sda_dev.pin_num, GPIO_alt_func, GPIO_no_pull, GPIO_open_drain, GPIO_high_speed, sda_dev.alt_func);
	uint8_t i2c_num = I2C_num(i2c);

	uint32_t ker_clk_freq = 0;
	uint8_t prescaler = 0;
	I2C_peripheral_frequencies[i2c_num] = 0;

	switch (scl_dev.dev_id.clk) {
		case DEV_CLOCK_ID_APB1:			ker_clk_freq = I2C123_kernel_frequency;	break;
		case DEV_CLOCK_ID_APB4:			ker_clk_freq = I2C4_kernel_frequency;	break;
	}

	prescaler = ker_clk_freq / freq.target;
	I2C_peripheral_frequencies[i2c_num] = ker_clk_freq / prescaler;
	if (ker_clk_freq % freq.target) {  // attempt to correct prescaler error
		freq.scl_l_pre = (double)freq.scl_l_pre * ((double)I2C_peripheral_frequencies[i2c_num] / (double)freq.target);
		freq.scl_h_pre = (double)freq.scl_h_pre * ((double)I2C_peripheral_frequencies[i2c_num] / (double)freq.target);
	}

	i2c->CR1 = 0;  // make sure I2C is off
	i2c->TIMINGR = (
			(prescaler << I2C_TIMINGR_PRESC_Pos)		|
			(freq.scl_delay << I2C_TIMINGR_SCLDEL_Pos)	|
			(freq.sda_delay << I2C_TIMINGR_SDADEL_Pos)	|
			(freq.scl_h_pre << I2C_TIMINGR_SCLH_Pos)	|
			(freq.scl_l_pre << I2C_TIMINGR_SCLL_Pos)
	);
	i2c->CR2 = (
			I2C_CR2_AUTOEND	|  // TODO: setting?
			(address_type << I2C_CR2_ADD10_Pos)
	);
	i2c->OAR1 = (
			((own_address != 0) << I2C_OAR1_OA1EN_Pos)	|
			(address_type << I2C_OAR1_OA1MODE_Pos)		|
			((own_address << (I2C_OAR1_OA1_Pos + (!address_type))) & I2C_OAR1_OA1)
	);
	i2c->OAR2 = (
			((dual_address != 0) << I2C_OAR2_OA2EN_Pos)			|
			((dual_mask & 0b111UL) << I2C_OAR2_OA2MASK01_Pos)	|
			((dual_address << I2C_OAR2_OA2_Pos) & I2C_OAR2_OA2)
	);
	i2c->CR1 |= I2C_CR1_PE;  // turn I2C on
}

void config_I2C(I2C_GPIO_t scl, I2C_GPIO_t sda, I2C_frequency_t freq, uint8_t own_address) {
	uint8_t i2c_num = I2C_num(
		pin_to_dev(*((dev_pin_t*)&scl))
	);
	I2C_frequency_setting_t setting;

	SYSCFG->PMCR &= ~(0b1UL << i2c_num);
	switch (freq) {
		case I2C_SM_FREQUENCY_10KHZ:
			setting.target =	4000000;
			setting.scl_l_pre =	0xC7U;	// 50 us	@ 4Mhz
			setting.scl_h_pre =	0xC3U;	// 49 us	@ 4Mhz
			setting.sda_delay =	2;
			setting.scl_delay =	4;
			break;
		case I2C_SM_FREQUENCY_100KHZ:
			setting.target =	4000000;
			setting.scl_l_pre =	0x13U;	// 5 us		@ 4Mhz
			setting.scl_h_pre =	0x0FU;	// 4 us		@ 4Mhz
			setting.sda_delay =	2;
			setting.scl_delay =	4;
			break;
		case I2C_FM_FREQUENCY_400KHZ:
			setting.target =	8000000;
			setting.scl_l_pre =	0x09U;	// 1250 ns	@ 8Mhz
			setting.scl_h_pre =	0x03U;	// 500 ns	@ 8Mhz
			setting.sda_delay =	2;
			setting.scl_delay =	3;
			// enable FM?
			break;
		case I2C_FMP_FREQUENCY_1MHZ:
			setting.target =	16000000;
			setting.scl_l_pre =	0x07U;	// 500 ns	@ 8Mhz
			setting.scl_h_pre =	0x03U;	// 250 ns	@ 8Mhz
			setting.sda_delay =	0;
			setting.scl_delay =	2;
			SYSCFG->PMCR |= (0b1UL << i2c_num);  // enable FM+
			break;
	}
	fconfig_I2C(scl, sda, setting, own_address, I2C_ADDR_7BIT, 0, 0);
}


/*!<
 * master io
 * */
uint8_t I2C_master_address(I2C_TypeDef* i2c, uint8_t i2c_address, uint32_t timeout) {  // -> 0 = OK
	return 0;  // p.g. 2006
}

uint32_t I2C_master_write(I2C_TypeDef* i2c, uint8_t i2c_address, const uint8_t* buffer, uint32_t size, uint32_t timeout) {  // -> n processed
	return 0;
}

uint32_t I2C_master_read(I2C_TypeDef* i2c, uint8_t i2c_address, uint8_t* buffer, uint32_t size, uint32_t timeout) {  // -> n processed
	return 0;
}

uint32_t I2C_master_write_reg(I2C_TypeDef* i2c, uint8_t i2c_address, uint8_t reg_address, const uint8_t* buffer, uint32_t size, uint32_t timeout) {  // -> n processed
	return 0;
}

uint32_t I2C_master_read_reg(I2C_TypeDef* i2c, uint8_t i2c_address, uint8_t reg_address, uint8_t* buffer, uint32_t size, uint32_t timeout) {  // -> n processed
	return 0;
}
