	JMP MAIN

MAIN  movi R1, #1       
      movi R2, #0       

      and R3, R1, R2   ; RD = R3, RS1 = R1, RS2 = R2, DC* = 27

      or R4, R1, R2    ; RD = R4, RS1 = R1, RS2 = R2, DC* = 28

      movi R7, #1 
      movi R0, #0
      TRP #0
