            ;Starting  comment
NUM .int #10               ; Initialize an integer variable with value 10
CHAR .byt 'A'              ; Initialize a character variable with 'A'

; Code Section starts here
        JMP MAIN          ; Jump to start of the program

; Main program execution starts
MAIN    MOVI R1, '\r'         
        LDR R2, NUM        
        ADD R1, R1, R2     

        LDR R3, CHAR       
        TRP #3             

        MOVI R4, '\\'         
        SUBI R2, R2, '\"'     
        TRP #1             

        TRP #0       
