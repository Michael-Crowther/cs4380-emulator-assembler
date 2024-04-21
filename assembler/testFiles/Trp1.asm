JMP MAIN

MAIN TRP #2            ; Execute TRP command to read an integer into R3 from stdin

    ; Echo the integer back to stdout
    MOV R1, R3        
    TRP #1            ; Execute TRP command to write the integer from R1 to stdout

    TRP #0            

