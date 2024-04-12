            ; Starting comment
NUM         .int #10               ; Initialize an integer variable with value 10
CHAR_ARRAY  .BYT  'H'
            .BYT  'e'
            .BYT  'l'
            .BYT  'l'
            .BYT  'o'

; Code Section starts here
            JMP MAIN               ; Jump to start of the program

; Main program execution starts
MAIN        LDB R3, CHAR_ARRAY + 0 ; Load 'H' into R3
            TRP #3                 ; Print 'H'
            
            LDB R3, CHAR_ARRAY + 1 ; Load 'e' into R3
            TRP #3                 ; Print 'e'
            
            LDB R3, CHAR_ARRAY + 2 ; Load 'l' into R3
            TRP #3                 ; Print 'l'
            
            LDB R3, CHAR_ARRAY + 3 ; Load 'l' again into R3
            TRP #3                 ; Print 'l'
            
            LDB R3, CHAR_ARRAY + 4 ; Load 'o' into R3
            TRP #3                 ; Print 'o'

            ; Assuming TRP #0 halts the program
            TRP #0                 ; Terminate the program
