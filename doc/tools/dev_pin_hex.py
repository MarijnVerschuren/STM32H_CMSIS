if __name__ == "__main__":
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
	
	while True:
		try:
			clk = input("clk: ")
			try:    clk = int(clk)
			except: clk = int(clocks[clk.upper()])
			dev = int(input("offset: "), base=16) >> 10
			sub = int(input("sub: "))
			alt = int(input("alt: "))
			port = input("port: ")
			try:    port = int(port)
			except: port = int(ports[port.upper()])
			pin = int(input("pin: "))
			res = (
				((pin & 0xf) << 24) |
				((port & 0xf) << 20) |
				((alt & 0xf) << 16) |
				((sub & 0x3f) << 10) |
				((clk & 0x1f) << 5) |
				(dev & 0x1f)
			)
			print(f"{res:#0{7}x}")
		except KeyboardInterrupt:   exit(0)
		except Exception as e:      print(e); pass