//
// Created by marijn on 7/10/23.
//

#ifndef STM32H_CMSIS_UART_H
#define STM32H_CMSIS_UART_H
#include "main.h"
#include "gpio.h"


/*!<
 * types
 * */
typedef enum {
	UART1_A0 = 0x00010044
} UART_GPIO_t;

typedef enum {
	USART_OVERSAMPLING_16 =	0,
	USART_OVERSAMPLING_8 =	1,
} UART_oversampling_t;


/*!<
 * init
 * */
void disable_UART(USART_TypeDef* usart);
void fconfig_UART(UART_GPIO_t tx, UART_GPIO_t rx, uint32_t baud, UART_oversampling_t oversampling);
void config_UART(UART_GPIO_t tx, UART_GPIO_t rx, uint32_t baud);
/*!<
 * irq
 * */
void start_UART_read_irq(USART_TypeDef* usart, io_buffer_t* buffer, uint8_t fifo);
void stop_UART_read_irq(USART_TypeDef* usart);
// void start_UART_transmit_irq(USART_TypeDef* usart, uint8_t* buffer, uint32_t size, uint8_t fifo, uint32_t hold_off);
void disable_UART_irq(USART_TypeDef* usart);
/*!<
 * input
 * */
uint32_t UART_read(USART_TypeDef* usart, uint8_t* buffer, uint32_t size, uint32_t timeout);
/*!<
 * output
 * */
uint32_t UART_write(USART_TypeDef* usart, const uint8_t* buffer, uint32_t size, uint32_t timeout);
uint8_t UART_print(USART_TypeDef* usart, char* str, uint32_t timeout);


#endif //STM32H_CMSIS_UART_H
