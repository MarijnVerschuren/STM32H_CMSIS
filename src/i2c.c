//
// Created by marijn on 7/15/23.
//

#include "i2c.h"


/*!<
 * hidden enums
 * */
typedef enum {
	I2C_WRITE = 0,
	I2C_READ =  1
} I2C_direction_t;


/*!<
 * helpers
 * */
uint8_t I2C_num(I2C_TypeDef* i2c) { dev_id_t id = dev_to_id(i2c); return id.clk ? id.num - 4 : id.num - 21; }

void reset_tx(I2C_TypeDef* i2c) {
	i2c->CR2 &= ~(
			I2C_CR2_SADD        |
			I2C_CR2_RD_WRN      |
			I2C_CR2_START       |
			I2C_CR2_STOP        |
			I2C_CR2_NBYTES      |
			I2C_CR2_RELOAD		|
			I2C_CR2_AUTOEND
	);
	i2c->ICR |= (
			I2C_ICR_STOPCF		|
			I2C_ICR_NACKCF
	);
}

void config_tx(I2C_TypeDef* i2c, uint16_t i2c_address, I2C_direction_t dir, uint8_t auto_end) {
	i2c->CR2 |= (
			(auto_end != 0) * I2C_CR2_AUTOEND	|
			(dir << I2C_CR2_RD_WRN_Pos)     	|
			((i2c_address << ((i2c->CR2 & I2C_CR2_ADD10) == 0)) & I2C_CR2_SADD)
	);
}

uint8_t config_tx_size(I2C_TypeDef* i2c, uint32_t* size) {
	uint8_t reload = (*size) > 255;     uint8_t n_bytes = (*size);
	if (reload) { n_bytes = 255; }      (*size) -= n_bytes;
	i2c->CR2 |= (
			(reload << I2C_CR2_RELOAD_Pos)  |
			(n_bytes << I2C_CR2_NBYTES_Pos)
	); return n_bytes;
}


/*!<
 * variables
 * */
uint32_t I2C123_kernel_frequency =			0;
uint32_t I2C4_kernel_frequency =			0;
uint32_t I2C_peripheral_frequencies[4] = 	{0, 0, 0, 0};


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

