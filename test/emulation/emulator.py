from unicorn import *
from unicorn.arm_const import *
from capstone import *

from struct import pack
from functools import partial
from re import finditer
import json, sys, os


# helper functions
word =		partial(pack, "<L")
dir_name =	os.path.dirname

# init disassembler
asm = Cs(CS_ARCH_ARM, UC_MODE_THUMB); asm.detail = True
emu = Uc(UC_ARCH_ARM, UC_MODE_THUMB)

# debug barrier (marker)
#define DBAR() asm volatile("mov pc, pc\n\tmov pc, pc")
DBAR = b"\xff\x46\xff\x46"


# memory map
FLASH_0 =		0x08000000	# load address
FLASH_1 =		0x08100000
FLASH_END =		0x08020000

PERIPH =		0x40000000
PERIPH_END =	0x60000000	# only one access outside: 0x58028000

UNKNOWN =		0xe0000000
UNKNOWN_END =	0xe0010000	# only one access outside: 0x58028000

VAR_BASE =		0x24000000

# sys config settings
SYS_CONFIG = {
	"CORE_VOS":			0b11,
	# clocks
	"HSI_enable":		0b0,
	"HSE_enable":		0b1,
	"CSI_enable":		0b0,
	"HSI48_enable":		0b1,
	"HSE_CSS_enable":	0b1,
	"LSI_enable":		0b0,
	"LSE_enable":		0b0,
	# PLL's
	"PLL1_enable":		0b1,
	"PLL2_enable":		0b1,
	"PLL3_enable":		0b0,
	# SYS clk
	"SYS_CLK_SRC":		0b011,
}

# TODO: write this based on DBAR markers (json?)
class SET():
	def __init__(self, address, data):
		self.address =	address
		self.data =		data
	
	def run(self, emu, *args, **kwargs):
		emu.mem_write(self.address, self.data)
		

class SKIP():
	def __init__(self, f_name):
		self.f_name = f_name
	
	def run(self, emu, *args, user_data, **kwargs):
		address, size = [(a, s) for a, s, n in user_data["functions"] if n == self.f_name][0]
		emu.reg_write(UC_ARM_REG_PC, address + size)


DBAR_ACTIONS = {
	"sys_clock_init_0": SET(0x58024818, word(0b1 << 13)),							# PWR->D3CR & PWR_D3CR_VOSRDY
	"sys_clock_init_1": SET(0x58024804, word(SYS_CONFIG["CORE_VOS"] << 14)),		# PWR->CSR1 & PWR_CSR1_ACTVOS
	"sys_clock_init_2": SET(0x58024400, word(
		(SYS_CONFIG["HSI_enable"] << 2)		|
		(SYS_CONFIG["HSE_enable"] << 17)	|
		(SYS_CONFIG["CSI_enable"] << 8)		|
		(SYS_CONFIG["HSI48_enable"] << 13)	|
		(SYS_CONFIG["HSE_CSS_enable"] << 19)												# RCC->CR & clock_ready_mask
	)),
	"sys_clock_init_3": SET(0x58024400, word(0b1 << 5)),							# RCC->CR & RCC_CR_HSIDIVF
	"sys_clock_init_4": SET(0x58024400, word(
		(SYS_CONFIG["PLL1_enable"] << 25)	|
		(SYS_CONFIG["PLL2_enable"] << 27)	|
		(SYS_CONFIG["PLL3_enable"] << 29)													# RCC->CR & clock_ready_mask
	)),
	"sys_clock_init_5": SET(0x58024410, word(SYS_CONFIG["SYS_CLK_SRC"] << 3)),		# RCC->CFGR & RCC_CFGR_SWS
	"sys_clock_init_6": SET(0x58024474, word(SYS_CONFIG["LSI_enable"] << 1)),		# RCC->CSR & RCC_CSR_LSIRDY
	"sys_clock_init_7": SET(0x58024470, word(SYS_CONFIG["LSE_enable"] << 1)),		# RCC->BDCR & RCC_BDCR_LSERDY

	"enable_id_0" : SKIP("enable_id")	# skip the function containing the barrier
}  # TODO: USB DBAR's


# hooks
def memory_invalid_hook(emu, access, address, size, value, user_data):
	print(f"invalid: {access}, {hex(address)}, {size}, {value}: {hex(value)}")
	return False
	

def memory_read_hook(emu, access, address, size, value, user_data):
	print(f"read: {access}, {hex(address)}, {size}, {value}")


def memory_write_hook(emu, access, address, size, value, user_data):
	print(f"write: {access}, {hex(address)}, {size}, {value}")
	

