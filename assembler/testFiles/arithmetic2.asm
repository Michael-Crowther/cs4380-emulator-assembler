        JMP MAIN         ; Jump to start of the program

MAIN    MOV R3, #42       ; Load 42 into R3
        MULI R3, R3, #2   ; Multiply R3 by 2, result in R3 (84)
        ADDI R3, R3, #10  ; Add 10 to R3, result in R3 (94)

        ; Convert the number 94 to ASCII and print it
        ; Assuming a simple case where we print each digit individually
        MOV R1, #9        ; Load 9 into R1
        ADDI R1, R1, #48  ; Convert the number 9 to ASCII ('9')
        TRP #3            ; Print '9'

        MOV R1, #4        ; Load 4 into R1
        ADDI R1, R1, #48  ; Convert the number 4 to ASCII ('4')
        TRP #3            ; Print '4'

        ; Print the rest of the message " = 94. Hello World!"
        MOV R3, #32       ; ' '
        TRP #3
        MOV R3, #61       ; '='
        TRP #3
        MOV R3, #32       ; ' '
        TRP #3
        MOV R3, #57       ; '9'
        TRP #3
        MOV R3, #52       ; '4'
        TRP #3
        MOV R3, #46       ; '.'
        TRP #3
        MOV R3, #32       ; ' ' (space)
        TRP #3
        ; "Hello World!" part
        ; ... (Re-use your existing "Hello World!" code here)
        MOV R3, #72       ; 'H'
        TRP #3
        ; (Continue with the rest of the "Hello World!" message)
        
        ; Terminate Program
        TRP #0            ; Halt the program execution

