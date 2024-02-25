import sys

def char_to_ascii(operand, instruction_bytes):
	char_value = eval(operand)
	value = ord(char_value)
	instruction_bytes.extend(value.to_bytes(4, byteorder='little', signed=True))
	return instruction_bytes

def parse_line(line, line_num, unresolved_labels):
	trimmed_line = line.strip() #remove whitespace from start and end of string

	if not trimmed_line or trimmed_line.startswith(';'): #does not parse comments
		return None, unresolved_labels

	#handles comments at end of line
	code, _, comment = trimmed_line.partition(';')
	code = code.strip().lower()

	if not code:
		return None, unresolved_labels

	#check for multiple directives and instructions on one line
	directive_keywords = ['.byt', '.int']
	instruction_keywords = [
    'jmp', 'mov', 'movi', 'lda', 'str', 'ldr', 'stb', 'ldb',
    'add', 'addi', 'sub', 'subi', 'mul', 'muli', 'div',
    'sdiv', 'divi', 'trp'
  ]

	directive_count = sum(code.count(keyword) for keyword in directive_keywords)
	instruction_count = sum(code.count(keyword) for keyword in instruction_keywords)

	if directive_count > 1 or instruction_count > 1:
		print(f"Assembler error encountered on line {line_num}!")
		sys.exit(2)

	#start parsing line parts
	parts = code.split(maxsplit=1)

	if len(parts) > 1 and parts[1].startswith('.'):
		#label then directive
		label = parts[0] if not parts[0].startswith('.') else None
		directive = parts[1]
		operand = ' '.join(parts[2:]) if len(parts) > 2 else None
		return ('directive', label, f"{directive} {operand}" if operand else directive), unresolved_labels

	elif parts[0].endswith(':'):
		label = parts[0][:-1]
		if label and label.lower() in unresolved_labels:
			unresolved_labels.remove(label)
		#check if there is more after label
		if len(parts) > 1:
			#split further to identify instruction/directive
			next_parts = parts[1].split(maxsplit=1)
			if next_parts[0].startswith('.'):
				directive = next_parts[0]
				operand = next_parts[1] if len(next_parts) == 2 else None
				return ('directive', label, f"{directive} {operand}" if operand else directive), unresolved_labels
			else:
				return ('instruction', label, parts[1]), unresolved_labels
		else: #just a label
			return ('label', label, None), unresolved_labels

	#check if line starts with directive
	elif parts[0].startswith('.'):
		directive = parts[0]
		operand = parts[1] if len(parts) == 2 else None
		return ('directive', None, f"{directive} {operand}" if operand else directive), unresolved_labels

	else:
		return ('instruction', None, code), unresolved_labels

	return None, unresolved_labels


def process_directive(directive_line, line_num, symbol_table, bytecode):
	line_type, label, components = directive_line
	directive_parts = components.split(maxsplit=1)
	directive = directive_parts[0].lower() #handle case-insensitive
	operand = directive_parts[1] if len(directive_parts) > 1 else None
	
	if directive == ".int":
		value = int(operand[1:]) if operand else 0
		bytes_to_add = value.to_bytes(4, byteorder='little', signed=True)

	elif directive == ".byt":
		if operand.startswith("'"):
			char_value = ord(eval(operand)) #handles escape characters like \n or \t
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


