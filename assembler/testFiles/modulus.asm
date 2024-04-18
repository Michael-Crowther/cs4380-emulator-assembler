; Assembly code to calculate the modulus of two integers

prompt1 .STR "Please enter an integer dividend: \n"
prompt2 .STR "Please enter an integer divisor: \n"
prompt3 .STR " divided by "
prompt4 .STR " results in a remainder of: "
newline .STR "\n"

JMP MAIN

mod  PSHR R8          ; Save callee-saved register
     PSHR R9          ; Save another callee-saved register
     MOV R8, R0       ; Move dividend to R8
     MOV R9, R1       ; Move divisor to R9
     DIV R3, R8, R9   ; R3 = quotient = dividend / divisor
     MUL R4, R3, R9   ; R4 = quotient * divisor
     SUB R2, R8, R4   ; R2 = remainder = dividend - (quotient * divisor)
     POPR R9          ; Restore callee-saved register
     POPR R8          ; Restore another callee-saved register
     RET              ; Return to caller

MAIN  MOVI R3, prompt1
      TRP #5
      TRP #2
      MOV R0, R3       ; Store dividend in R0

      ; Prompt for divisor
      MOVI R3, prompt2
      TRP #5
      TRP #2
      MOV R1, R3       ; Store divisor in R1

      ; Call modulus function
      CALL mod         ; Call mod function

      ; Print the result
      ;MOVI R3, prompt1
      ;TRP #5
      MOV R3, R0
      TRP #1           ; Print dividend
      MOVI R3, prompt3
      TRP #5
      MOV R3, R1
      TRP #1           ; Print divisor
      MOVI R3, prompt4
      TRP #5
      MOV R3, R2
      TRP #1           ; Print remainder
			MOVI R3, newline
      TRP #5            ; Print newline for clean output
      TRP #0           ; Terminate program

