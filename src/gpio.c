//
// Created by marijn on 6/26/23.
//

#include "gpio.h"


/*!<
 * init / enable / disable
 * */
void enable_GPIO(GPIO_TypeDef* port) {
	RCC->AHB4ENR |= 0b1UL << dev_to_int(port);
}
void disable_GPIO(GPIO_TypeDef* port) {
	RCC->AHB4RSTR |= 0b1UL << dev_to_int(port);
}

void reset_GPIO(GPIO_TypeDef* port, uint8_t pin) {
	port->MODER &= ~(0b11UL << (pin << 1));
	port->OSPEEDR &= ~(0b11UL << (pin << 1));
	port->PUPDR &= ~(0b11UL << (pin << 1));
	port->OTYPER &= ~(0b1UL << pin);
}
void fconfig_GPIO(GPIO_TypeDef* port, uint8_t pin, GPIO_MODE_t mode, GPIO_PULL_t pull, GPIO_OT_t output_type, GPIO_SPEED_t speed, uint8_t alternate_function) {
	enable_GPIO(port);
	reset_GPIO(port, pin);
	uint8_t pos = ((pin & 0xFUL) << 1);
	port->MODER |= mode << pos;
	port->OSPEEDR |= speed << pos;
	port->PUPDR |= pull << pos;
	port->OTYPER |= output_type << pin;
	pos = ((pin & 0x7UL) << 2);
	port->AFR[pin >> 3] &= ~(0xFUL << pos);							// clear AFR entry
	port->AFR[pin >> 3] |= ((alternate_function & 0xFUL) << pos);	// set AFR entry
}
void config_GPIO(GPIO_TypeDef* port, uint8_t pin, GPIO_MODE_t mode, GPIO_PULL_t pull, GPIO_OT_t output_type) {
	fconfig_GPIO(port, pin, mode, pull, output_type, GPIO_low_speed, 0);
}


/*!<
 * output
 * */
void GPIO_write(GPIO_TypeDef* port, uint8_t pin, uint8_t data) {
	port->BSRR |= ((data ? 0x1UL: 0x100UL) << pin);
}
void GPIO_toggle(GPIO_TypeDef* port, uint8_t pin) {
	port->ODR ^= (0b1UL << pin);
}


/*!<
 * input
 * */
uint8_t GPIO_read(GPIO_TypeDef* port, uint8_t pin) {
	return (port->IDR >> pin) & 0b1UL;
}