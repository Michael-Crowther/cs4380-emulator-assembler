import sys

def char_to_ascii(operand, instruction_bytes):
	char_value = eval(operand)
	value = ord(char_value)
	instruction_bytes.extend(value.to_bytes(4, byteorder='little', signed=True))
	return instruction_bytes

def parse_line(line, line_num, unresolved_labels, symbol_table, bytecode, starting_bytes):
	trimmed_line = line.strip() #remove whitespace from start and end of string

	if not trimmed_line or trimmed_line.startswith(';'): #does not parse comments
		return None, unresolved_labels, bytecode

	#handles comments at end of line
	code, _, comment = trimmed_line.partition(';')
	code = code.strip().lower()

	if not code:
		return None, unresolved_labels, bytecode

	#print(f"code: {code}")

	#check for multiple directives and instructions on one line
	directive_keywords = ['.byt', '.int', '.bts', '.str']
	instruction_keywords = [
    'jmp', 'mov', 'movi', 'lda', 'str', 'ldr', 'stb', 'ldb',
    'add', 'addi', 'sub', 'subi', 'mul', 'muli', 'div',
    'sdiv', 'divi', 'trp', 'istr', 'ildr', 'istb', 'ildb', 'jmr',
		'bnz', 'bgt', 'blt', 'cmp', 'cmpi', 'and', 'or', 'pshr', 'pshb',
		'popr', 'popb', 'call', 'ret', 'alci', 'allc', 'iallc'
  ]

	words = code.split()

	# Initialize counts
	directive_count = 0
	instruction_count = 0

	# Find exact matches in the split words for directives and instructions
	for word in words:
		if word in directive_keywords:
			directive_count += 1
		if word in instruction_keywords:
			instruction_count += 1

	if directive_count > 1 or instruction_count > 1:
		print(f"Assembler error encountered on line {line_num}!")
		sys.exit(2)

	#start parsing line parts
	parts = code.split(maxsplit=1)

	#print(f"parsed line parts: {parts}")

	if len(parts) > 1 and parts[1].startswith('.'):
		#label then directive
		label = parts[0] if not parts[0].startswith('.') else None
		directive = parts[1]
		operand = ' '.join(parts[2:]) if len(parts) > 2 else None
		return ('directive', label, f"{directive} {operand}" if operand else directive), unresolved_labels, bytecode

	elif parts[0].lower() in unresolved_labels:
		label = parts[0].lower()
		#print(f"removing unresolved_label: {label}")
 
		# resolve label of later found function call
		for label, address_pos in unresolved_labels.items():
			if label != 'main':
				address = starting_bytes - 8
				#print(f"address converting: {address} for: {label}")
				address_bytes = address.to_bytes(2, byteorder='little', signed=True)
				bytecode[address_pos - 7] = address_bytes[0]
		   

		unresolved_labels.pop(label)
		#check if there is more after label
		if len(parts) > 1:
			#split further to identify instruction/directive
			next_parts = parts[1].split(maxsplit=1)
			if next_parts[0].startswith('.'):
				directive = next_parts[0]
				operand = next_parts[1] if len(next_parts) == 2 else None
				return ('directive', label, f"{directive} {operand}" if operand else directive), unresolved_labels, bytecode
			else:
				return ('instruction', label, parts[1]), unresolved_labels, bytecode
		else: #just a label
			return ('label', label, None), unresolved_labels, bytecode

	#check if line starts with directive
	elif parts[0].startswith('.'):
		directive = parts[0]
		operand = parts[1] if len(parts) == 2 else None
		return ('directive', None, f"{directive} {operand}" if operand else directive), unresolved_labels, bytecode

	elif len(parts) > 1 and parts[0].lower() not in instruction_keywords:
		#label then instruction
		label = parts[0]
		instruction_and_operands = parts[1].split(maxsplit=1)
		instruction = instruction_and_operands[0]
		operand = instruction_and_operands[1] if len(instruction_and_operands) > 1 else None
		return ('instruction', label, f"{instruction} {operand}" if operand else instruction), unresolved_labels, bytecode

	else:
		return ('instruction', None, code), unresolved_labels, bytecode

	return None, unresolved_labels, bytecode


