; Assembly code for Prime Number Generator using Heap Memory

welcomeMsg1 .STR "Welcome to the Prime Number Generator.\n"
welcomeMsg2 .STR "This program searches for and displays the first 20 prime numbers greater than or equal to a user provided lower bound.\n"
welcomeMsg3 .STR "Please enter a lower bound: "
resultMsg   .STR "The first 20 prime numbers greater than or equal to "
newLine     .STR "\n"

JMP MAIN

MAIN  LDA R3, welcomeMsg1
    TRP  #5            ; Print part 1 of welcome message
    LDA R3, welcomeMsg2
    TRP  #5            ; Print part 2 of welcome message
    LDA R3, welcomeMsg3
    TRP  #5            ; Print part 3 of welcome message
    TRP  #2            ; Read user's lower bound into R3

    MOV  R5, R3        ; Move lower bound to R5 for prime checking
    MOV  R9, R3        ; Save initial lower bound for later printing
    MOVI R11, #0        ; Prime count
    ALCI R7, #80       ; Allocate space and store base address in R7

find_primes  MOV  R4, R5        ; Copy current number to R4 for prime checking
    CALL is_prime     ; Check if R4 is prime
    BNZ  R1, store_prime ; If prime, store it
    ADDI R5, R5, #1    ; Increment number to check
    CMPI R8, R11, #20   ; Check if 20 primes are found
    BLT  R8, find_primes ; If less than 20, find next prime
    JMP print_results

store_prime  ISTR R4, R7         ; Store prime number at address in R7
    ADDI R11, R11, #1     ; Increment count of primes found
    ADDI R7, R7, #4     ; Move to next storage location
    ADDI R5, R5, #1     ; Increment number to check even after storing a prime
    JMP find_primes     ; Jump back to find more primes

print_results  LDA R3, resultMsg
    TRP  #5             ; Print result message
    MOV  R3, R9
    TRP  #1             ; Print lower bound
    LDA R3, newLine
    TRP  #5             ; Print newline

    MOV R7, R7, #0    ; Reset R7 to start of prime numbers

print_primes  ILDR R3, R7         ; Load prime from storage at address in R7
    TRP  #1             ; Print prime number
    LDA R3, newLine
    TRP  #5             ; Print newline
    ADDI R7, R7, #4     ; Increment address to next prime
    CMPI R8, R11, #20    ; Check if all primes printed
    BLT  R8, print_primes

    TRP  #0             ; Terminate program

is_prime  MOVI R1, #1         ; Assume R4 is prime
    MOVI R2, #2         ; Start divisor from 2

check_loop  CMP  R3, R2, R4     ; Compare divisor (R2) with the number (R4), result in R3
    BGT  R3, end_check  ; If divisor > number, end check (R4 is prime if no divisors found)
    BLT  R3, continue_check  ; If divisor < number, continue check

    MOVI R1, #1         ; Confirm number is prime
    RET                 ; Return from the function

continue_check  DIV  R6, R4, R2     ; Divide number by divisor, result in R6
    MUL  R7, R6, R2     ; Multiply quotient by divisor, result in R7
    CMP  R8, R7, R4     ; Compare product with number
    BNZ  R8, next_div   ; If not zero, divisor does not divide the number perfectly, try next

    MOVI R1, #0         ; If we reach here, it means R7 == R4, not prime
    RET                 ; Return with R1 as 0 (not prime)

next_div  ADDI R2, R2, #1     ; Increment divisor
    JMP  check_loop     ; Continue loop

end_check  RET                 ; Return with R1 still 1 (prime)

