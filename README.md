# C-minus-minus
c-- 


```bash
ghc -no-hs-main ./main.cmm -o app
```
## ex cmm
```c
#include "Cmm.h"

section "data" {
    msg: bits8[] "hi my world\0";
}

export main;
main() {
    foreign "C" puts(msg "ptr");
    return (0);
}
```
