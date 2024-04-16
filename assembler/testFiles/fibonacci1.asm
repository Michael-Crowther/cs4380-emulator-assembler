; Fibonacci.asm - Calculates the nth Fibonacci number iteratively
prompt1 .STR "Please enter the Fibonacci term you would like computed: \n"
prompt2 .STR "Term "
prompt3 .STR " in the Fibonacci sequence is: "
JMP MAIN

MAIN  MOVI R3, prompt1
      TRP #5
      TRP #2            ; Read integer into R3

    ; Initialize Fibonacci variables
    MOVI R1, #0       ; R1 = n1 = 0, first Fibonacci number
    MOVI R2, #1       ; R2 = n2 = 1, second Fibonacci number
    MOV R4, R3        ; R4 will hold the final result

    ; Check if the input is 1
    CMPI R5, R3, #1   ; Compare input with 1
    BNZ R5, TWO       ; If not 1, jump to TWO

ONE  MOV R4, R1        ; If R3 is 1, R4 = n1
     JMP PRINT

TWO  CMPI R5, R3, #2   ; Compare input with 2
     BNZ R5, ELSE      ; If not 2, jump to ELSE
     MOV R4, R2        ; If R3 is 2, R4 = n2
     JMP PRINT

ELSE  MOVI R5, #2       ; R5 = currentTerm
      MOVI R6, #0       ; R6 = temp

LOOP1  CMP R7, R5, R3    ; Compare currentTerm with stopValue
       BGT R7, DONE1     ; If currentTerm >= stopValue, exit loop

CONT  ADD R6, R1, R2    ; temp = n1 + n2
      MOV R1, R2        ; n1 = n2
      MOV R2, R6        ; n2 = temp
      ADDI R5, R5, #1   ; Increment currentTerm
      JMP LOOP1         ; Jump back to start of loop

DONE1  MOV R4, R2        ; Final result into R4

PRINT  MOVI R3, prompt2
       TRP #5
       MOV R3, R4
       TRP #1
       MOVI R3, prompt3
       TRP #5
       MOV R3, R2
       TRP #1
       TRP #0             ; Terminate the program
