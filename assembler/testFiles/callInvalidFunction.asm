JMP MAIN
MAIN    MOV R3, #72
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
        MOV R3, #10       ; '\n' (new line)
        TRP #3

        ; Terminate Program
        TRP #0            ; Assuming TRP #0 is the halt instruction
JMP SECOND
THIRD: MOV R3, #45
