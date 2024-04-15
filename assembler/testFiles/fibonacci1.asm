; 4380 Assembly File: Fibonacci.asm
; Computes the Fibonacci number at the specified position entered by the user

prompt1 .STR "enter: \n"
prompt2 .STR "term "
prompt3 .STR " in the seq is: "

JMP MAIN

MAIN  MOVI R3, prompt1  ; Load the address of the prompt into R3
      TRP #5            ; Trigger to print the string at the address in R3

      ; Read the user's input term number
      TRP #2            ; Read an integer from the user into R3

      ; Initialize Fibonacci sequence starting values
      MOVI R1, #0        ; R1 = n1 = 0, first Fibonacci number
      MOVI R2, #1        ; R2 = n2 = 1, second Fibonacci number
      MOV R4, R3        ; R4 will hold the input number for comparison

      ; Base case checks
      CMPI R5, R4, #1        ; Compare input with 1
      BNZ R5, NOT_ONE   ; If not 1, skip to NOT_ONE
      MOV R4, R1       ; If it is 1, R4 = n1 which is 0
      JMP PRINT_RESULT  ; Jump to printing result

NOT_ONE  CMPI R5, R4, #2        ; Compare input with 2
         BNZ R5, COMPUTE   ; If not 2, skip to COMPUTE
         MOV R4, R2        ; If it is 2, R4 = n2 which is 1
         JMP PRINT_RESULT  ; Jump to printing result

COMPUTE  MOVI R5, #3        ; R5 = currentTerm, starting from 3
				 MOV R7, R3

LOOP     CMP R6, R5, R7        ; Compare currentTerm with the input term
         BGT R6, DONE      ; If currentTerm >= input, done calculating
         ADD R6, R1, R2    ; R6 = n1 + n2
         MOV R1, R2        ; Move n2 to n1
         MOV R2, R6        ; Move sum to n2
         ADDI R5, R5, #1    ; Increment currentTerm
         JMP LOOP          ; Repeat loop

DONE     MOV R4, R2        ; Final result into R4

PRINT_RESULT  MOVI R3, prompt2  ; Load address of prompt2 into R3
              TRP #5            ; Print "Term "
              MOV R3, R4        ; Load the term number to be printed
              TRP #1            ; Print term number
              MOVI R3, prompt3  ; Load address of prompt3 into R3
              TRP #5            ; Print " in the Fibonacci sequence is: "
              MOV R3, R2        ; Load the Fibonacci result to be printed
              TRP #1            ; Print Fibonacci result
              TRP #0            ; Stop execution
