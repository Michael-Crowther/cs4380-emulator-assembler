; Assembly code for Prime Number Generator

welcomeMsg1 .STR "Welcome to the Prime Number Generator.\n"
welcomeMsg2 .STR "This program searches for and displays the first 20 prime numbers greater than or equal to a user provided lower bound.\n"
welcomeMsg3 .STR "Please enter a lower bound: "
resultMsg .STR "The first 20 prime numbers greater than or equal to "
newLine .STR "\n"
primeList .BTS #80 ; Allocate 80 bytes (20 integers * 4 bytes each)

JMP MAIN

; Function to check if a number is prime
is_prime  MOVI R1, #1       ; Assume R4 is prime
          MOVI R2, #2       ; Start divisor from 2

check_loop  CMP R3, R2, R4    ; Compare divisor with the number
            BGT R3, end_check ; If divisor >= number, end check
            DIV R3, R4, R2    ; Divide number by divisor
            MUL R5, R3, R2    ; Multiply quotient by divisor
            CMP R6, R5, R4    ; Compare product with number
            BNZ R6, next_div  ; If not equal, try next divisor
            MOVI R1, #0       ; Number is not prime
            JMP end_check     ; Exit check

next_div  ADDI R2, R2, #1   ; Increment divisor
          JMP check_loop    ; Continue loop

end_check  RET               ; Return with result

MAIN  MOVI R3, welcomeMsg1
      TRP #5            ; Print part 1 of welcome message
      MOVI R3, welcomeMsg2
      TRP #5            ; Print part 2 of welcome message
      MOVI R3, welcomeMsg3
      TRP #5            ; Print part 3 of welcome message
      TRP #2            ; Read user's lower bound into R3
      MOV R5, R3        ; Move lower bound to R5

      ; Initialize count of primes found
      MOVI R6, #0       ; Prime count
      MOVI R7, #0       ; Index for storing primes

find_primes  MOV R4, R5        ; Copy current number to R4 for prime checking
             CALL is_prime     ; Check if R5 is prime
             BNZ R1, store_prime ; If prime, store it
             ADDI R5, R5, #1   ; Increment number to check
             JMP find_primes   ; Loop to find 20 primes

store_prime  STR R5, primeList + R7 ; Store prime number at primeList + offset R7
             ADDI R7, R7, #4    ; Move to next storage location
             ADDI R6, R6, #1    ; Increment count of primes found
             CMP R8, R6, #20    ; Check if 20 primes are found
             BLT R8, find_primes ; If less than 20, find next prime

            ; Print results
            MOVI R3, resultMsg
            TRP #5             ; Print result message
            MOV R3, R5
            TRP #1             ; Print lower bound
            MOVI R3, newLine
            TRP #5             ; Print newline

            ; Print all primes
            MOVI R7, #0        ; Reset index for output
print_primes  LDR R3, primeList + R7 ; Load prime from storage
              TRP #1             ; Print prime number
              MOVI R3, newLine
              TRP #5             ; Print newline
              ADDI R7, R7, #4    ; Move to next prime in list
              CMP R8, R7, #80    ; Check if all primes printed
              BLT R8, print_primes

              TRP #0             ; Terminate program
