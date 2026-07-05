#run library c + cmm 
```
gcc -c main.c -o cmain.o
```
to this build app
```
./gmm -o app.exe main.cmm app.o 
```
or 
```
ghc -no-hs-main main.cmm cmain.o -o app
```
