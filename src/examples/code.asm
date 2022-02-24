           .ORIG   x3000
            BR     MAIN              ; Branch to main 

;; Data declarations
PARAM      .FILL   x789A             ; Input parameter

;; Program entry point
MAIN        LD    R6,STACK           ; Initialize stack pointer
            LD    R0,PARAM           ; Load parameter
            ADD   R6,R6,#-1          ; Push parameter
            STR   R0,R6,#0           ; 
            JSR   PRINT              ; Call print
            ADD   R6,R6,#1           ; Cleanup stack
            HALT                     ; Program complete

;; Print a positive hexadecimal number
PRINT       ;; stack entry
            ADD   R6,R6,#-1          ; Push return address
            STR   R7,R6,#0           ;
            ADD   R6,R6,#-1          ; Push frame pointer
            STR   R5,R6,#0           ;
            ADD   R5,R6,#0           ; Setup frame pointer

            ;; function body
BODY        LDR   R0,R5,#2           ; Load parameter
            ADD   R6,R6,#-1          ; Push parameter
            STR   R0,R6,#0           ;
            LD    R1,DENOM           ; Load denominator
            ADD   R6,R6,#-1          ; Push denominator
            STR   R1,R6,#0           ;
            JSR   DIVIDE             ; Call divide
            LDR   R2,R6,#0           ; Load result    
            ADD   R6,R6,#3           ; Cleanup stack
           
            AND   R2,R2,x000f        ; Mask result 
            ADD   R6,R6,#-1          ; Push result
            STR   R2,R6,#0           ;
            JSR   OUTPUT             ; Call output
            ADD   R6,R6,#1           ; Cleanup stack

            LD    R1,DENOM           ; Load denominator
            ADD   R6,R6,#-1          ; Push denominator
            STR   R1,R6,#0           ; 
            LD    R0, SIXTEEN        ; Load constant
            ADD   R6,R6,#-1          ; Push constant
            STR   R0,R6,#0           ; 
            JSR   DIVIDE             ; Call divide
            LDR   R2,R6,#0           ; Load result
            ST    R2,DENOM           ; Update denominator    
            ADD   R6,R6,#3           ; Cleanup stack

            ADD   R2,R2,#0           ; Check denominator
            BRp   BODY               ; Loop if positive 

            ;; stack exit
DONE        LDR   R5,R6,#0           ; Pop frame pointer
            ADD   R6,R6,#1           ;
            LDR   R7,R6,#0           ; Pop return address
            ADD   R6,R6,#1           ;
            RET                      ; Return

;; Output a single hexadecimal digit
OUTPUT      ;; stack entry
            ADD   R6,R6,#-1          ; Push return address
            STR   R7,R6,#0           ; 
            ADD   R6,R6,#-1          ; Push frame pointer 
            STR   R5,R6,#0           ; 
            ADD   R5,R6,#0           ; Setup frame pointer

            ;; function body
            LDR   R1,R5,#2           ; Load parameter
            ADD   R0,R1,#-9          ; Check magnitude
            BRp   ISLETTER           ; Display letter
ISDIGIT     LD    R0,DIGIT           ; Load offset
            ADD   R0,R1,R0           ; Digit conversion
            TRAP  x21                ; Output digit
            BR    CONTINUE           ; Skip letter
ISLETTER    LD    R0,LETTER          ; Load offset    
            ADD   R0,R1,R0           ; Letter conversion
            TRAP  x21                ; Output letter

CONTINUE    ;; stack exit
            LDR   R5,R6,#0           ; Pop frame pointer
            ADD   R6,R6,#1           ; 
            LDR   R7,R6,#0           ; Pop return address
            ADD   R6,R6,#1           ;
            RET                      ; Return

;; Divide a number by power of 2
DIVIDE      ;; stack entry
            ADD   R6,R6,#-1          ; Allocate return value
            ADD   R6,R6,#-1          ; Push return address
            STR   R7,R6,#0           ;
            ADD   R6,R6,#-1          ; Push frame pointer
            STR   R5,R6,#0           ;
            ADD   R5,R6,#0           ; Setup frame pointer

            ;; function body
            LDR   R0,R5,#4           ; Load numerator
            LDR   R1,R5,#3           ; Load denominator
            NOT   R1,R1              ; 2's complement (negate)
            ADD   R1,R1,#1           ; 2's complement (increment)
            AND   R2,R2,#0           ; Initialize result
LOOP        ADD   R0,R0,R1           ; Subtract denominator
            BRn   EXIT               ; Loop complete?
            ADD   R2,R2,#1           ; Increment result
            BR    LOOP               ; Continue processing

EXIT        ;; stack exit
            STR   R2,R5,#2           ; Store return value
            LDR   R5,R6,#0           ; Pop frame pointer
            ADD   R6,R6,#1           ; 
            LDR   R7,R6,#0           ; Pop return address
            ADD   R6,R6,#1           ;
            RET                      ; Return

;; Constants
SIXTEEN   .FILL   x0010
DENOM     .FILL   x1000
DIGIT     .FILL   x0030
LETTER    .FILL   x0057
STACK     .FILL   x4000

          .END

