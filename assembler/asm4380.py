import sys

def parse_line(line):
	trimmed_line = line.strip() #remove whitespace from start and end of string

	if not trimmed_line or trimmed_line.startswith(';'): #does not parse comments
		return None

	#handles comments at end of line
	code, _, comment = trimmed_line.partition(';')
	code = code.strip()

	if not code:
		return None


	parts = code.split(maxsplit=1)
	#check if line is directive or instruction
	if len(parts) == 2 and parts[0].endswith(':'):
		#if two parts, first part could be a label
		label = parts[0][:-1] #remove colon from label
		remainder = parts[1]
		if remainder.startswith('.'):
			return ('directive', label, remainder)
		else:
			return ('instruction', label, remainder)
	elif len(parts) == 1:
			#no label here
			part = parts[0]
			if part.startswith('.'):
				return 'directive', None, part
			else:
				return 'instruction', None, part
	elif len(parts) == 2:
		directive_or_instruction, operands = parts
		if directive_or_instruction.startswith('.'):
			return ('directive', None, f"{directive_or_instruction} {operands}")
		else:
			return ('instruction', None, f"{directive_or_instruction} {operands}")

	return None


def process_directive(directive_line, line_num, symbol_table, bytecode):
	line_type, label, components, _ = directive_line
	directive_parts = components.split(maxsplit=1)
	directive = directive_parts[0].lower() #handle case-insensitive
	operand = directive_parts[1] if len(directive_parts) > 1 else None

	if directive == ".int":
		value = int(operand[1:]) if operand else 0
		bytes_to_add = value.to_bytes(4, byteorder='little', signed=True)

	elif directive == ".byt":
		if operand.startswith("'"):
			char_value = eval(operand) #handles escape characters like \n or \t
			bytes_to_add = bytes([char_value])
		else: #numeric val
			value = int(operand[1:]) if operand else 0
			bytes_to_add = bytes([value])

	else:
		print(f"Assembler error encountered on line {line_num}!")
		sys.exit(2)

	#update symbol table if there is a label
	if label:
		symbol_table[label] = len(bytecode)

	#extend bytecode
	bytecode.extend(bytes_to_add)

	return bytecode


def process_instruction(instruction_line, line_num, symbol_table, bytecode):
	_, _, instruction = instruction_line
	parts = instruction.split(maxsplit=1)
	operator = parts[0].lower() #handle case insensitive
	operands = parts[1].split(',') if len(parts) > 1 else []
	opcode_map = {
		'jmp': 1, 'mov': 7, 'movi': 8, 'lda': 9, 'str': 10, 'ldr': 11, 'stb': 12, 'ldb': 13,
		'add': 18, 'addi': 19, 'sub': 20, 'subi': 21, 'mul': 22, 'muli': 23, 'div': 24,
		'sdiv': 25, 'divi': 26, 'trp': 31
	}

	#check for invalid operator
	if operator not in opcode_map:
		print(f"Assembler error encountered on line {line_num}!")
		sys.exit(2)

	instruction_bytes = [opcode_map[operator]]
	
	#process operands from instruction type
	if operator in ['jmp', 'lda', 'str', 'ldr', 'stb', 'ldb']:
		for operand in operands:
			if operand.isdigit() or (operand.startswith('-') and operand[1:].isdigit()):
				#numeric operand
				value = int(operand)
				instruction_bytes.extend(value.to_bytes(4, byteorder='little', signed=True))
			else:
				#resolve address of label
				address = symbol_table.get(operand.strip(), 0)
				instruction_bytes.extend(address.to_bytes(4, byteorder='little', signed=False))

	elif operator in ['mov', 'add', 'sub', 'mul', 'div']:
		for operand in operands:
			operand = operand.strip()
			if operand.lower().startswith('r'):
				#register operand
				reg_id = int(operand[1:])
				instruction_bytes.append(reg_id)
			else:
				#immediate value
				if operand.startswith('#'):
					operand = operand[1:]
				value = int(operand)
				instruction_bytes.extend(value.to_bytes(4, byteorder='little', signed=True))

	elif operator in ['movi', 'addi', 'subi', 'muli', 'sdiv', 'divi']:
		reg_operand = operands[0].lower()
		if reg_operand.startswith('r'):
			reg_id = int(reg_operand[1:])
			instruction_bytes.append(reg_id)
		else:
			print(f"Assembler error encountered on line {line_num}!")
			sys.exit(2)

		#process immediate value
		imm_value_operand = operands[1]
		if imm_value_operand.isdigit() or (imm_value_operand.startswith('-') and imm_value_operand[1:].isdigit()):
			imm_value = int(imm_value_operand)
			instruction_bytes.extend(imm_value.to_bytes(4, byteorder='little', signed=True))
		else:
			print(f"Assembler error encountered on line {line_num}!")
			sys.exit(2)

	elif operator == 'trp':
		if len(operands) == 0:
			print(f"Assembler error encountered on line {line_num}!")
			sys.exit(2)
		if operands[0].startswith('#'):
			operands[0] = operands[0][1:]
		trap_code = int(operands[0])
		instruction_bytes.append(trap_code)
		#bytecode.extend((0).to_bytes(6, byteorder='little', signed=False))
	
	while len(instruction_bytes) < 8:
		instruction_bytes.append(0)

	bytecode.extend(instruction_bytes)

#	print(f"Line {line_num} bytecode: {instruction_bytes}")
	
	return bytecode



def assemble(filename):
	#read in lines from the asm file
	try:
		with open(filename, 'r') as asm_file:
			lines = asm_file.readlines()
	except FileNotFoundError:
		print(f"Error: File {filename} not found.")
		sys.exit(1)

	symbol_table = {}
	address_counter = 0
	bytecode = []

	for line_num, line in enumerate(lines, 1):
		parsed_line = parse_line(line)
		#print(f"Parsed line: {parsed_line}")

		if not parsed_line:
			continue #skip empty lines and comments

		line_type, label, components = parsed_line

		if label:
			symbol_table[label] = address_counter
		address_counter += 8

		if line_type == 'directive':
			bytecode = process_directive(parsed_line, line_num, symbol_table, bytecode)
			pass
		elif line_type == 'instruction':
			bytecode = process_instruction(parsed_line, line_num, symbol_table, bytecode)
			pass
		else:
			print(f"Assembler error encountered on line {line_num}!")
			sys.exit(2)

	#write the output file
	output_filename = filename.replace('.asm', '.bin')
	with open(output_filename, 'wb') as bin_file:
		bin_file.write(bytearray(bytecode))

	print(f"Assembly completed successfully. Output file: {output_filename}")


if __name__ == "__main__":
	if len(sys.argv) != 2 or not sys.argv[1].endswith('.asm'):
		print("USAGE: python3 asm4380.py inputFile.asm")
		sys.exit(1)

	input_filename = sys.argv[1]
	assemble(input_filename)
