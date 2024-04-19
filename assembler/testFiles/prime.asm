; Assembly code for Prime Number Generator

welcomeMsg1 .STR "Welcome to the Prime Number Generator.\n"
welcomeMsg2 .STR "This program searches for and displays the first 20 prime numbers greater than or equal to a user provided lower bound.\n"
welcomeMsg3 .STR "Please enter a lower bound: "
resultMsg   .STR "The first 20 prime numbers greater than or equal to "
newLine     .STR "\n"
primeList   .BTS #80 ; Allocate 80 bytes (20 integers * 4 bytes each)

JMP MAIN

; Start of main program execution
MAIN  MOVI R3, welcomeMsg1
    TRP  #5            ; Print part 1 of welcome message
    MOVI R3, welcomeMsg2
    TRP  #5            ; Print part 2 of welcome message
    MOVI R3, welcomeMsg3
    TRP  #5            ; Print part 3 of welcome message
    TRP  #2            ; Read user's lower bound into R3

    MOV  R5, R3        ; Move lower bound to R5 for prime checking
		MOV  R9, R3
    MOVI R6, #0        ; Prime count
    MOVI R7, #0        ; Offset index for storing primes in primeList

find_primes  MOV  R4, R5        ; Copy current number to R4 for prime checking
    CALL is_prime     ; Check if R4 is prime
    BNZ  R1, store_prime ; If prime, store it
    ADDI R5, R5, #1    ; Increment number to check
    CMPI  R8, R6, #20   ; Check if 20 primes are found
    BLT  R8, find_primes ; If less than 20, find next prime

    ; Print results
    MOVI R3, resultMsg
    TRP  #5             ; Print result message
    MOV  R3, R9
    TRP  #1             ; Print lower bound
    MOVI R3, newLine
    TRP  #5             ; Print newline

    ; Print all primes
    MOVI R7, #0        ; Reset index for output

print_primes  LDR  R3, primeList ; Load prime from storage
    TRP  #1            ; Print prime number
    MOVI R3, newLine
    TRP  #5            ; Print newline
    ADDI R7, R7, #4    ; Move to next prime in list
    CMPI  R8, R7, #80   ; Check if all primes printed
    BLT  R8, print_primes

    TRP  #0            ; Terminate program

is_prime  MOVI R1, #1             ; Assume R4 is prime
    MOVI R2, #2             ; Start divisor from 2

check_loop  CMP R3, R2, R4          ; Compare divisor (R2) with the number (R4), result in R3
    BGT R3, end_check       ; If divisor > number, end check (R4 is prime if no divisors found)
    BLT R3, continue_check  ; If divisor < number, continue check

    ; This point reached means R2 == R4
    MOVI R1, #1             ; Confirm number is prime
    RET                     ; Return from the function

continue_check  DIV R6, R4, R2          ; Divide number by divisor, result in R6
    MUL R7, R6, R2          ; Multiply quotient by divisor, result in R7
    CMP R8, R7, R4          ; Compare product with number
    BNZ R8, next_div        ; If not zero, divisor does not divide the number perfectly, try next

    MOVI R1, #0             ; If we reach here, it means R7 == R4, not prime
    RET                     ; Return with R1 as 0 (not prime)

next_div  ADDI R2, R2, #1         ; Increment divisor
    JMP check_loop          ; Continue loop

end_check  RET                     ; Return with R1 still 1 (prime)

store_prime  STR  R4, primeList      ; Store prime number at primeList offset
    ADDI R6, R6, #1         ; Increment count of primes found
    RET                     ; Return to find next prime
