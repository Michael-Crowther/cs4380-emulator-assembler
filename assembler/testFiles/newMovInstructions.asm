    ; Define starting memory address for data storage
NUM .int #100     ; Arbitrary address in memory to start data storage

    ; Code Section
    JMP MAIN

    ; Main Program
MAIN  MOVI R4, NUM

    ; Store integer value 12345 into memory at address in R4
    MOVI R1, #12345       ; Move 12345 into R1
    ISTR R1, R4          ; Store the integer from R1 into address pointed by R4

    ; Store ASCII value of 'A' into memory at address in R4 + 4
    MOVI R1, 'A'         ; ASCII value of 'A'
    ADDI R4, R4, #4       ; Move address in R4 to next memory location
    ISTB R1, R4          ; Store the byte from R1 into address pointed by R4

    ; Reset R4 to start address
    MOVI R4, NUM

    ; Load integer value from memory at address in R4 into R2
    ILDR R2, R4          ; Load the integer at address in R4 into R2

    ; Load byte value from memory at address in R4 + 4 into R3
    ADDI R4, R4, #4       ; Move address in R4 to next memory location
    ILDB R3, R4          ; Load the byte at address in R4 into R3

    ; Print loaded integer and character to confirm correct storage and loading
    MOV R3, R2           ; Move loaded integer to R3 for printing
    TRP #1               ; Print integer value
    MOV R3, R2           ; Move loaded character to R3 for printing
    TRP #3               ; Print character

    ; Terminate Program
    TRP #0
