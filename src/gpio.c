//
// Created by marijn on 2/13/23.
//
#include "gpio.h"


/*!< misc */
uint8_t GPIO_to_int(GPIO_TypeDef* port) {
	return ((uint32_t)(port - AHB1PERIPH_BASE) >> 10u) & 0x7u;
}
GPIO_TypeDef* int_to_GPIO(uint8_t num) {
	return (GPIO_TypeDef*)(((num & 0x7u) << 10) + AHB1PERIPH_BASE);
}

/*!< init / disable */
#ifdef STM32F4xx
void enable_GPIO(GPIO_TypeDef* port) {
	RCC->AHB1ENR |= 0b1u << GPIO_to_int(port);
}
void disable_GPIO(GPIO_TypeDef* port) {
	RCC->AHB1ENR &= ~(0b1u << GPIO_to_int(port));
}

#elif defined(STM32F3xx)
void enable_GPIO(GPIO_TypeDef* port) {
    RCC->AHBENR |= (0b1u << (16 + (GPIO_to_int(port) + 1)));
}
void disable_GPIO(GPIO_TypeDef* port) {
    RCC->AHBENR &= ~(0b1u << (16 + (GPIO_to_int(port) + 1)));
}
#endif

void reset_GPIO(GPIO_TypeDef* port, uint8_t pin) {
	port->MODER &= ~(0b11u << (pin << 1u));
	port->OSPEEDR &= ~(0b11u << (pin << 1u));
	port->PUPDR &= ~(0b11u << (pin << 1u));
	port->OTYPER &= (0b1u << pin);
}
void fconfig_GPIO(GPIO_TypeDef* port, uint8_t pin, GPIO_MODE_t mode, GPIO_PULL_t pull, GPIO_OT_t output_type, GPIO_SPEED_t speed, uint8_t alternate_function) {
	enable_GPIO(port);
	reset_GPIO(port, pin);
	port->MODER |= (mode << (pin << 1u));
	port->OSPEEDR |= (speed << (pin << 1u));
	port->PUPDR |= (pull << (pin << 1u));
	port->OTYPER |= output_type << pin;
	uint8_t pos = ((pin & 0x7) << 2);
	port->AFR[pin >> 3] &= ~(0xf << pos);							// clear AFR entry
	port->AFR[pin >> 3] |= ((alternate_function & 0xf) << pos);		// set AFR entry
}
void config_GPIO(GPIO_TypeDef* port, uint8_t pin, GPIO_MODE_t mode, GPIO_PULL_t pull, GPIO_OT_t output_type) {
	fconfig_GPIO(port, pin, mode, pull, output_type, GPIO_low_speed, 0);
}


/*!< output */
void GPIO_write(GPIO_TypeDef* port, uint8_t pin, uint8_t data) {
	port->BSRR |= (1u << (pin + (16 * !data)));
}
void GPIO_toggle(GPIO_TypeDef* port, uint8_t pin) {
	port->ODR ^= (1u << pin);  // TODO: BSRR
}


/*!< input */
uint8_t GPIO_read(GPIO_TypeDef* port, uint8_t pin) {
	return (port->IDR >> pin) & 1u;
}