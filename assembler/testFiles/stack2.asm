			JMP MAIN

MAIN  movi R1, #10
      movi R2, #20

      pshr R1               

      pshb R2               

      call func             

      movi R3, #30

			TRP #0          

func  movi R5, #40                   

      popb R4               

      popr R5               

      ret                   
