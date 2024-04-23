; Assembly code for Recursive Fibonacci with Memoization on Heap

prompt1 .STR "Please enter the Fibonacci term you would like computed: \n"
prompt2 .STR "Term "
prompt3 .STR " in the Fibonacci sequence is: "
newline .STR "\n"

JMP MAIN

MAIN             LDA R3, prompt1
                 TRP #5           
                 TRP #2           
                 MOV R5, R3        ; Store user input for later use in printing

                 ; Allocate heap space for memoization
                 ALCI R11, #104     ; Allocate space for storing results of terms 0 to 100 on the heap
              
                 MOV R10, R11       ; Copy HP to R10 for initialization start point
                 SUBI R10, R10, #104 ; Move back R10 to the start of allocated block
                 MOVI R9, #26      ; Number of entries to initialize

                ; Check for special cases before calling the recursive function
                 CMPI R6, R3, #1
                 BGT R6, CHECK_TWO ; If R3 > 1, check for term 2
                 MOVI R0, #0       ; If the term is 1, result is 0
                 JMP PRINT_RESULT

CHECK_TWO        CMPI R6, R3, #2
                 BGT R6, RECURSIVE_CALL ; If R3 > 2, perform recursive call
                 MOVI R0, #1       ; If the term is 2, result is 1
                 JMP PRINT_RESULT

RECURSIVE_CALL   MOV R1, R3        ; Pass the term number in R1
                 CALL FIB          ; Call the recursive Fibonacci function
                 JMP PRINT_RESULT

FIB              CMPI R6, R1, #2
                 BGT R6, RECURSIVE_STEP     ; If R1 > 2, proceed with recursive steps
                 CMPI R6, R1, #1
                 BGT R6, RETURN_ONE         ; If R1 == 2, return 1
                 MOVI R0, #0                ; Else it's Fib(1), return 0
                 RET

RETURN_ONE       MOVI R0, #1
                 RET

RECURSIVE_STEP   MOV R2, R1
                 ADD R2, R2, R2             ; R2 = R1 * 2
                 ADD R2, R2, R2             ; R2 = R2 * 2 (now R2 = R1 * 4)
                 ADD R2, R7, R2             ; R2 = HP - offset for the term (Address calculation)
                 ILDR R0, R2                ; Load previously computed result
                 CMP R6, R0, #0
                 BNZ R6, RETURN_MEMOIZED    ; If not 0, skip computation

                 ; Only compute if R1 > 2
                 SUBI R1, R1, #1            ; Calculate Fib(n-1)
                 CALL FIB
                 PSHR R0                    ; Save result of Fib(n-1) on stack

                 MOV R1, R3                 ; Restore original value from R3 (safekeeping R1's value)
                 SUBI R1, R1, #2            ; Calculate Fib(n-2)
                 CALL FIB

                 POPR R2                    ; Get Fib(n-1) from stack
                 ADD R0, R0, R2             ; Sum Fib(n-1) and Fib(n-2) for final result
                 ISTR R0, R2                ; Store result in heap at calculated address
                 RET

RETURN_MEMOIZED  RET

PRINT_RESULT     MOV R4, R0                 ; Move result to R4 for printing
                 LDA R3, prompt2
                 TRP #5
                 MOV R3, R5                 
                 TRP #1
                 LDA R3, prompt3
                 TRP #5
                 MOV R3, R4                 
                 TRP #1
                 LDA R3, newline
                 TRP #5                     
                 TRP #0                     