def process_instruction(instruction_line, line_num, symbol_table, bytecode, unresolved_labels):
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
			if operand.startswith("'"):
				instruction_bytes = char_to_ascii(operand, instruction_bytes)
			elif operand.isdigit() or (operand.startswith('-') and operand[1:].isdigit()):
				#numeric operand
				value = int(operand)
				instruction_bytes.extend(value.to_bytes(4, byteorder='little', signed=True))
			elif operand in symbol_table:
				address = symbol_table[operand]
				instruction_bytes.extend(address.to_bytes(4, byteorder='little', signed=True))
			else:
				#resolve address of label
				address = symbol_table.get(operand.strip(), 0)
				instruction_bytes.extend(address.to_bytes(4, byteorder='little', signed=False))
				unresolved_labels.add(operand)

	elif operator in ['mov', 'add', 'sub', 'mul', 'div']:
		for operand in operands:
			operand = operand.strip()
			if operand.startswith("'"):
				instruction_bytes = char_to_ascii(operand, instruction_bytes)

			elif operand.lower().startswith('r'):
				#register operand
				reg_id = int(operand[1:])
				instruction_bytes.append(reg_id)
				#immediate value
			elif operand.startswith('#'):
				operand = operand[1:]
				value = int(operand)
				instruction_bytes.extend(value.to_bytes(4, byteorder='little', signed=True))
			elif operand in symbol_table:
				address = symbol_table.get(operand.strip(), 0)
				instruction_bytes.extend(address.to_bytes(4, byteorder='little', signed=False))
			else:
				unresolved_labels.add(operand)
				#print(f"Assembler error encountered on line {line_num}!")
				#sys.exit(2)

	elif operator in ['movi', 'addi', 'subi', 'muli', 'sdiv', 'divi']:
		reg_operand = operands[0].lower()
		if operand.startswith("'"):
			instruction_bytes = char_to_ascii(operand, instruction_bytes)

		elif operand in symbol_table:
			address = symbol_table.get(operand.strip(), 0)
			instruction_bytes.extend(address.to_bytes(4, byteorder='little', signed=False))

		elif reg_operand.startswith('r'):
			reg_id = int(reg_operand[1:])
			instruction_bytes.append(reg_id)
		else:
			unresolved_labels.add(operand)

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
	
	while len(instruction_bytes) < 8:
		instruction_bytes.append(0)

	bytecode.extend(instruction_bytes)

	return bytecode, unresolved_labels



def assemble(filename):
	#read in lines from the asm file
	try:
		with open(filename, 'r') as asm_file:
			lines = asm_file.readlines()
	except FileNotFoundError:
		print(f"Error: File {filename} not found.")
		sys.exit(1)

	symbol_table = {}
	unresolved_labels = set()
	address_counter = 0
	in_code_section = False
	first_instruction_checked = False
	bytecode = []

	for line_num, line in enumerate(lines, 1):

		parsed_line, unresolved_labels = parse_line(line, line_num, unresolved_labels)

		if not parsed_line:
			continue #skip empty lines and comments

		line_type, label, components = parsed_line

		#check for validity of label
		if label in symbol_table or label and not label[0].isalnum():
			print(f"Assembler error encountered on line {line_num}!")
			sys.exit(2)

		if label:
			for char in label:
				if not (char.isalnum() or char == '_' or char == '$'):
					print(f"Assembler error encountered on line {line_num}!")
					sys.exit(2)

		if label:
			symbol_table[label] = address_counter
		address_counter += 8

		if line_type == 'directive':
			if in_code_section:
				print(f"Assembler error encountered on line {line_num}!")
				sys.exit(2)
			bytecode = process_directive(parsed_line, line_num, symbol_table, bytecode)
			pass
		elif line_type == 'instruction':
			in_code_section = True
			if not first_instruction_checked:
				first_instruction_checked = True
				if components.upper().strip() != 'JMP MAIN':
					print(f"Assembler error encountered on line {line_num}!")
					sys.exit(2)
			bytecode, unresolved_labels = process_instruction(parsed_line, line_num, symbol_table, bytecode, unresolved_labels)
			pass
		else:
			print(f"Assembler error encountered on line {line_num}!")
			sys.exit(2)

	#throw error if any label called in an operand is not a valid function name
	if len(unresolved_labels) > 0:
		print(f"Assembler error encountered on line {line_num}!")
		sys.exit(2)

	#write the output file
	output_filename = filename.replace('.asm', '.bin')
	with open(output_filename, 'wb') as bin_file:
		bin_file.write(bytearray(bytecode))

	print(f"Assembly completed successfully. Output file: {output_filename}")
	sys.exit(0)


if __name__ == "__main__":
	if len(sys.argv) != 2 or not sys.argv[1].endswith('.asm'):
		print("USAGE: python3 asm4380.py inputFile.asm")
		sys.exit(1)

	input_filename = sys.argv[1]
	assemble(input_filename)
