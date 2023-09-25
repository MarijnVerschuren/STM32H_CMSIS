from sys import argv


if __name__ == "__main__":
	tim_dev = "-tim" in argv
	tim_dev |= (hrtim_dev := "-hrtim" in argv)
	tim_dev |= (lptim_dev := "-lptim" in argv)
	uart_dev = "-uart" in argv
	uart_dev |= (lpuart_dev := "-lpuart" in argv)
	i2c_dev = "-i2c" in argv
	usb_dev = "-usb" in argv
	
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
		"TIM1": ("APB2", 0),     "TIM2": ("APB1", 0),
		"TIM3": ("APB1", 1),     "TIM4": ("APB1", 2),
		"TIM5": ("APB1", 3),     "TIM6": ("APB1", 4),
		"TIM7": ("APB1", 5),     "TIM8": ("APB2", 1),
		"TIM12": ("APB1", 6),    "TIM13": ("APB1", 7),
		"TIM14": ("APB1", 8),    "TIM15": ("AHB1", 16),
		"TIM16": ("AHB1", 17),   "TIM17": ("AHB1", 18),
		"HRTIM": ("APB2", 29),   "LPTIM1": ("APB1", 9),
		"LPTIM2": ("APB4", 9),   "LPTIM3": ("APB4", 10),
		"LPTIM4": ("APB4", 11),  "LPTIM5": ("APB4", 12)
	}
	uarts = {
		"UART1": ("APB2", 4),    "UART2": ("APB1", 17),
		"UART3": ("APB1", 18),   "UART4": ("APB1", 19),
		"UART5": ("APB1", 20),   "UART6": ("APB2", 5),
		"UART7": ("APB1", 30),   "UART8": ("APB1", 31),
		"LPUART1": ("APB4", 3)
	}
	i2cs = {
		"I2C1": ("APB1", 21),    "I2C2": ("APB1", 22),
		"I2C3": ("APB1", 23),    "I2C4": ("APB4", 7)
	}
	usbs = {
		"USB1": ("AHB1", 25),    "USB2": ("AHB1", 27)
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
				clk = clocks[clk]
				if hrtim_dev:
					hrtim = input("sub_timer: ")
					try:    hrtim = int(hrtim)
					except: hrtim = ports[hrtim.upper()]
					sub |= hrtim << 3
				channel = max((int(input("channel: ")) - 1), 0)
				sub |= channel & 0x7
			elif uart_dev:
				uart = input("uart: ")
				try:    uart = uarts[f"LPUART{int(uart)}" if lpuart_dev else f"UART{int(uart)}"]
				except: uart = uarts[uart.upper()]
				clk, dev = uart
				clk = clocks[clk]
			elif i2c_dev:
				i2c = input("i2c: ")
				try:    i2c = i2cs[f"I2C{int(i2c)}"]
				except: i2c = i2cs[i2c.upper()]
				clk, dev = i2c
				clk = clocks[clk]
			elif usb_dev:
				usb = input("usb: ")
				ulpi = input("ulpi?") != ""
				print("ulpi " + ("ON" if ulpi else "OFF"))
				try:
					clk, dev = usbs[f"USB{int(usb)}"]
					if ulpi: sub = (0x1 << 5) | ((dev + 1) & 0x1f)  # clock is always AHB1
				except:
					clk, dev = usbs[usb.upper()]
					if ulpi: sub = (0x1 << 5) | ((dev + 1) & 0x1f)  # clock is always AHB1
				clk = clocks[clk]
			else:
				clk = input("clk: ")
				try:    clk = int(clk)
				except: clk = clocks[clk.upper()]
				dev = int(input("offset: "), base=16) >> 10
			alt = int(input("alt: "))
			pin = input("pin: ")
			port = int(ports[pin[0].upper()])
			pin = int(pin[1:])
			res = (
				((pin & 0xf) << 24)     |
				((port & 0xf) << 20)    |
				((alt & 0xf) << 16)     |
				((sub & 0x3f) << 10)    |   # |
				((clk & 0x1f) << 5)     |   # - dev_id
				(dev & 0x1f)                # |
			)
			print(hex(sub))
			print(f"{res:#0{10}x}".upper().replace("X", "x"))
		except KeyboardInterrupt:   exit(0)
		except Exception as e:      print(e); pass
