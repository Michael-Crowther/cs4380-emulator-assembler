; Assembly code to calculate Fibonacci number recursively

prompt1 .STR "Please enter the Fibonacci term you would like computed: \n"
prompt2 .STR "The Fibonacci number is: "

JMP MAIN

fib  CMPI R2, R1, #1   ; Compare n with 1
     BLT R2, RECURSE   ; If n < 1, go to recursive computation
     BNZ R2, BASE_CASE ; If n == 1, go to base case

BASE_CASE  RET               ; If n is 0 or 1, return n as it is the result


    ; Recursive case: fib(n-1) + fib(n-2)
    ; Calculate fib(n-1)
RECURSE    PSHR R1           ; Save current n
           SUBI R1, R1, #1   ; n = n - 1
           CALL fib          ; Calculate fib(n-1)
           MOV R3, R1        ; Store result of fib(n-1) in R3

           ; Calculate fib(n-2)
           POPR R1           ; Restore original n
           SUBI R1, R1, #2   ; n = n - 2
           CALL fib          ; Calculate fib(n-2)

          ; Add results of fib(n-1) and fib(n-2)
          ADD R1, R3, R1    ; R1 = fib(n-1) + fib(n-2)
          RET

MAIN  MOVI R3, prompt1
      TRP #5
      TRP #2            ; Read integer into R3
      MOV R1, R3        ; Move read value to R1

      CALL fib          ; Calculate fibonacci number

      MOVI R3, prompt2
      TRP #5
      MOV R3, R1
      TRP #1            ; Print Fibonacci number
      TRP #0            ; Terminate program

