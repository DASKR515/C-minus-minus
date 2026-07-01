lua + cmm via C lib
```
gcc -c main.c -o app.o -I. 
```
```
ghc -no-hs-main main.cmm .\app.o .\liblua54.a -o app.exe
```
