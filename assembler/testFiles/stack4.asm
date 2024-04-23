      ONE .int #10
			TWO .int #20

			JMP MAIN

func  movi R5, #40

      popb R4

      popr R5

      ret

MAIN  movi R1, ONE
      movi R2, TWO

      pshr R1

      pshb R2

      call func

      movi R3, #30

      TRP #0
