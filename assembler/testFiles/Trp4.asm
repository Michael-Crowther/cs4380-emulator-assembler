JMP MAIN
MAIN  TRP #4        

    MOV R1, R3       
    TRP #3           ; Execute TRP command to write the character from R1 to stdout

    TRP #0           ; Execute TRP command to stop/exit the program