def process_directive(directive_line, line_num, symbol_table, bytecode, variables, starting_bytes):
	line_type, label, components = directive_line
	directive_parts = components.split(maxsplit=1)
	directive = directive_parts[0].lower() #handle case-insensitive
	operand = directive_parts[1] if len(directive_parts) > 1 else None

	#store label as variable
	variables.add(label)
	
	if directive == ".int":
		value = int(operand[1:]) if operand else 0
		starting_bytes += 4
		#print(f"adding 4 bytes for .int -- Total: {starting_bytes}")
		bytes_to_add = value.to_bytes(4, byteorder='little', signed=True)

	elif directive == ".byt":
		if operand.startswith("'"):
			char_value = ord(eval(operand)) #handles escape characters like \n or \t
			bytes_to_add = bytes([char_value])
		else: #numeric val
			value = int(operand[1:]) if operand else 0
			bytes_to_add = bytes([value])
		starting_bytes += 1
		#print(f"adding 1 byte for .byt -- Total: {starting_bytes}")

	elif directive == ".bts":
		size = int(operand[1:]) if operand else 0
		bytes_to_add = bytes([0] * size)  # Allocate space initialized to zero
		starting_bytes += size

	elif directive == '.str':
		if operand.startswith('"') or operand.startswith("'"):
    # Remove the quotes and handle escape characters
			string_value = eval(operand)
			length = len(string_value)
			if length > 255:
				print(f"Assembler error encountered on line {line_num}!")
				sys.exit(2)
			bytes_to_add = bytes([length]) + string_value.encode() + bytes([0])
		else:
    # Numeric literal handling
			numeric_value = int(operand[1:]) if operand else 0
			if numeric_value > 255:
				print(f"Assembler error encountered on line {line_num}!")
				sys.exit(2)
			bytes_to_add = bytes([numeric_value] + [0] * (numeric_value + 1))
		starting_bytes += len(bytes_to_add)

	else:
		print(f"Assembler error encountered on line {line_num}!")
		sys.exit(2)

	#update symbol table if there is a label
	if label:
		symbol_table[label] = len(bytecode)

	#extend bytecode
	bytecode.extend(bytes_to_add)

	return variables, bytecode, starting_bytes


