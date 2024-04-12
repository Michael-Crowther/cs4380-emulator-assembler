        JMP MAIN             ; Jump to start of the program

MAIN    MOVI R3, #72          ; 'H', for an example output
        MOVI R4, #101         ; 'e', for an example output

        ; TRP #1 - Write int in R3 to stdout (console)
        TRP #1

        ; TRP #2 - Read an integer into R3 from stdin
        TRP #2

        ; TRP #3 - Write char in R3 to stdout
        MOVI R3, #72          ; Load ASCII for 'H'
        TRP #3

        ; TRP #4 - Read a char into R3 from stdin
        TRP #4

        ; Demonstrating use of TRP with various codes for specific actions
        ; TRP #98 - Print all register contents to stdout
        TRP #98

        ; More TRP examples with hypothetical actions
        ; Assuming TRP #0 is a halt/stop routine
        MOVI R3, #0           ; Prepare R3 with a value if needed for halt
        TRP #0                ; Halt the program execution
