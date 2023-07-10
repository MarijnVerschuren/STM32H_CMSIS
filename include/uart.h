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
	// UART1
	UART1_CK_A8 =		0x08070044,		UART1_TX_A9 =		0x09070044,
	UART1_RX_A10 =		0x0A070044,		UART1_CTS_A11 =		0x0B070044,
	UART1_NSS_A11 =		0x0B070044,		UART1_RTS_A12 =		0x0C070044,
	UART1_DE_A12 =		0x0C070044,		UART1_TX_B14 =		0x0E140044,
	UART1_RX_B15 =		0x0F140044,		UART1_TX_B6 =		0x06170044,
	UART1_RX_B7 =		0x07170044,
	// UART2
	UART2_CTS_A0 =		0x00070011,		UART2_NSS_A0 =		0x00070011,
	UART2_RTS_A1 =		0x01070011,		UART2_DE_A1 =		0x01070011,
	UART2_TX_A2 =		0x02070011,		UART2_RX_A3 =		0x03070011,
	UART2_CK_A4 =		0x04070011,		UART2_CTS_D3 =		0x03370011,
	UART2_NSS_D3 =		0x03370011,		UART2_RTS_D4 =		0x04370011,
	UART2_DE_D4 =		0x04370011,		UART2_TX_D5 =		0x05370011,
	UART2_RX_D6 =		0x06370011,		UART2_CK_D7 =		0x07370011,
	// UART3
	UART3_TX_B10 =		0x0A170012,		UART3_RX_B11 =		0x0B170012,
	UART3_CK_B12 =		0x0C170012,		UART3_CTS_B13 =		0x0D170012,
	UART3_NSS_B13 =		0x0D170012,		UART3_RTS_B14 =		0x0E170012,
	UART3_DE_B14 =		0x0E170012,		UART3_TX_C10 =		0x0A270012,
	UART3_RX_C11 =		0x0B270012,		UART3_CK_C12 =		0x0C270012,
	UART3_TX_D8 =		0x08370012,		UART3_RX_D9 =		0x09370012,
	UART3_CK_D10 =		0x0A370012,		UART3_CTS_D11 =		0x0B370012,
	UART3_NSS_D11 =		0x0B370012,		UART3_RTS_D12 =		0x0C370012,
	UART3_DE_D12 =		0x0C370012,
	// UART4
	UART4_RX_A11 =		0x0B060013,		UART4_TX_A12 =		0x0C060013,
	UART4_TX_A0 =		0x00080013,		UART4_RX_A1 =		0x01080013,
	UART4_CTS_A15 =		0x0F080013,		UART4_DE_A15 =		0x0F080013,
	UART4_CTS_B0 =		0x00180013,		UART4_RX_B8 =		0x08180013,
	UART4_TX_B9 =		0x09180013,		UART4_RTS_B14 =		0x0E180013,
	UART4_DE_B14 =		0x0E180013,		UART4_CTS_B15 =		0x0F180013,
	UART4_TX_C10 =		0x0A280013,		UART4_RX_C11 =		0x0B280013,
	UART4_RX_D0 =		0x00380013,		UART4_TX_D1 =		0x01380013,
	UART4_TX_H13 =		0x0D780013,		UART4_RX_H14 =		0x0E780013,
	UART4_RX_I9 =		0x09880013,
	// UART5
	UART5_RX_B5 =		0x051E0014,		UART5_TX_B6 =		0x061E0014,
	UART5_RX_B12 =		0x0C1E0014,		UART5_TX_B13 =		0x0D1E0014,
	UART5_RTS_C8 =		0x08280014,		UART5_DE_C8 =		0x08280014,
	UART5_CTS_C9 =		0x09280014,		UART5_TX_C12 =		0x0C280014,
	UART5_RX_D2 =		0x02380014,
	// UART6
	UART6_TX_C6 =		0x06270045,		UART6_RX_C7 =		0x07270045,
	UART6_CK_C8 =		0x08270045,		UART6_CK_G7 =		0x07670045,
	UART6_RTS_G8 =		0x08670045,		UART6_DE_G8 =		0x08670045,
	UART6_RX_G9 =		0x09670045,		UART6_RTS_G12 =		0x0C670045,
	UART6_DE_G12 =		0x0C670045,		UART6_CTS_G13 =		0x0D670045,
	UART6_NSS_G13 =		0x0D670045,		UART6_TX_G14 =		0x0E670045,
	UART6_CTS_G15 =		0x0F670045,		UART6_NSS_G15 =		0x0F670045,
	// UART7
	UART7_RX_A8 =		0x080B001E,		UART7_TX_A15 =		0x0F0B001E,
	UART7_RX_B3 =		0x031B001E,		UART7_TX_B4 =		0x041B001E,
	UART7_RX_E7 =		0x0747001E,		UART7_TX_E8 =		0x0847001E,
	UART7_RTS_E9 =		0x0947001E,		UART7_DE_E9 =		0x0947001E,
	UART7_CTS_E10 =		0x0A47001E,		UART7_RX_F6 =		0x0657001E,
	UART7_TX_F7 =		0x0757001E,		UART7_RTS_F8 =		0x0857001E,
	UART7_DE_F8 =		0x0857001E,		UART7_CTS_F9 =		0x0957001E,
	// UART8
	UART8_CTS_D14 =		0x0E38001F,		UART8_RTS_D15 =		0x0F38001F,
	UART8_DE_D15 =		0x0F38001F,		UART8_RX_E0 =		0x0048001F,
	UART8_TX_E1 =		0x0148001F,		UART8_TX_J8 =		0x0898001F,
	UART8_RX_J9 =		0x0998001F,

	// LPUART1
	LPUART1_TX_A9 =		0x090300C3,		LPUART1_RX_A10 =	0x0A0300C3,
	LPUART1_CTS_A11 =	0x0B0300C3,		LPUART1_RTS_A12 =	0x0C0300C3,
	LPUART1_DE_A12 =	0x0C0300C3,		LPUART1_TX_B6 =		0x061800C3,
	LPUART1_RX_B7 =		0x071800C3
} UART_GPIO_t;

typedef enum {
	USART_OVERSAMPLING_16 =	0,
	USART_OVERSAMPLING_8 =	1,
} UART_oversampling_t;


/*!<
 * init
 * */
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
