//
// Created by marijn on 7/10/23.
//

#include "uart.h"


/*!<
 * init
 * */
void fconfig_UART(UART_GPIO_t tx, UART_GPIO_t rx, uint32_t baud, UART_oversampling_t oversampling) {
	
}

void config_UART(UART_GPIO_t tx, UART_GPIO_t rx, uint32_t baud) {

}


/*!<
 * irq
 * */
void start_UART_read_irq(USART_TypeDef* usart, io_buffer_t* buffer, uint8_t fifo) {

}

void stop_UART_read_irq(USART_TypeDef* usart) {

}

// void start_UART_transmit_irq(USART_TypeDef* usart, uint8_t* buffer, uint32_t size, uint8_t fifo, uint32_t hold_off);

void disable_UART_irq(USART_TypeDef* usart) {

}


/*!<
 * input
 * */
uint32_t UART_read(USART_TypeDef* usart, uint8_t* buffer, uint32_t size, uint32_t timeout) {
	return 0;
}


/*!<
 * output
 * */
uint32_t UART_write(USART_TypeDef* usart, const uint8_t* buffer, uint32_t size, uint32_t timeout) {
	return 0;
}

uint8_t UART_print(USART_TypeDef* usart, char* str, uint32_t timeout) {
	return 0;
}
