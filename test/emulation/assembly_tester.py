from unicorn import *
from unicorn.arm_const import *
from capstone import *

from functools import partial


CODE = b"\xff\x46\xff\x46"


def code_hook(emu, address, size, user_data, d_asm):
	opcode =	emu.mem_read(address, size)
	mnemonics =	d_asm.disasm(opcode, address)
	#print(f"code: {hex(address)} -> {opcode.hex()}")
	for i in mnemonics:
		print(f"{hex(i.address)}: {i.op_str}\t\t{i.mnemonic}")


if __name__ == "__main__":
	emu = Uc(UC_ARCH_ARM, UC_MODE_THUMB)
	d_asm = Cs(CS_ARCH_ARM, CS_MODE_THUMB)
	d_asm.detail = True
	
	emu.mem_map(0x0, 0x1000)
	
	emu.mem_write(0x0, CODE)
	
	emu.hook_add(UC_HOOK_CODE,			partial(code_hook, d_asm=d_asm))
	
	emu.emu_start(0x0, 0x10)
	