def code_hook(emu, address, size, user_data):
	opcode =	emu.mem_read(address, size)
	mnemonics =	asm.disasm(opcode, address)
	for i in mnemonics:
		print(f"{hex(i.address)}: {i.mnemonic}\t{i.op_str}")

def interrupt_hook(emu, address, size, user_data):
	print("interrupt")

# custom actions on debug barrier
def barrier_hook(emu, address, size, user_data):
	f_name, bar_cnt = user_data["barriers"][address]
	print(f"barrier: {f_name}_{bar_cnt} @ {address}")
	if (f"{f_name}_{bar_cnt}" not in DBAR_ACTIONS):	input(); return  # breakpoint
	DBAR_ACTIONS[f"{f_name}_{bar_cnt}"].run(emu, user_data=user_data)



if __name__ == "__main__":
	# build selection and commands
	os.chdir(dir_name(dir_name(os.getcwd())))
	envs = os.popen("cat platformio.ini | grep env: | sed 's/.*env://' | sed 's/]//'").read()
	envs = envs.split("\n")[:-1]
	for i, env in enumerate(envs):
		print(f"[{i}] ->\t{env}")
	
	while True:
		try:
			env = envs[int(input("select program: "))]
			break
		except	KeyboardInterrupt: sys.exit(0)
		except:	pass
		
	os.system(f"pio run -e {env}")
	os.system(f"cp ./.pio/build/{env}/firmware.bin ./test/emulation/{env}.bin")
	os.system(f"cp ./.pio/build/{env}/firmware.elf ./test/emulation/{env}.elf")
	os.chdir(dir_name(__file__))
	
	# read firmware and set address
	ADDRESS = FLASH_0
	with open(f"{env}.bin", "rb") as prog:
		CODE = prog.read()
		prog.close()
	
	# analyse code
	symbols = os.popen(
		"arm-none-eabi-readelf STM32H750_optimized.elf -Ws |"	+
		"grep -E 'FUNC|OBJECT|SECTION' |"						+
		"grep -E 'LOCAL|GLOBAL' |"								+
		"sed 's/.*: //'"
	).read().split("\n")
	sp =		int.from_bytes(CODE[0:4], "little")
	entry =		int.from_bytes(CODE[4:8], "little")
	sections =	sorted([(int(s[0:8], 16), s[43:]) for s in symbols if "SECTION" in s],				key=lambda x: x[0])
	functions =	sorted([(int(s[0:8], 16), int(s[8:14]), s[43:]) for s in symbols if "FUNC" in s],	key=lambda x: x[0])
	variables =	sorted([(int(s[0:8], 16), int(s[8:14]), s[43:]) for s in symbols if "OBJECT" in s],	key=lambda x: x[0])
	bars =		sorted([ADDRESS + bar.start() for bar in finditer(DBAR, CODE)], reverse=True)
	barriers =	{}
	for f_address, f_size, f_name in functions[::-1]:
		bar_cnt = 0
		for addr in bars[::-1]:
			if addr > f_address:
				barriers.update({addr: (f_name, bar_cnt)})
				bar_cnt += 1; bars.remove(addr)
	
	# setup memory map and code loading
	emu.mem_map(FLASH_0, FLASH_1 - FLASH_0)			# map flash memory
	emu.mem_map(PERIPH, PERIPH_END - PERIPH)		# map peripheral space
	emu.mem_map(UNKNOWN, UNKNOWN_END - UNKNOWN)		# map unknown space
	emu.mem_map(VAR_BASE, 0x100000)			# map variable space
	
	emu.mem_write(ADDRESS, CODE)					# load code
	
	# add hooks
	emu.hook_add(
		UC_HOOK_MEM_READ_UNMAPPED	|
		UC_HOOK_MEM_WRITE_UNMAPPED	|
		UC_HOOK_MEM_INVALID,
		memory_invalid_hook
	)
	emu.hook_add(UC_HOOK_MEM_READ,		memory_read_hook)
	emu.hook_add(UC_HOOK_MEM_WRITE,		memory_write_hook)
	emu.hook_add(UC_HOOK_CODE,			code_hook)
	emu.hook_add(UC_HOOK_INTR,			interrupt_hook)
	for barrier in barriers.keys():
		emu.hook_add(
			UC_HOOK_CODE, 				barrier_hook,
			begin=barrier, end=barrier, user_data={
				"barriers": barriers,
				"sections": sections,
				"functions": functions,
				"variables": variables,
			}
		)
	
	# initialize machine state
	emu.reg_write(UC_ARM_REG_SP, sp)
	
	# start emulation
	try:
		emu.emu_start(entry, ADDRESS + len(CODE))
	except UcError as e:
		print(e)
	
	# dump machine state TODO
	