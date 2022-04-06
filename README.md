# LC3b assembler

This is LC3b assembler implemented in cpp.

It is based on material provided for course of [EE 460N](http://users.ece.utexas.edu/~patt/18f.460N/handouts.html).

## Download
Compiled assembler can be found at [releases](https://github.com/tejasMadrewar/LC3b_Assembler/releases).

## Compile
makefile is using clang compiler by default.
To use gcc compiler change "clange++" to "g++" in src/makefile
```
cd src
make
```
It will generate src\main file which is LC3b assembler.

## Use
```
main file.asm
```
It will generate the hex, symbol table file in asm file folder.
