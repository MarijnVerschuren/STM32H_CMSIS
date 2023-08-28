//
// Created by marijn on 7/10/23.
//

#include "usart.h"


/*!<
 * types
 */
typedef struct {
	io_buffer_t*	rx_buf;
	io_buffer_t*	tx_buf;
	uint8_t			rx_fifo : 1;
	uint8_t			tx_fifo : 1;
} USART_IRQ_IO_t;


/*!<
 * variables
 * */
uint32_t USART16_kernel_frequency =		0;
uint32_t USART234578_kernel_frequency =	0;
uint32_t LPUART1_kernel_frequency =		0;


/*!<
 * static
 * */
static inline uint16_t UART_division(dev_clock_id_t clk, uint32_t baud) {
	switch (clk) {
		case DEV_CLOCK_ID_APB1:	return USART16_kernel_frequency / baud;
		case DEV_CLOCK_ID_APB2: return USART234578_kernel_frequency / baud;
		case DEV_CLOCK_ID_APB4: return LPUART1_kernel_frequency / baud;
	} return 0;
}

static inline void UART_irq_handler(USART_TypeDef* usart, USART_IRQ_IO_t* io) {
	// TODO
}


/*!<
 * IRQ
 */
USART_IRQ_IO_t uart_buf_1;		extern void USART1_IRQHandler(void)		{ UART_irq_handler(USART1,	&uart_buf_1); }
USART_IRQ_IO_t uart_buf_2;		extern void USART2_IRQHandler(void)		{ UART_irq_handler(USART2,	&uart_buf_2); }
USART_IRQ_IO_t uart_buf_3;		extern void USART3_IRQHandler(void)		{ UART_irq_handler(USART3,	&uart_buf_3); }
USART_IRQ_IO_t uart_buf_4;		extern void UART4_IRQHandler(void)		{ UART_irq_handler(UART4,	    &uart_buf_4); }
USART_IRQ_IO_t uart_buf_5;		extern void UART5_IRQHandler(void)		{ UART_irq_handler(UART5,	    &uart_buf_5); }
USART_IRQ_IO_t uart_buf_6;		extern void USART6_IRQHandler(void)		{ UART_irq_handler(USART6,	&uart_buf_6); }
USART_IRQ_IO_t uart_buf_7;		extern void UART7_IRQHandler(void)		{ UART_irq_handler(UART7,	    &uart_buf_7); }
USART_IRQ_IO_t uart_buf_8;		extern void UART8_IRQHandler(void)		{ UART_irq_handler(UART8,	    &uart_buf_8); }
USART_IRQ_IO_t uart_buf_lp1;	extern void LPUART1_IRQHandler(void)	{ UART_irq_handler(LPUART1,	&uart_buf_lp1); }


/*!<
 * init
 * */
void config_USART_kernel_clocks(USART_CLK_SRC_t usart16_src, USART_CLK_SRC_t usart234578_src, USART_CLK_SRC_t lpuart1_src) {
	RCC->D2CCIP2R &= ~(
			RCC_D2CCIP2R_USART16SEL |
			RCC_D2CCIP2R_USART28SEL
	);
	RCC->D2CCIP2R |= (
			(usart16_src << RCC_D2CCIP2R_USART16SEL_Pos) |
			(usart234578_src << RCC_D2CCIP2R_USART28SEL_Pos)
	);
	RCC->D3CCIPR &= ~RCC_D3CCIPR_LPUART1SEL;
	RCC->D3CCIPR |=	lpuart1_src << RCC_D3CCIPR_LPUART1SEL_Pos;
	switch (usart16_src) {
		case USART_CLK_SRC_APBx:	USART16_kernel_frequency = APB2_clock_frequency; break;		// APB2
		case USART_CLK_SRC_PLL2_Q:	USART16_kernel_frequency = PLL2_Q_clock_frequency; break;
		case USART_CLK_SRC_PLL3_Q:	USART16_kernel_frequency = PLL3_Q_clock_frequency; break;
		case USART_CLK_SRC_HSI:		USART16_kernel_frequency = HSI_clock_frequency; break;
		case USART_CLK_SRC_CSI:		USART16_kernel_frequency = CSI_clock_frequency; break;
		case USART_CLK_SRC_LSE:		USART16_kernel_frequency = LSE_clock_frequency; break;
	}	switch (usart234578_src) {
		case USART_CLK_SRC_APBx:	USART234578_kernel_frequency = APB1_clock_frequency; break;	// APB1
		case USART_CLK_SRC_PLL2_Q:	USART234578_kernel_frequency = PLL2_Q_clock_frequency; break;
		case USART_CLK_SRC_PLL3_Q:	USART234578_kernel_frequency = PLL3_Q_clock_frequency; break;
		case USART_CLK_SRC_HSI:		USART234578_kernel_frequency = HSI_clock_frequency; break;
		case USART_CLK_SRC_CSI:		USART234578_kernel_frequency = CSI_clock_frequency; break;
		case USART_CLK_SRC_LSE:		USART234578_kernel_frequency = LSE_clock_frequency; break;
	}	switch (lpuart1_src) {
		case USART_CLK_SRC_APBx:	LPUART1_kernel_frequency = APB4_clock_frequency; return;	// APB4
		case USART_CLK_SRC_PLL2_Q:	LPUART1_kernel_frequency = PLL2_Q_clock_frequency; return;
		case USART_CLK_SRC_PLL3_Q:	LPUART1_kernel_frequency = PLL3_Q_clock_frequency; return;
		case USART_CLK_SRC_HSI:		LPUART1_kernel_frequency = HSI_clock_frequency; return;
		case USART_CLK_SRC_CSI:		LPUART1_kernel_frequency = CSI_clock_frequency; return;
		case USART_CLK_SRC_LSE:		LPUART1_kernel_frequency = LSE_clock_frequency; return;
	}
}