def process_instruction(instruction_line, line_num, symbol_table, bytecode, unresolved_labels, starting_bytes):
	_, _, instruction = instruction_line
	parts = instruction.split(maxsplit=1)
	operator = parts[0].lower() #handle case insensitive
	operands = parts[1].split(',') if len(parts) > 1 else []
	opcode_map = {
		'jmp': 1, 'jmr': 2, 'bnz': 3, 'bgt': 4, 'blt': 5, 'mov': 7, 'movi': 8, 'lda': 9, 'str': 10, 'ldr': 11, 'stb': 12, 'ldb': 13,
		'istr': 14, 'ildr': 15, 'istb': 16, 'ildb': 17,
		'add': 18, 'addi': 19, 'sub': 20, 'subi': 21, 'mul': 22, 'muli': 23, 'div': 24,
		'sdiv': 25, 'divi': 26, 'cmp': 29, 'cmpi': 30, 'trp': 31,
		'and': 27, 'or': 28, 'pshr': 35, 'pshb': 36, 'popr': 37, 'popb': 38,
		'call': 39, 'ret': 40, 'alci': 32, 'allc': 33, 'iallc': 34
	}
	register_map = {'r0': 0, 'r1': 1, 'r2': 2, 'r3': 3, 'r4': 4, 'r5': 5, 'r6': 6, 'r7': 7, 'r8': 8, 'r9': 9, 'r10': 10, 'r11': 11, 'r12': 12, 'r13': 13, 'r14': 14, 'r15': 15, 'pc': 16, 'sl': 17, 'sb': 18, 'sp': 19, 'fp': 20, 'hp': 21}

	#print(f"got to process instruction")
	#print(f"instruction: {instruction}")
	#print(f"parts: {parts}")
	#print(f"{unresolved_labels}")
	#print(f"operator: {operator}")

	#check for invalid operator
	if operator not in opcode_map:
		print(f"Assembler error encountered on line {line_num}!")
		sys.exit(2)

	instruction_bytes = [opcode_map[operator]]
	#process operands from instruction type
	if operator in ['jmp', 'lda', 'str', 'ldr', 'stb', 'ldb', 'bnz', 'bgt', 'blt', 'popr', 'popb', 'call', 'ret']:
		#print(f"operator: {operator}")
		#print(f"operands: {operands}")
		for operand in operands:
			#print(f"operand: {operand}")
			if operand.startswith("'"):
				instruction_bytes = char_to_ascii(operand, instruction_bytes)
			elif operand.isdigit() or (operand.startswith('-') and operand[1:].isdigit()):
				#numeric operand
				value = int(operand)
				instruction_bytes.extend(value.to_bytes(4, byteorder='little', signed=True))
			elif operand in symbol_table:
				address = symbol_table[operand.strip()]
				address_bytes = address.to_bytes(4, byteorder='little', signed=True)
				#modify second byte to be starting_bytes
				address_bytes_list = list(address_bytes)
				address_bytes_list[3] = starting_bytes
				address_bytes = bytes(address_bytes_list)
				instruction_bytes.extend(address_bytes)
			if operand in register_map:
				reg_id = register_map[operand]
				instruction_bytes.append(reg_id)	
				#print(f"adding {reg_id} for {operator}")

				instruction_bytes.extend([0, 0])
			else:
				#resolve address of label
				address = symbol_table.get(operand.strip(), 0)
				address_bytes = address.to_bytes(4, byteorder='little', signed=True)
				
				# Modify the second byte here
				if operator in ['jmp']:
					address_bytes_list = list(address_bytes)
					address_bytes_list[3] = starting_bytes
					address_bytes = bytes(address_bytes_list)
					instruction_bytes.extend(address_bytes)
					#print(f"adding {address_bytes_list} for {operator}")
				else:
					#print(f"adding bytes for {operator}: {address}")
					#print(f"symbol table: {symbol_table}")
					instruction_bytes.extend(address_bytes)
				if operand.strip() not in symbol_table and operand.strip() not in register_map:
					unresolved_labels[operand.strip()] = len(bytecode) + 8

	elif operator in ['mov', 'add', 'sub', 'mul', 'div', 'sdiv', 'jmr', 'istr', 'ildr', 'istb', 'ildb', 'cmp', 'and', 'or', 'pshr', 'pshb', 'iallc']:
		for operand in operands:
			operand = operand.strip()
			if operand.startswith("'"):
				instruction_bytes = char_to_ascii(operand, instruction_bytes)

			elif operand in register_map:
				reg_id = register_map[operand]
				instruction_bytes.append(reg_id)
			elif operand.startswith('#'):
				operand = operand[1:]
				value = int(operand)
				instruction_bytes.extend(value.to_bytes(4, byteorder='little', signed=True))
			elif operand in symbol_table:
				address = symbol_table.get(operand.strip(), 0)
				instruction_bytes.extend(address.to_bytes(4, byteorder='little', signed=False))
			else:
				#print(f"adding unresolved label: {operand.strip()}")
				unresolved_labels[operand.strip()] = len(instruction_bytes)

	elif operator in ['movi', 'addi', 'subi', 'muli', 'divi', 'cmpi', 'alci', 'allc']:
		reg_operand_1 = operands[0].strip().lower()
		# Handle register operand
		if reg_operand_1 in register_map:
			reg_id = register_map[reg_operand_1]
			instruction_bytes.append(reg_id)
		else:
			print(f"Assembler error encountered on line {line_num}!")  
			sys.exit(2)

		reg_operand_2 = operands[1].strip().lower() if len(operands) > 1 else None
		if reg_operand_2 and reg_operand_2 in register_map:
			reg_id = register_map[reg_operand_2]
			instruction_bytes.append(reg_id)
			instruction_bytes.extend([0])
		else:
    	# If there's no second register operand, append a placeholder zero
			instruction_bytes.extend([0, 0])

		# Initially extend with zeros to later fill with immediate value
		instruction_bytes.extend([0, 0, 0, 0])

    # Handle immediate value operand
		imm_value_operand = operands[-1].strip()

		if imm_value_operand.startswith('#'):
			imm_value_operand = imm_value_operand[1:]

		if imm_value_operand.isdigit() or (imm_value_operand.startswith('-') and imm_value_operand[1:].isdigit()):
			imm_value = int(imm_value_operand)
			instruction_bytes[-4:] = imm_value.to_bytes(4, byteorder='little', signed=True)

    # If operand starts with a quote, treat as character
		elif imm_value_operand.startswith("'"):
			instruction_bytes[-4:] = char_to_ascii(imm_value_operand, [])

    # If operand is in the symbol table, treat as variable label
		elif imm_value_operand in symbol_table:
			# Fetch value or address associated with label
			value_or_address = symbol_table[imm_value_operand] + 4
			instruction_bytes[-4:] = value_or_address.to_bytes(4, byteorder='little', signed=False)

		# If operand is unrecognized, it's unresolved
		else:
			unresolved_labels.add((imm_value_operand, len(instruction_bytes) - 4))

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

	return bytecode, unresolved_labels, starting_bytes



