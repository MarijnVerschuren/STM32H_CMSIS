//
// Created by marijn on 2/13/23.
//
#ifndef STM32F_GPIO_H
#define STM32F_GPIO_H
#include "main.h"
#include "base.h"


typedef enum {
	GPIO_input =		0b00,
	GPIO_output =		0b01,
	GPIO_alt_func =		0b10,
	GPIO_analog =		0b11
} GPIO_MODE_t;
typedef enum {
	GPIO_low_speed =		0b00,
	GPIO_medium_speed =		0b01,
	GPIO_high_speed =		0b10,
	GPIO_very_high_speed =	0b11
} GPIO_SPEED_t;
typedef enum {
	GPIO_no_pull =		0b00,
	GPIO_pull_up =		0b01,
	GPIO_pull_down =	0b10,
	GPIO_reserved =		0x11
} GPIO_PULL_t;
typedef enum {
	GPIO_push_pull =	0b0,
	GPIO_open_drain =	0b1
} GPIO_OT_t;


/*!< misc */
uint8_t GPIO_to_int(GPIO_TypeDef* port);
GPIO_TypeDef* int_to_GPIO(uint8_t num);
/*!< init / enable / disable */
void enable_GPIO(GPIO_TypeDef* port);
void disable_GPIO(GPIO_TypeDef* port);
void reset_GPIO(GPIO_TypeDef* port, uint8_t pin);
void fconfig_GPIO(GPIO_TypeDef* port, uint8_t pin, GPIO_MODE_t mode, GPIO_PULL_t pull, GPIO_OT_t output_type, GPIO_SPEED_t speed, uint8_t alternate_function);
void config_GPIO(GPIO_TypeDef* port, uint8_t pin, GPIO_MODE_t mode, GPIO_PULL_t pull, GPIO_OT_t output_type);
/*!< output */
void GPIO_write(GPIO_TypeDef* port, uint8_t pin, uint8_t data);
void GPIO_toggle(GPIO_TypeDef* port, uint8_t pin);
/*!< input */
uint8_t GPIO_read(GPIO_TypeDef* port, uint8_t pin);

#endif //STM32F_GPIO_H
