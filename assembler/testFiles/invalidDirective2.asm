            ;Starting  comment
CHAR1 .BYT 'B'               CHAR2 .BYT 'A'              ; CANT HAVE TWO DIRECTIVES ON ONE LINE

; Code Section starts here
        JMP MAIN          ; Jump to start of the program

; Main program execution starts
MAIN:   MOV R1, '\r'         ; Clear R1 to start with 0
        MOV R2, CHAR1        ; ERROR HERE
        ADD R1, R1, R2     ; Add NUM to R1, result in R1

        MOV R3, CHAR2       ; Move ASCII value of 'A' into R3
        TRP #3             ; Print 'A' on the screen

        MOV R4, '\\'         ; Move 1 into R4
        SUB R2, R2, '\"'     ; Decrement NUM by 1
        TRP #1             ; Assuming a TRP call to print integer in R2

        ; Assuming TRP #0 halts the program
        TRP #0             ; Terminate the program
