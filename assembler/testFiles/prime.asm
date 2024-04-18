; Prime Number Generator Assembly Program

welcomeMsg1 .STR "Welcome to the Prime Number Generator.\n"
welcomeMsg2 .STR "This program searches for and displays the first 20 prime numbers greater than or equal to a user provided lower bound.\n"
welcomeMsg3 .STR "Please enter a lower bound: "
resultMsg   .STR "The first 20 prime numbers greater than or equal to "
newLine     .STR "\n"
primeList   .BTS 80          ; Allocate 80 bytes (20 integers * 4 bytes each)

JMP MAIN

is_prime  MOVI R1, #1             ; Assume R4 is prime
          MOVI R2, #2             ; Start divisor from 2

check_loop  CMP R3, R2, R4          ; Compare divisor with the number
            BGT R3, end_check       ; If divisor >= number, end check
            DIV R3, R4, R2          ; Divide number by divisor
            MUL R5, R3, R2          ; Multiply quotient by divisor
            CMP R6, R5, R4          ; Compare product with number
            BNZ R6, next_div        ; If not equal, try next divisor
            MOVI R1, #0             ; Number is not prime
            JMP end_check

next_div    ADDI R2, R2, #1         ; Increment divisor
            JMP check_loop

end_check   RET                     ; Return with result in R1

MAIN        MOVI R3, welcomeMsg1
            TRP #5                  ; Print part 1 of welcome message
            MOVI R3, welcomeMsg2
            TRP #5                  ; Print part 2 of welcome message
            MOVI R3, welcomeMsg3
            TRP #5                  ; Print part 3 of welcome message
            TRP #2                  ; Read user's lower bound into R3
            MOV R5, R3              ; Move lower bound to R5 for processing

            ; Initialize count of primes found and index
            MOVI R6, #0             ; Prime count
            MOVI R9, primeList      ; Initialize pointer to start of primeList

find_primes  MOV R4, R5              ; Copy current number to R4 for prime checking
             CALL is_prime           ; Check if R5 is prime
             BNZ R1, store_prime     ; If prime, store it
             ADDI R5, R5, #1         ; Increment number to check
             CMP R8, R6, #20         ; Check if 20 primes are found
             BLT R8, find_primes     ; If less than 20, find next prime
             JMP print_result

store_prime  STR R5, R9            ; Store prime number at current position in primeList
             ADDI R9, R9, #4         ; Move pointer to next storage location
             ADDI R6, R6, #1         ; Increment count of primes found
             JMP find_primes

print_result  MOVI R3, resultMsg
              TRP #5                  ; Print result message
              MOV R3, R5
              TRP #1                  ; Print the lower bound
              MOVI R3, newLine
              TRP #5                  ; Print newline

             ; Print all primes
              MOVI R9, primeList      ; Reset pointer for output
              MOVI R7, #20            ; Initialize count for printing

print_primes  LDR R3, R9            ; Load prime from storage
              TRP #1                  ; Print prime number
              MOVI R3, newLine
              TRP #5                  ; Print newline
              ADDI R9, R9, #4         ; Move pointer to the next prime
              SUBI R7, R7, #1         ; Decrement the print count
              BNZ R7, print_primes    ; If not zero, continue printing

              TRP #0                  ; Terminate program
