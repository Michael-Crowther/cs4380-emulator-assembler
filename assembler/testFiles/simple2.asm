    ONE .int #1
		lower .byt 'a'
		upper .byt 'A' 
		   

		JMP MAIN

MAIN		LDB R3, lower
				TRP #3
		    LDB R3, upper
				TRP #3
				TRP #0
