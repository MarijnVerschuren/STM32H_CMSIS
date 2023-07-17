//
// Created by marijn on 7/15/23.
//

#include "i2c.h"


/*!<
 * init
 * */
void fconfig_I2C(I2C_GPIO_t scl, I2C_GPIO_t sda, uint16_t own_address, I2C_address_t address_type, uint8_t dual_address) {

}

void config_I2C(I2C_GPIO_t scl, I2C_GPIO_t sda, uint8_t own_address) {
	fconfig_I2C(scl, sda, own_address, I2C_ADDR_7BIT, 0);
}
