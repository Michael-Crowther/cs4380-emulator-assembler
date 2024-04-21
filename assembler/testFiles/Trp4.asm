JMP MAIN
MAIN  TRP #4           ; Execute TRP command to read a character into R3

    ; Echo the character back to stdout
    MOV R1, R3       ; Move the character from R3 to R1, preparing for output
    TRP #3           ; Execute TRP command to write the character from R1 to stdout

    ; Stop the program execution
    TRP #0           ; Execute TRP command to stop/exit the program
