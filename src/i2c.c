//
// Created by marijn on 7/15/23.
//

#include "i2c.h"


/*!<
 * static
 * */
static inline uint8_t I2C_freq(dev_clock_id_t clk, uint32_t freq) {
	uint32_t ker_clk_freq = 0;
	switch (clk) {  // TODO: create and use peripheral kernel clock frequency variables (clock can still be used to determine what kernel clock is used)
		case DEV_CLOCK_ID_APB1:	ker_clk_freq = APB1_clock_frequency;
		case DEV_CLOCK_ID_APB4: ker_clk_freq = APB4_clock_frequency;
	}
	// TODO: add kernel clock selection function in this (and .h) file
	// TODO: add freq and scl H/L arguments to config func
	// p.g. 1983

	return 0;
}


/*!<
 * init
 * */
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
