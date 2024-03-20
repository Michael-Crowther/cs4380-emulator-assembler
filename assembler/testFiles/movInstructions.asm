        JMP MAIN             ; Jump to start of the program

MAIN    MOV R1, R2            ; Move the content of R2 into R1
        MOV R3, R4            ; Move the content of R4 into R3
        MOV R5, R6            ; Move the content of R6 into R5

        ; Demonstrating MOVI instructions
        MOVI R1, #10          ; Move immediate value 10 into R1
        MOVI R2, #20          ; Move immediate value 20 into R2
        MOVI R3, #30          ; Move immediate value 30 into R3
        MOVI R4, #40          ; Move immediate value 40 into R4
        MOVI R5, #50          ; Move immediate value 50 into R5
        MOVI R6, #60          ; Move immediate value 60 into R6
        MOVI R7, #70          ; Move immediate value 70 into R7
        MOVI R8, #80          ; Move immediate value 80 into R8

        ; Example usage of MOV with register and MOVI for setting up an operation
        MOV R1, R2            ; Assume R2 holds a value, move it into R1
        MOVI R3, #5           ; Set immediate value 5 into R3
        ADD R1, R1, R3        ; Example: Add R3 to R1, result back in R1

        ; Additional operations or end of program
        ; ...
        
        ; Terminate Program
        TRP #0                ; Halt the program execution

