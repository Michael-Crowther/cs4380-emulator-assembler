; Assembly code for Prime Number Generator using Heap Memory

welcomeMsg1 .STR "Welcome to the Prime Number Generator.\n"
welcomeMsg2 .STR "This program searches for and displays the first 20 prime numbers greater than or equal to a user provided lower bound.\n"
welcomeMsg3 .STR "Please enter a lower bound: "
resultMsg   .STR "The first 20 prime numbers greater than or equal to "
newLine     .STR "\n"
primeList   .BTS #80

JMP MAIN

MAIN            LDA R3, welcomeMsg1
                TRP #5            ; Print part 1 of welcome message
                LDA R3, welcomeMsg2
                TRP #5            ; Print part 2 of welcome message
                LDA R3, welcomeMsg3
                TRP #5            ; Print part 3 of welcome message
                TRP #2            ; Read user's lower bound into R3

                MOV R5, R3        ; Move lower bound to R5 for prime checking
                MOV R9, R3        ; Save initial lower bound for later printing
                MOVI R11, #0      ; Prime count
                LDA R7, primeList      ; Allocate space and store base address in R7

find_primes     MOV R4, R5        ; Copy current number to R4 for prime checking
                CALL is_prime     ; Check if R4 is prime
                BNZ R1, store_prime ; If prime, store it
                ADDI R5, R5, #1   ; Increment number to check
                CMPI R8, R11, #20 ; Check if 20 primes are found
                BLT R8, find_primes ; If less than 20, find next prime
                JMP print_results

store_prime     ISTR R4, R7       ; Store prime number at address in R7
                ADDI R11, R11, #1 ; Increment count of primes found
                ADDI R7, R7, #4   ; Move to next storage location
                ADDI R5, R5, #1   ; Increment number to check even after storing a prime
                JMP find_primes   ; Jump back to find more primes

print_results   LDA R3, resultMsg
                TRP #5            ; Print result message
                MOV R3, R9
                TRP #1            ; Print lower bound
                LDA R3, newLine
                TRP #5            ; Print newline
                LDA R7, primeList   ; Reset R7 to start of prime numbers

print_primes    ILDR R3, R7       ; Load prime from storage at address in R7
                TRP #1            ; Print prime number
                LDA R3, newLine
                TRP #5            ; Print newline
                ADDI R7, R7, #4   ; Increment address to next prime
                CMPI R8, R11, #20 ; Check if all primes printed
                BLT R8, print_primes

                TRP #0            ; Terminate program

is_prime        PSHR R11
                MOVI R1, #1       ; Assume R4 is prime  
                MOVI R2, #2       ; Start divisor from 2

check_loop      CMP R3, R2, R4    ; Compare current divisor (R2) with the number (R4)
                BGT R3, end_check ; If divisor > number, R4 is prime (no divisors found)
                DIV R6, R4, R2    ; Divide number by divisor
                MUL R7, R6, R2    ; Multiply quotient by divisor
                CMP R8, R4, R7    ; Compare original number with the result
                BNZ R8, next_div  ; If not zero, no exact division, try next divisor

                MOVI R1, #0       ; Exact division found, number is not prime
                JMP end_check     ; Exit checking

                MOVI R1, #0       ; Set not prime
                JMP end_check     ; Jump to end, bypassing additional checks

next_div        ADDI R2, R2, #1   ; Increment divisor
                JMP check_loop    ; Continue loop

end_check       POPR R11          ; Restore R11 from stack
                RET               ; Return with R1 indicating prime status

