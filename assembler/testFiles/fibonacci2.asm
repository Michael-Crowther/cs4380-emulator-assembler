; Recursive Fibonacci.asm - Calculates the nth Fibonacci number recursively

prompt1 .STR "Please enter the Fibonacci term you would like computed: \n"
prompt2 .STR "Term "
prompt3 .STR " in the Fibonacci sequence is: "
newline .STR "\n"

JMP MAIN

MAIN             MOVI R3, prompt1
                 TRP #5            ; Print the initial prompt
                 TRP #2            ; Read integer into R3 (Fibonacci term)
                 MOV R5, R3        ; Store user input for later use in printing

                 ; Check for special cases before calling the recursive function
                 CMPI R6, R3, #1
                 BGT R6, CHECK_TWO
                 MOVI R0, #0       ; If the term is 1, result is 0
                 JMP PRINT_RESULT

CHECK_TWO        CMPI R6, R3, #2
                 BGT R6, RECURSIVE_CALL
                 MOVI R0, #1       ; If the term is 2, result is 1
                 JMP PRINT_RESULT

RECURSIVE_CALL   MOV R1, R3        ; Pass the term number in R1
                 CALL FIB          ; Call the recursive Fibonacci function

FIB              CMPI R6, R1, #2
                 BGT R6, RECURSIVE_STEP
                 MOV R0, R1        ; Return 1 for Fib(1) and Fib(2)
                 RET

RECURSIVE_STEP   PSHR R1           ; Save n-1 on stack
								 SUBI R1, R1, #1
                 CALL FIB
                 PSHR R0           ; Save Fib(n-1) result on stack

                 ; Fib(n-2)
                 POPR R1           ; Get n-1 from stack
                 SUBI R1, R1, #2
                 CALL FIB          ; Calculate Fib(n-2)

                 ; Add Fib(n-1) and Fib(n-2)
                 POPR R2           ; Get Fib(n-1) from stack
                 ADD R0, R0, R2    ; R0 = Fib(n-1) + Fib(n-2)

                 RET               ; Return result

PRINT_RESULT     MOV R4, R0        ; Move result to R4 for printing
                 ; Print sequence and result
                 MOVI R3, prompt2
                 TRP #5
                 MOV R3, R5        ; Load the term number to be printed
                 TRP #1
                 MOVI R3, prompt3
                 TRP #5
                 MOV R3, R4        ; Load the Fibonacci result to be printed
                 TRP #1
                 MOVI R3, newline
                 TRP #5            ; Print newline for clean output
                 TRP #0            ; Terminate the program
