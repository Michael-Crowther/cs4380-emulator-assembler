		 JMP MAIN

MAIN  movi SP, #10

      movi R1, #20   ; RS = R1

      pshr R1               ; Opcode 35, SP = SP - 4; [SP] = R1

      movi R2, #30         ; RS = R2, only the LSB will be used

      pshb R2               ; Opcode 36, SP = SP - 1; [SP] = LSB of R2

      TRP #0
