lua + cmm via C lib
this test on windows 

```
gcc -c main.c -o app.o -I. 
```
to 
```
this on linux
gmm -o app main.cmm app.o liblua55.so
```
or
```
ghc -no-hs-main main.cmm .\app.o .\liblua54.a -o app.exe
```
