import sys, os


if __name__ == "__main__":
	tim_dev = "-tim" in sys.argv
	tim_dev |= (hrtim_dev := "-hrtim" in sys.argv)
	tim_dev |= (lptim_dev := "-lptim" in sys.argv)
	uart_dev = "-uart" in sys.argv
	uart_dev |= (lpuart_dev := "-lpuart" in sys.argv)
	
	clocks = {
		"APB1": 0, "AHB1": 1,
		"APB2": 2, "AHB2": 3,
		"APB3": 4, "AHB3": 5,
		"APB4": 6, "AHB4": 7,
	}
	ports = {
		"A": 0, "B": 1, "C": 2, "D": 3,
		"E": 4, "F": 5, "G": 6, "H": 7,
		"I": 8, "J": 9, "K": 10
	}
	tims = {
		"TIM1": (2, 0),     "TIM2": (0, 0),
		"TIM3": (0, 1),     "TIM4": (0, 2),
		"TIM5": (0, 3),     "TIM6": (0, 4),
		"TIM7": (0, 5),     "TIM8": (2, 1),
		"TIM12": (0, 6),    "TIM13": (0, 7),
		"TIM14": (0, 8),    "TIM15": (1, 16),
		"TIM16": (1, 17),   "TIM17": (1, 18),
		"HRTIM": (2, 29),   "LPTIM1": (0, 9),
		"LPTIM2": (6, 9),   "LPTIM3": (6, 10),
		"LPTIM4": (6, 11),  "LPTIM5": (6, 12)
	}
	uarts = {
		"UART1": (2, 4),   "UART2": (0, 17),
		"UART3": (0, 18),  "UART4": (0, 19),
		"UART5": (0, 20),  "UART6": (2, 5),
		"UART7": (0, 30),  "UART8": (0, 31),
		"LPUART1": (6, 3)
	}
	
	while True:
		try:
			sub = 0
			if tim_dev:
				tim = tims["HRTIM"]
				if not hrtim_dev:
					tim = input("tim: ")
					try:    tim = tims[f"LPTIM{int(tim)}" if lptim_dev else f"TIM{int(tim)}"]
					except: tim = tims[tim.upper()]
				clk, dev = tim
				if hrtim_dev:
					hrtim = input("sub_timer: ")
					try:    hrtim = int(hrtim)
					except: hrtim = int(ports[hrtim.upper()])
					sub |= hrtim << 3
				channel = max((int(input("channel: ")) - 1), 0)
				sub |= channel & 0x7
			elif uart_dev:
				uart = input("uart: ")
				try:    uart = uarts[f"LPUART{int(uart)}" if lpuart_dev else f"UART{int(uart)}"]
				except: uart = uarts[uart.upper()]
				clk, dev = uart
			else:
				clk = input("clk: ")
				try:    clk = int(clk)
				except: clk = int(clocks[clk.upper()])
				dev = int(input("offset: "), base=16) >> 10
			alt = int(input("alt: "))
			pin = input("pin: ")
			port = int(ports[pin[0].upper()])
			pin = int(pin[1:])
			res = (
				((pin & 0xf) << 24) |
				((port & 0xf) << 20) |
				((alt & 0xf) << 16) |
				((sub & 0x3f) << 10) |
				((clk & 0x1f) << 5) |
				(dev & 0x1f)
			)
			print(f"{res:#0{10}x}".upper().replace("X", "x"))
		except KeyboardInterrupt:   exit(0)
		except Exception as e:      print(e); pass