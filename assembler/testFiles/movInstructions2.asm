ONE .int #1
TWO .int #2
THREE .int #3

JMP MAIN

MAIN MOV R1, #1
     MOV R2, #2
     MOV R3, #0
     MOV R4, #0

     MOVI R5, #100
     STB R1, ONE
     STB R2, TWO

     LDB R3, THREE
     LDB R4, ONE

     LDR R1, TWO
     STR R2, THREE

     TRP #0
