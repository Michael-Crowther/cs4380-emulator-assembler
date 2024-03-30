NUMBER .int #34
BYTE .BYT 'a'

LABEL1 .byt 'b'
LABEL2 .byt 'c'
LABEL3 .byt 'd'

JMP MAIN

MAIN MOV R1, R2
     MOVI R3, #100
     LDA R4, NUMBER
     STR R3, NUMBER
     LDR R5, NUMBER
     STB R3, BYTE
     LDB R6, BYTE

; Arithmetic Operations
ADD R7, R1, R2
ADDI R8, R1, #10
SUB R9, R1, R2
SUBI R10, R1, #10
MUL R11, R1, R2
MULI R12, R1, #2
DIV R13, R1, R2
SDIV R14, R1, R2
DIVI R15, R1, #2

; New Store/Load Instructions
ISTR R3, R2    ; Store integer in R3 at address in R2
ILDR R4, R2    ; Load integer at address in R2 into R4
ISTB R3, R2    ; Store byte in R3 at address in R2
ILDB R4, R2    ; Load byte at address in R2 into R4

; Jump Instructions
JMR R1         ; Update PC to value in R1
BNZ R1, LABEL1 ; Update PC to LABEL1 if R1 != 0
BGT R1, LABEL2 ; Update PC to LABEL2 if R1 > 0
BLT R1, LABEL3 ; Update PC to LABEL3 if R1 < 0

; Compare Instructions
CMP R1, R2, R3  ; Set R1 = 0 if R2 == R3, 1 if R2 > R3, -1 if R2 < R3
CMPI R1, R2, #5 ; Set R1 = 0 if R2 == #5, 1 if R2 > #5, -1 if R2 < #5

; Traps/Interrupts
TRP #1
TRP #2
TRP #3
TRP #4
TRP #98

; End of Program
TRP #0
