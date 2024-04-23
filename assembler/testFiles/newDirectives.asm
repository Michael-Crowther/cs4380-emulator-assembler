string1 .STR "Hello, world!"
buffer .BTS #10
num_value .STR #8   

        JMP MAIN

MAIN    movi R1, #10
        movi R2, #20

        pshr R1
        pshb R2

        call func

        movi R3, string1 

        TRP #5            

        movi R3, buffer   
        TRP #6            

        movi R3, num_value
        TRP #5            

        TRP #0            

func    movi R5, #40
        popb R4
        popr R5
        ret

