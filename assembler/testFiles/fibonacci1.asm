
;fibonacci.asm - Calculates the nth Fibonacci number iteratively

prompt1 .STR "Please enter the Fibonacci term you would like computed: \n"
prompt2 .STR "Term "
prompt3 .STR " in the Fibonacci sequence is: "
newline .STR "\n"

JMP MAIN

MAIN           LDA R3, prompt1
               TRP #5            ; Print the initial prompt
               TRP #2            ; Read integer into R3 (stopValue)

               ; Initialize Fibonacci variables
               MOVI R1, #0       ; R1 = n1 = 0, first Fibonacci number
               MOVI R2, #1       ; R2 = n2 = 1, second Fibonacci number
							 MOV  R5, R3       ;store user input for printing
		           MOVI R4, #0       ; final term
							 MOVI R8, #0

               ; Check if the input is 1
               CMPI R6, R5, #1   ; Compare R5 (user input) with 1
               BGT R6, CHECK_TWO ; If input > 0, check for input being 2
               MOV R4, R1        ; If input is 1, R4 = n1 (0)
               JMP PRINT

CHECK_TWO    CMPI R6, R5, #2   ; Compare input with 2
             BGT R6, COMPUTE   ; If input > 0, go to compute more terms
             MOV R4, R2        ; If input is 2, R4 = n2 (1)
             JMP PRINT         ; Go to print result

COMPUTE      MOVI R7, #3       ; Start term counter at 3

LOOP         ADD R6, R1, R2    ; R6 = n1 + n2 (calculate next Fibonacci number)
             MOV R1, R2        ; Move n2 to n1
             MOV R2, R6        ; Update n2 to the new Fibonacci number
             CMP R8, R7, R5   ; Compare current term index with requested term
             BLT R8, INCREMENT ; If current term index < requested term, continue loop
             MOV R4, R2        ; Set final result to R4
             JMP PRINT         ; Go to print result

INCREMENT   ADDI R7, R7, #1   ; Increment term index
            JMP LOOP          ; Continue the loop

PRINT       LDA R3, prompt2
            TRP #5
            MOV R3, R5        ; Load the temp variable to be printed
            TRP #1
            LDA R3, prompt3
            TRP #5
            MOV R3, R4        ; Load the Fibonacci result to be printed
            TRP #1
            LDA R3, newline
            TRP #5            ; Print newline for clean output
            TRP #0            ; Terminate the program
