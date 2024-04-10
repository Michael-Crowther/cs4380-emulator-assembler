TEN .int #10
TWENTY .int #20        

				JMP MAIN

MAIN    MOVI R1, TEN        ; Initialize R1 to 10
        MOVI R2, TWENTY        ; Initialize R2 to 20
        MOV R7, R1          ; Move the value in R1 to R7
        ADD R3, R7, R2      ; Add R7 and R2, store the result in R3

        ; Print the result of R7 + R2, which is R1 + R2 stored in R3
        TRP #1              ; Print the integer value in R3 to the screen

        MOVI R3, #65        ; Initialize R3 to 65 ('A' in ASCII)
        TRP #3              ; Print the character ('A') stored in R3 to the screen

        ; Print a newline character for neatness
        MOVI R3, #10        ; ASCII value for newline
        TRP #3              ; Print newline

        ; Demonstrate the use of MOV with a register that has a calculated value
        MOV R4, R3          ; Move the newline character ASCII value from R3 to R4
        ADDI R4, R4, #42    ; Add 42 to the value in R4, just as an example operation
        MOV R3, R4          ; Move the modified value back to R3 for printing
        TRP #1              ; Print the modified value in R3, demonstrating MOV effectiveness

        TRP #0              ; Terminate the program

