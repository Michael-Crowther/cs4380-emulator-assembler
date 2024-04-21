message .STR "This is my Hellow World program! \n"

JMP MAIN

MAIN    LDA R3, message
				TRP #5
				MOVI  R3, #72
        TRP #3
        MOVI R3, #101      ; 'e'
        TRP #3
        MOVI R3, #108      ; 'l'
        TRP #3
        MOVI R3, #108      ; 'l'
        TRP #3
        MOVI R3, #111      ; 'o'
        TRP #3
        MOVI R3, #32       ; ' ' (space)
        TRP #3
        MOVI R3, #87       ; 'W'
        TRP #3
        MOVI R3, #111      ; 'o'
        TRP #3
        MOVI R3, #114      ; 'r'
        TRP #3
        MOVI R3, #108      ; 'l'
        TRP #3
        MOVI R3, #100      ; 'd'
        TRP #3
        MOVI R3, #33       ; '!'
        TRP #3
        MOVI R3, #10       ; '\n' (new line)
        TRP #3

        ; Terminate Program
        TRP #0            ; Assuming TRP #0 is the halt instruction