void fconfig_UART(
		USART_GPIO_t tx, USART_GPIO_t rx, uint32_t baud, uint8_t fifo,
		USART_stop_bit_t stop, USART_parity_t parity, uint8_t msb_first,
		USART_oversampling_t oversampling, USART_word_length_t word_length
) {
	uint8_t			tx_enable = tx != UART_PIN_DISABLE,			rx_enable = rx != UART_PIN_DISABLE;
	dev_pin_t		tx_dev = *((dev_pin_t*)&tx),				rx_dev = *((dev_pin_t*)&rx);
	USART_TypeDef	*tx_uart = id_to_dev(tx_dev.dev_id),		*rx_uart = id_to_dev(rx_dev.dev_id),		*uart = NULL;
	GPIO_TypeDef	*tx_port = int_to_GPIO(tx_dev.port_num),	*rx_port = int_to_GPIO(tx_dev.port_num);
	if (tx_enable) { uart = tx_uart; fconfig_GPIO(tx_port, tx_dev.pin_num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, tx_dev.alt_func); }
	if (rx_enable) { uart = rx_uart; fconfig_GPIO(rx_port, rx_dev.pin_num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, tx_dev.alt_func); }
	if (tx_enable && rx_enable && (tx_uart != rx_uart)) { return; }  // error if tx and rx are on different usart devices
	enable_dev(uart);
	uint16_t uart_div = UART_division(tx_dev.dev_id.clk, baud) >> (1 - oversampling);
	uart->BRR = ((uart_div & 0xfff0) | ((uart_div & 0xf) >> oversampling));
	do { uart->CR1 &= ~USART_CR1_UE; } while (uart->CR1 & USART_CR1_UE);
	uart->CR1 = (
			(fifo << USART_CR1_FIFOEN_Pos)						|
			(((word_length >> 1) & 0b1UL) << USART_CR1_M1_Pos)	|
			((word_length & 0b1UL) << USART_CR1_M0_Pos)			|
			(oversampling << USART_CR1_OVER8_Pos)				|
			(((parity >> 1) & 0b1UL) << USART_CR1_PCE_Pos)		|
			((parity & 0b1UL) << USART_CR1_PS_Pos)
	);
	uart->CR2 = (
			(msb_first << USART_CR2_MSBFIRST_Pos)				|
			(stop << USART_CR2_STOP_Pos)
	);
	do { uart->CR1 |= (
			(tx_enable << USART_CR1_TE_Pos)						|
			(rx_enable << USART_CR1_RE_Pos)						|
			USART_CR1_UE
	); } while (
			(tx_enable && !(uart->ISR & USART_ISR_TEACK))		||
			(rx_enable && !(uart->ISR & USART_ISR_REACK))
	);
}

void config_UART(USART_GPIO_t tx, USART_GPIO_t rx, uint32_t baud, uint8_t fifo) {
	fconfig_UART(
			tx, rx, baud, fifo, USART_STOP_BIT_1, USART_PARITY_DISABLED,
			0, USART_OVERSAMPLING_16, USART_WORD_LENGTH_8
	);
}


/*!<
 * irq
 * */
void start_UART_read_irq(USART_TypeDef* usart, io_buffer_t* buffer, uint8_t fifo) {

}

void stop_UART_read_irq(USART_TypeDef* usart) {

}

void start_UART_transmit_irq(USART_TypeDef* usart, uint8_t* buffer, uint32_t size, uint8_t fifo, uint32_t hold_off) {

}

void disable_UART_irq(USART_TypeDef* usart) {

}


/*!<
 * io
 * */
uint32_t UART_read(USART_TypeDef* usart, uint8_t* buffer, uint32_t size, uint32_t timeout) {  // -> n unprocessed
	uint64_t start = tick;
	while (size--) {
		while (!(usart->ISR & USART_ISR_RXNE_RXFNE)) { if ( tick - start > timeout) { return size; } }
		*buffer++ = usart->RDR;
	} return size;
}

uint32_t UART_write(USART_TypeDef* usart, const uint8_t* buffer, uint32_t size, uint32_t timeout) {  // -> n unprocessed
	uint64_t start = tick;
	while (size--) {
		while (!(usart->ISR & USART_ISR_TXE_TXFNF)) { if ( tick - start > timeout) { return size; } }
		usart->TDR = *buffer++;
	} return size;
}

uint8_t UART_print(USART_TypeDef* usart, const char* str, uint32_t timeout) {  // -> 0 = OK
	uint64_t start = tick;
	while (*str) {
		while (!(usart->ISR & USART_ISR_TXE_TXFNF)) { if (tick - start > timeout) { return -1; } }
		usart->TDR = *str++;
	} return 0;
}
