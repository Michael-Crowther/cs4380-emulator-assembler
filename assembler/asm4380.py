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
		parts = code.split(maxplit=1)
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
