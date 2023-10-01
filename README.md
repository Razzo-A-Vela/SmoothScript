# SmoothScript
### *The worst language in the world*

<br>

## This compiler only compiles for windows
### You need to have mingw64 installed

<br>

## Features :
  - variables
  - if, for and while statments (with break and continue)
  - math (+, -, *, /, ()) with variables (++, --)
  - boolean operators (==, !=, <=)
  - exit with a code
  - scopes
  - int and char literals
  - put characters in the console
  - get characters from the console
  - functions

<br>

## See <a href="Prova.smt">Prova.smt</a> for syntax

## Usage :
#### <code>smooth \<filename\> [-asm [-noLink]]</code>
  - filename: the name of the .smt file
  - -asm: generate assembly
  - -noLink: do not assemble and link

<br>

## To compile the code :  
  - default: <code>g++ -std=c++20 smooth.cpp -o ../smooth.exe</code>
  - debug: <code>g++ -std=c++20 -g3 smooth.cpp -o ../smooth.exe</code>

<br>

**SmoothScript is still in development**
