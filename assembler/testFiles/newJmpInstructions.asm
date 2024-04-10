		NUM1 .int #1
		NUM2 .int #2    

		JMP MAIN

MAIN   MOVI R1, #10
			 JMP LOOP_START

    ; Start of loop
LOOP_START  SUBI R1, R1, NUM1
            BNZ R1, LOOP_START
            MOVI R2, #5
						LDA R4, NEXT_PHASE
						JMR R4


NEXT_PHASE       SUBI R2, R2, NUM1
                 BGT R2, POSITIVE_PATH
                 BLT R2, NEGATIVE_PATH
                 JMP ZERO_PATH

POSITIVE_PATH  MOVI R3, NUM1
							 JMP PRINT

NEGATIVE_PATH  MOVI R3, NUM2
               JMP PRINT

ZERO_PATH     MOVI R3, #0

PRINT  TRP #1
       TRP #0
