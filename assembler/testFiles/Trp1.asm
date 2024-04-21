JMP MAIN

MAIN TRP #2            ; Execute TRP command to read an integer into R3 from stdin

    ; Echo the integer back to stdout
    MOV R1, R3        ; Move the integer from R3 to R1 to prepare for output
    TRP #1            ; Execute TRP command to write the integer from R1 to stdout

    ; Stop the program execution
    TRP #0            ; Execute TRP command to stop/exit the program

