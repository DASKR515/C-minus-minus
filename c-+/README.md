#run library c + cmm 
```
gcc -c mylib.c -o mylib.o
```
```
ghc -no-hs-main main.cmm mylib.o -o app
```
