lua + cmm via C lib
```
gcc -c main.c -o app.o -I. 
```
to 
```
./gmm -o app main.cmm app.o liblua54.a
```
or
```
ghc -no-hs-main main.cmm .\app.o .\liblua54.a -o app
```
