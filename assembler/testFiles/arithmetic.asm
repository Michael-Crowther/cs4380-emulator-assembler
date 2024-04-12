        JMP MAIN         ; Jump to start of the program

MAIN   MOV R3, #42       ; Load 42 into R3
        SUBI R3, R3, #42  ; Subtract 42 from R3, result in R3 (0)
        ADDI R3, R3, #48  ; Convert the number 0 to ASCII ('0')
        TRP #3            ; Print '0'

        ; Print the rest of the message " = 0. Hello World!"
        MOV R3, #45       ; '-'
        TRP #3
        MOV R3, #52       ; '4'
        TRP #3
        MOV R3, #50       ; '2'
        TRP #3
        MOV R3, #32       ; ' '
        TRP #3
        MOV R3, #61       ; '='
        TRP #3
        MOV R3, #32       ; ' '
        TRP #3
        MOV R3, #48       ; '0'
        TRP #3
        MOV R3, #46       ; '.'
        TRP #3
        MOV R3, #32       ; ' ' (space)
        TRP #3
        ; "Hello World!" part (re-use your existing code)
        MOV R3, #72       ; 'H'
        TRP #3            
        MOV R3, #101      ; 'e'
        TRP #3            
        MOV R3, #108      ; 'l'
        TRP #3            
        MOV R3, #108      ; 'l'
        TRP #3            
        MOV R3, #111      ; 'o'
        TRP #3            
        MOV R3, #32       ; ' ' (space)
        TRP #3            
        MOV R3, #87       ; 'W'
        TRP #3            
        MOV R3, #111      ; 'o'
        TRP #3            
        MOV R3, #114      ; 'r'
        TRP #3            
        MOV R3, #108      ; 'l'
        TRP #3            
        MOV R3, #100      ; 'd'
        TRP #3            
        MOV R3, #33       ; '!'
        TRP #3            
        MOV R3, #32       ; ' '
        TRP #3
        ; Print the newline character
        MOV R3, #10       ; '\n'
        TRP #3

        ; Terminate Program
        TRP #0            ; Halt the program execution

