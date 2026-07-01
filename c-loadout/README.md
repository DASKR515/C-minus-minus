#run library c + cmm 
```
gcc -c main.c -o cmain.o
```
```
ghc -no-hs-main main.cmm cmain.o -o app
```
