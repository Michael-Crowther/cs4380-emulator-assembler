import sys

def parse_line(line):
	#strip white space
	trimmed_line = line.strip()

	if not trimmed_line or trimmed_line.startswith(';'):
		return None

	#handle end of line comments
	code, _, comment = trimmed_line.partition(';')
	code = code.strip()

	#check if line is directive or instruction
	if code:
		#seperate labels from directives/instructions
		parts = code.split(maxsplit=1)
		if len(parts) == 1:
			#handles no label
			if parts[0].startswith('.'):
				return 'directive', None, parts[0], comment.strip()
			else:
				return 'instruction', None, parts[0], comment.strip()
	else:
			#handles labels
			label = parts[0]
			remainder = parts[1]
			if remainder.startswith('.'):
				return 'directive', label, remainder, comment.strip()
			else:
				return 'instruction', label, remainder, comment.strip()

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

	return symbol_table, bytecode


def process_instruction(instruction_line, line_num, symbol_table, bytecode):
	_, _, instruction, _ = instruction_line
	parts = instruction.split(maxsplit=1)
	operator = parts[0].lower() #handle case insensitive
	operands = parts[1].split(',') if len(parts) > 1 else []
	opcode_map = {
		'jmp': 1, 'mov': 7, 'movi': 8, 'lda': 9, 'str': 10, 'ldr': 11, 'stb': 12, 'ldb': 13,
		'add': 18, 'addi': 19, 'sub': 20, 'subi': 21, 'mul': 22, 'muli': 23, 'div': 24,
		'sdiv': 25, 'divi': 26
	}

	#check for invalid operator
	if operator not in opcode_map:
		print(f"Assembler error encountered on line {line_num}!")
		sys.exit(2)

	#convert operator to opcode
	opcode = opcode_map[operator]
	bytecode.append(opcode)
	
	#process operands from instruction type
	if operator in ['jmp', 'lda', 'str', 'ldr', 'stb', 'ldb']:
		for operand in operands:
			if operand.isdigit() or (operand.startswith('-') and operand[1:].isdigit()):
				#numeric operand
				value = int(operand)
				bytecode.extend(value.to_bytes(4, byteorder='little', signed=True))
			else:
				#resolve address of label
				address = symbol_table.get(operand.strip(), 0)
				bytecode.extend(address.to_bytes(4, byteorder='little', signed=False))

	elif operator in ['mov', 'add', 'sub', 'mul', 'div']:
		for operand in operands:
			if operand.lower().startswith('r'):
				#register operand
				reg_id = int(operand[1:])
				bytecode.append(reg_id)
			else:
				#immediate value
				value = int(operand)
				bytecode.extend(value.to_bytes(4, byteorder='little', signed=True))

	elif operator in ['movi', 'addi', 'subi', 'muli', 'sdiv', 'divi']:
		reg_operand = operands[0].lower()
		if reg_operand.startswith('r'):
			reg_id = int(reg_operand[1:])
			bytecode.append(reg_id)
		else:
			print(f"Assembler error encountered on line {line_num}!")
			sys.exit(2)

		#process immediate value
		imm_value_operand = operands[1]
		if imm_value_operand.isdigit() or (imm_value_operand.startswith('-') and imm_value_operand[1:].isdigit()):
			imm_value = int(imm_value_operand)
			bytecode.extend(imm_value.to_bytes(4, byteorder='little', signed=True))
		else:
			print(f"Assembler error encountered on line {line_num}!")
			sys.exit(2)

	return symbol_table, bytecode



def assemble(filename):
	#read in lines from the asm file
	try:
		with open(filename, 'r') as asm_file:
			lines = asm_file.readlines()
	except FileNotFoundError:
		print(f"Error: File {filename} not found.")
		sys.exit(1)

	symbol_table = {}
	bytecode = []

	for line_num, line in enumerate(lines, 1):
		parsed_line = parse_line(line)

		if not parsed_line:
			continue #skip empty lines and comments

		line_type, label, components = parsed_line

		if line_type == 'directive':
			symbol_table, bytecode = process_directive(parsed_line, line_num, symbol_table, bytecode)
			pass
		elif line_type == 'instruction':
			symbol_table, bytecode = process_instruction(parsed_line, line_num, symbol_table, bytecode)
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
