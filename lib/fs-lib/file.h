#pragma once
#include "Cmm.h"

#define MODE_WRITE "w\0"
#define MODE_READ  "r\0"

#define ASSERT_FILE(ptr) \
    if (ptr == 0) { \
        foreign "C" puts("Error: File!\0" "ptr"); \
        return (1); \
    }
