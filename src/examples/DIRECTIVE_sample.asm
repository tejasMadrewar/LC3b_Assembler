.ORIG x3010 ; where to place program
AND R1, R2, #3
AND R1, R2, #3
HELLO1 .STRINGZ "Hello, World!" ; init a seq of n+a mem locations where a = 1 if n is odd and a = 2 if n is even
AND R1, R2, #3
AND R1, R2, #3
HELLO2 .STRINGZ "Hello, TEST!"
AND R1, R2, #3
AND R1, R2, #3
HELLO3 .STRINGZ "Hello, World!"
AND R1, R2, #3
AND R1, R2, #3
ASCII .FILL x0030 ; set aside next location and init it with the value of operand
AND R1, R2, #3
AND R1, R2, #3
PTR .FILL x4000
AND R1, R2, #3
AND R1, R2, #3
VAR1 .BLKW 1 ; set aside number of sequential mem locations(BLocK of Words)
AND R1, R2, #3
AND R1, R2, #3
.END ; tells where program ends. any chars after it will not be utilized by assembler