def assemble(filename):
	#read in lines from the asm file
	try:
		with open(filename, 'r') as asm_file:
			lines = asm_file.readlines()
	except FileNotFoundError:
		print(f"Error: File {filename} not found.")
		sys.exit(1)

	symbol_table = {}
	unresolved_labels = {}
	variables = set()
	instructions_processed = 0
	starting_bytes = 12; # -- 4 for first unsigned int + 8 to jump to main
	in_code_section = False
	first_instruction_checked = False
	bytecode = []

	for line_num, line in enumerate(lines, 1):

		parsed_line, unresolved_labels, bytecode = parse_line(line, line_num, unresolved_labels, symbol_table, bytecode, starting_bytes)

		#print(f"{parsed_line}")
		#print(f"{unresolved_labels}")

		if not parsed_line:
			continue #skip empty lines and comments

		#print(f"parsed line: {parsed_line}")

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
			symbol_table[label] = starting_bytes - 8

		if line_type == 'directive':
			if in_code_section:
				print(f"Assembler error encountered on line {line_num}!")
				sys.exit(2)
			variables, bytecode, starting_bytes = process_directive(parsed_line, line_num, symbol_table, bytecode, variables, starting_bytes)
			#print(f"variables: {variables}")
			pass
		elif line_type == 'instruction':
			in_code_section = True
			if not first_instruction_checked:
				first_instruction_checked = True
				if ' '.join(components.upper().split()) != 'JMP MAIN':
					print(f"Assembler error encountered on line {line_num}!")
					sys.exit(2)
			bytecode, unresolved_labels, starting_bytes = process_instruction(parsed_line, line_num, symbol_table, bytecode, unresolved_labels, starting_bytes)
			starting_bytes += 8
			#print(f"processed instruction: {parsed_line}")
			instructions_processed += 1
			pass
		elif line_type == 'label':
			pass
		else:
			print(f"Assembler error encountered on line {line_num}!")
			sys.exit(2)

	#need to check if anytghin inside variables set is inside unresolved labels set
	to_remove = [label for label in unresolved_labels if label in variables]
	for label in to_remove:
		del unresolved_labels[label]
	#throw error if any label called in an operand is not a valid function name
	if len(unresolved_labels) > 0:
		print(f"{unresolved_labels}")
		#print(f"{symbol_table}")
		print(f"Assembler error encountered on line {line_num}!")
		sys.exit(2)

	#write the first 4 bytes for prog_mem
	initial_bytes = [starting_bytes - 8 - instructions_processed * 8, 0, 0, 0]

	#write the output file
	output_filename = filename.replace('.asm', '.bin')
	with open(output_filename, 'wb') as bin_file:
		bin_file.write(bytearray(initial_bytes))
		bin_file.write(bytearray(bytecode))

	print(f"Assembly completed successfully. Output file: {output_filename}")
	sys.exit(0)


if __name__ == "__main__":
	if len(sys.argv) != 2 or not sys.argv[1].endswith('.asm'):
		print("USAGE: python3 asm4380.py inputFile.asm")
		sys.exit(1)

	input_filename = sys.argv[1]
	assemble(input_filename)

