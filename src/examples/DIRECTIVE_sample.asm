.ORIG x3010 ; where to place program
HELLO1 .STRINGZ "Hello, World!" ; init a seq of n+a mem locations where a = 1 if n is odd and a = 2 if n is even
HELLO2 .STRINGZ "Hello, LC3!"
HELLO3 .STRINGZ "Hello, World!"
ASCII .FILL x0030 ; set aside next location and init it with the value of operand
PTR .FILL x4000
VAR1 .BLKW 1 ; set aside number of sequential mem locations(BLocK of Words)
.END ; tells where program ends. any chars after it will not be utilized by assembler