void fconfig_I2C(I2C_GPIO_t scl, I2C_GPIO_t sda, I2C_setting_t setting, uint16_t own_address, I2C_address_t address_type, uint8_t dual_address, uint8_t dual_mask) {
	if (scl == I2C_PIN_DISABLE || sda == I2C_PIN_DISABLE) { return; }
	dev_pin_t		scl_dev = *((dev_pin_t*)&scl),				sda_dev = *((dev_pin_t*)&sda);
	I2C_TypeDef		*scl_i2c = id_to_dev(scl_dev.dev_id),		*sda_i2c = id_to_dev(sda_dev.dev_id),		*i2c = NULL;
	GPIO_TypeDef	*scl_port = int_to_GPIO(scl_dev.port_num),	*sda_port = int_to_GPIO(sda_dev.port_num);
	if (scl_i2c != sda_i2c) { return; } i2c = scl_i2c; enable_dev(i2c);
	fconfig_GPIO(scl_port, scl_dev.pin_num, GPIO_alt_func, GPIO_pull_up, GPIO_open_drain, GPIO_high_speed, scl_dev.alt_func);
	fconfig_GPIO(sda_port, sda_dev.pin_num, GPIO_alt_func, GPIO_pull_up, GPIO_open_drain, GPIO_high_speed, sda_dev.alt_func);
	uint8_t i2c_num = I2C_num(i2c);

	uint32_t ker_clk_freq = 0;
	switch (scl_dev.dev_id.clk) {
		case DEV_CLOCK_ID_APB1:			ker_clk_freq = I2C123_kernel_frequency;	break;
		case DEV_CLOCK_ID_APB4:			ker_clk_freq = I2C4_kernel_frequency;	break;
	} I2C_peripheral_frequencies[i2c_num] = ker_clk_freq / setting.prescaler;

	i2c->CR1 = 0;  // make sure I2C is off
	i2c->TIMINGR = (
		(setting.prescaler << I2C_TIMINGR_PRESC_Pos)	|
		(setting.scl_delay << I2C_TIMINGR_SCLDEL_Pos)	|
		(setting.sda_delay << I2C_TIMINGR_SDADEL_Pos)	|
		(setting.scl_h_pre << I2C_TIMINGR_SCLH_Pos)		|
		(setting.scl_l_pre << I2C_TIMINGR_SCLL_Pos)
	);
	i2c->CR2 = (
		I2C_CR2_AUTOEND |
		I2C_CR2_NACK    |  // should only be disabled in slave mode
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

void config_I2C(I2C_GPIO_t scl, I2C_GPIO_t sda, I2C_setting_t setting, uint8_t own_address) {
	fconfig_I2C(scl, sda, setting, own_address, I2C_ADDR_7BIT, 0, 0);
}


/*!<
 * master io
 * */
uint32_t I2C_master_write(I2C_TypeDef* i2c, uint16_t i2c_address, const uint8_t* buffer, uint32_t size, uint32_t timeout) {  // -> n unprocessed
	uint64_t start = tick;
	reset_tx(i2c);

	config_tx(i2c, i2c_address, I2C_WRITE, 1);
	uint8_t n_bytes = config_tx_size(i2c, &size);
	while (i2c->ISR & I2C_ISR_BUSY) { if (tick - start > timeout) { return size + n_bytes; } }
	i2c->CR2 |= I2C_CR2_START;

	while (n_bytes--) {
	 	while (!(i2c->ISR & I2C_ISR_TXIS)) { if (tick - start > timeout) { i2c->CR2 |= I2C_CR2_STOP; return size + n_bytes; } }
		i2c->TXDR = *buffer++;
		if (!n_bytes && size) {
			while (!(i2c->ISR & I2C_ISR_TCR)) { if (tick - start > timeout) { i2c->CR2 |= I2C_CR2_STOP; return size + n_bytes; } }
			n_bytes = config_tx_size(i2c, &size);
		}
	}

	while (!(i2c->ISR & I2C_ISR_STOPF)) { if (tick - start > timeout) { i2c->CR2 |= I2C_CR2_STOP; return 0; } }
	i2c->ICR |= I2C_ICR_STOPCF;  // clear stop flag
	return 0;
}

uint32_t I2C_master_read(I2C_TypeDef* i2c, uint16_t i2c_address, uint8_t* buffer, uint32_t size, uint32_t timeout) {  // -> n unprocessed
	uint64_t start = tick;
	reset_tx(i2c);

	config_tx(i2c, i2c_address, I2C_READ, 1);
	uint8_t n_bytes = config_tx_size(i2c, &size);
	while (i2c->ISR & I2C_ISR_BUSY) { if (tick - start > timeout) { return size + n_bytes; } }
	i2c->CR2 |= I2C_CR2_START;

	while (n_bytes--) {
		while (!(i2c->ISR & I2C_ISR_RXNE)) { if (tick - start > timeout) { i2c->CR2 |= I2C_CR2_STOP; return size + n_bytes; } }
		*buffer++ = i2c->RXDR;
		if (!n_bytes && size) {
			while (!(i2c->ISR & I2C_ISR_TCR)) { if (tick - start > timeout) { i2c->CR2 |= I2C_CR2_STOP; return size + n_bytes; } }
			n_bytes = config_tx_size(i2c, &size);
		}
	}

	while (!(i2c->ISR & I2C_ISR_STOPF)) { if (tick - start > timeout) { i2c->CR2 |= I2C_CR2_STOP; return 0; } }
	i2c->ICR |= I2C_ICR_STOPCF;  // clear stop flag
	return 0;
}

uint32_t I2C_master_write_reg(
		I2C_TypeDef* i2c, uint16_t i2c_address, uint64_t reg_address,
		I2C_register_address_t reg_address_type, const uint8_t* buffer,
		uint32_t size, uint32_t timeout
) {  // -> n unprocessed
	uint64_t start = tick;
	reset_tx(i2c);

	config_tx(i2c, i2c_address, I2C_WRITE, 1);
	uint8_t reg_address_size = (0b1UL << reg_address_type);	size += reg_address_size;
	uint8_t n_bytes = config_tx_size(i2c, &size);			n_bytes -= reg_address_size;
	while (i2c->ISR & I2C_ISR_BUSY) { if (tick - start > timeout) { return size + n_bytes; } }
	i2c->CR2 |= I2C_CR2_START;

	while (reg_address_size--) {
		while (!(i2c->ISR & I2C_ISR_TXIS)) { if (tick - start > timeout) { i2c->CR2 |= I2C_CR2_STOP; return size + n_bytes; } }
		i2c->TXDR = (reg_address >> (8 * reg_address_size)) & 0xFFU;
	}

	while (n_bytes--) {
		while (!(i2c->ISR & I2C_ISR_TXIS)) { if (tick - start > timeout) { i2c->CR2 |= I2C_CR2_STOP; return size + n_bytes; } }
		i2c->TXDR = *buffer++;
		if (!n_bytes && size) {
			while (!(i2c->ISR & I2C_ISR_TCR)) { if (tick - start > timeout) { i2c->CR2 |= I2C_CR2_STOP; return size + n_bytes; } }
			n_bytes = config_tx_size(i2c, &size);
		}
	}

	while (!(i2c->ISR & I2C_ISR_STOPF)) { if (tick - start > timeout) { i2c->CR2 |= I2C_CR2_STOP; return 0; } }
	i2c->ICR |= I2C_ICR_STOPCF;  // clear stop flag
	return 0;
}

uint32_t I2C_master_read_reg(
		I2C_TypeDef* i2c, uint16_t i2c_address, uint64_t reg_address,
		I2C_register_address_t reg_address_type, uint8_t* buffer,
		uint32_t size, uint32_t timeout
) {  // -> n unprocessed
	uint64_t start = tick;
	reset_tx(i2c);

	config_tx(i2c, i2c_address, I2C_WRITE, 1);
	uint8_t reg_address_size = (0b1UL << reg_address_type);
	i2c->CR2 |= (reg_address_size << I2C_CR2_NBYTES_Pos);
	while (i2c->ISR & I2C_ISR_BUSY) { if (tick - start > timeout) { return size; } }
	i2c->CR2 |= I2C_CR2_START;

	while (reg_address_size--) {
		while (!(i2c->ISR & I2C_ISR_TXIS)) { if (tick - start > timeout) { i2c->CR2 |= I2C_CR2_STOP; return size; } }
		i2c->TXDR = (reg_address >> (8 * reg_address_size)) & 0xFFU;
	}

	while (!(i2c->ISR & I2C_ISR_STOPF)) { if (tick - start > timeout) { i2c->CR2 |= I2C_CR2_STOP; return 0; } }
	i2c->ICR |= I2C_ICR_STOPCF;  // clear stop flag

	return I2C_master_read(i2c, i2c_address, buffer, size, timeout - (tick - start));
}