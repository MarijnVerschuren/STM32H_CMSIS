import csv
import sys


if __name__ == "__main__":
	if len(sys.argv) > 1:	peripherals = sys.argv[1]
	else:					peripherals = input("peripherals: ")
	peripherals = peripherals.strip(" ").split(",")
	data = {}
	
	with open("peripheral_dump.csv", "r", newline="") as file:
		reader = csv.DictReader(file, delimiter=",")
		for row in reader:
			periph = row["Peripheral"]
			if not any([peripheral in periph for peripheral in peripherals]): continue
			if periph not in data: data[periph] = []
			data[periph].append(
				{key: val for key, val in row.items() if key != "Peripheral"}
			)
	
	for peripheral, registers in data.items():
		print(peripheral)
		for register in registers:
			reg = f"    {register['Register']}"
			print(reg, f"{' ' * (25 - len(reg))}=>\t", register["Value"])
			for field in register["Fields"].split(";"):
				field, val = field.split(":")
				field = f"        {field}"
				print(field, f"{' ' * (20 - len(field))}->\t", val)