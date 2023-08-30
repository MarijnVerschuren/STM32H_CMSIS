//
// Created by marijn on 7/15/23.
//

#ifndef STM32H_CMSIS_I2C_H
#define STM32H_CMSIS_I2C_H
#include "main.h"
#include "gpio.h"
#include "sys.h"


/*!<
 * types
 * */
typedef enum {
	I2C_CLK_SRC_APBx =		0b00,	//R
	I2C_CLK_SRC_PLL3_R =	0b01,
	I2C_CLK_SRC_HSI =		0b10,
	I2C_CLK_SRC_CSI =		0b11
} I2C_CLK_SRC_t;

typedef enum {
	I2C_PIN_DISABLE =	0x00000000,
	// I2C1
	I2C1_SMBA_B5 =		0x05140015,		I2C1_SCL_B6 =		0x06140015,
	I2C1_SDA_B7 =		0x07140015,		I2C1_SCL_B8 =		0x08140015,
	I2C1_SDA_B9 =		0x09140015,
	// I2C2
	I2C2_SCL_B10 =		0x0A140016,		I2C2_SDA_B11 =		0x0B140016,
	I2C2_SMBA_B12 =		0x0C140016,		I2C2_SDA_F0 =		0x00540016,
	I2C2_SCL_F1 =		0x01540016,		I2C2_SMBA_F2 =		0x02540016,
	I2C2_SCL_H4 =		0x04740016,		I2C2_SDA_H5 =		0x05740016,
	I2C2_SMBA_H6 =		0x06740016,
	// I2C3
	I2C3_SCL_A8 =		0x08040017,		I2C3_SMBA_A9 =		0x09040017,
	I2C3_SDA_C9 =		0x09240017,		I2C3_SCL_H7 =		0x07740017,
	I2C3_SDA_H8 =		0x08740017,		I2C3_SMBA_H9 =		0x09740017,
	// I2C4
	I2C4_SMBA_B5 =		0x051600C7,		I2C4_SCL_B6 =		0x061600C7,
	I2C4_SDA_B7 =		0x071600C7,		I2C4_SCL_B8 =		0x081600C7,
	I2C4_SDA_B9 =		0x091600C7,		I2C4_SMBA_B9 =		0x091B00C7,
	I2C4_SMBA_D11 =		0x0B3400C7,		I2C4_SCL_D12 =		0x0C3400C7,
	I2C4_SDA_D13 =		0x0D3400C7,		I2C4_SMBA_F13 =		0x0D5400C7,
	I2C4_SCL_F14 =		0x0E5400C7,		I2C4_SDA_F15 =		0x0F5400C7,
	I2C4_SMBA_H10 =		0x0A7400C7,		I2C4_SCL_H11 =		0x0B7400C7,
	I2C4_SDA_H12 =		0x0C7400C7
} I2C_GPIO_t;

typedef enum {
	I2C_ADDR_7BIT =		0b0u,
	I2C_ADDR_10BIT =	0b1u,
} I2C_address_t;

typedef enum {
	I2C_REG_8 =		0,
	I2C_REG_16 =	1,
	I2C_REG_32 =	2,
	I2C_REG_64 =	3
} I2C_register_address_t;

typedef struct {
	uint8_t prescaler;
	uint8_t scl_l_pre;
	uint8_t scl_h_pre;
	uint8_t sda_delay;
	uint8_t scl_delay;
} I2C_setting_t;


/*!<
 * variables
 * */
extern uint32_t I2C123_kernel_frequency;
extern uint32_t I2C4_kernel_frequency;
extern uint32_t I2C_peripheral_frequencies[4];

/*!<
 * init
 * */
void config_I2C_kernel_clocks(I2C_CLK_SRC_t i2c123_src, I2C_CLK_SRC_t i2c4_src);
void fconfig_I2C(I2C_GPIO_t scl, I2C_GPIO_t sda, I2C_setting_t setting, uint16_t own_address, I2C_address_t address_type, uint8_t dual_address,  uint8_t dual_mask);
void config_I2C(I2C_GPIO_t scl, I2C_GPIO_t sda, I2C_setting_t setting, uint8_t own_address);

/*!<
 * master io
 * */
uint32_t I2C_master_write(I2C_TypeDef* i2c, uint16_t i2c_address, const uint8_t* buffer, uint32_t size, uint32_t timeout);								// -> n unprocessed
uint32_t I2C_master_read(I2C_TypeDef* i2c, uint16_t i2c_address, uint8_t* buffer, uint32_t size, uint32_t timeout);										// -> n unprocessed
uint32_t I2C_master_write_reg(
		I2C_TypeDef* i2c, uint16_t i2c_address, uint64_t reg_address,
		I2C_register_address_t reg_address_type, const uint8_t* buffer,
		uint32_t size, uint32_t timeout
);		// -> n unprocessed
uint32_t I2C_master_read_reg(
		I2C_TypeDef* i2c, uint16_t i2c_address, uint64_t reg_address,
		I2C_register_address_t reg_address_type, uint8_t* buffer,
		uint32_t size, uint32_t timeout
);		// -> n unprocessed

/*!<
 * slave io
 * */ /* TODO: fix / implement these
uint32_t I2C_slave_write(I2C_TypeDef* i2c, const uint8_t* buffer, uint32_t size, uint32_t timeout);
uint32_t I2C_slave_read(I2C_TypeDef* i2c, uint8_t* buffer, uint32_t size, uint32_t timeout); */

/*!<
 * slave io irq
 * */ /*
uint32_t start_I2C_slave_read_irq(I2C_TypeDef* i2c, io_buffer_t* buffer, uint8_t fifo);
uint32_t stop_I2C_slave_read_IT(I2C_TypeDef* i2c);
*/


#endif //STM32H_CMSIS_I2C_H
