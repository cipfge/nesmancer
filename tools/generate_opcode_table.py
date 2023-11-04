import json

am_type_dic = {
    "implied":     "CPU::AM_IMPLIED",
    "accumulator": "CPU::AM_IMPLIED",
    "immediate":   "CPU::AM_IMMEDIATE",
    "absolute":    "CPU::AM_ABSOLUTE",
    "absoluteX":   "CPU::AM_ABSOLUTE_INDEXED_X",
    "absoluteY":   "CPU::AM_ABSOLUTE_INDEXED_Y",
    "relative":    "CPU::AM_RELATIVE",
    "zeropage":    "CPU::AM_ZEROPAGE",
    "zeropageX":   "CPU::AM_ZEROPAGE_INDEXED_X",
    "zeropageY":   "CPU::AM_ZEROPAGE_INDEXED_Y",
    "indirect":    "CPU::AM_INDIRECT",
    "indirectX":   "CPU::AM_INDEXED_INDIRECT",
    "indirectY":   "CPU::AM_INDIRECT_INDEXED"
}

am_func_dic = {
    "implied":     "CPU::read_implied",
    "accumulator": "CPU::read_implied",
    "immediate":   "CPU::read_immediate",
    "absolute":    "CPU::read_absolute",
    "absoluteX":   "CPU::read_absolute_x",
    "absoluteY":   "CPU::read_absolute_y",
    "relative":    "CPU::read_relative",
    "zeropage":    "CPU::read_zeropage",
    "zeropageX":   "CPU::read_zeropage_x",
    "zeropageY":   "CPU::read_zeropage_y",
    "indirect":    "CPU::read_indirect",
    "indirectX":   "CPU::read_indexed_indirect",
    "indirectY":   "CPU::read_indirect_indexed"
}

if __name__ == '__main__':
    json_file = open("cpu_opcodes.json")
    opcodes = json.load(json_file)

    for op in opcodes:
        mnemonic = op["mnemonics"][0]
        mnemonic += ","

        am_name = "CPU::AM_IMPLIED"
        am_func = "CPU::read_implied"
        if "addressingMode" in op:
            am_name = am_type_dic[op["addressingMode"]]
            am_func = am_func_dic[op["addressingMode"]]

        am_name += ","
        am_func += ","

        cycles = 2
        if "cycles" in op:
            cycles = op["cycles"]

        op_func = "CPU::op_" + mnemonic.lower()

        print(f"{{ \"{mnemonic:<3}\" {am_name:<27} &{op_func} &{am_func:<27} {cycles} }}, // 0x{op['opcode']:X}")
