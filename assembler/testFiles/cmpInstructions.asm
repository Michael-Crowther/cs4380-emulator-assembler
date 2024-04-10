NUM1 .int #10
NUM2 .int #20

    JMP MAIN

MAIN  LDR R1, NUM1         ; Load value of NUM1 into R1
      LDR R2, NUM2         ; Load value of NUM2 into R2

      CMP R3, R1, R2
      MOV R4, R3           ; Move result to R4 for printing
      TRP #3               ; Print comparison result of R1 and R2

      CMPI R5, R1, #15
      MOV R4, R5           ; Move result to R4 for printing
      TRP #3               ; Print comparison result of R1 and 15

      TRP #0
