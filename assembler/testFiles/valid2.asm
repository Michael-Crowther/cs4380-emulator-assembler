            ;Starting  comment
NUM .INT #10               ; Initialize an integer variable with value 10
CHAR .BYT 'A'              ; Initialize a character variable with 'A'

; Code Section starts here
        JMP MAIN          ; Jump to start of the program

; Main program execution starts
MAIN:   MOV R1, #0         ; Clear R1 to start with 0
        MOV R2, NUM        ; ERROR HERE
        ADD R1, R1, R2     ; Add NUM to R1, result in R1

        MOV R3, CHAR       ; Move ASCII value of 'A' into R3
        TRP #3             ; Print 'A' on the screen

        MOV R4, #1         ; Move 1 into R4
        SUB R2, R2, R4     ; Decrement NUM by 1
        TRP #1             ; Assuming a TRP call to print integer in R2

        ; Assuming TRP #0 halts the program
        TRP #0             ; Terminate the program

