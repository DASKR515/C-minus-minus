# stdc--.h

This stdc--.h file is a wrapper for C libraries for C-- . Almost all the names are invented by me, mostly as jokes trolling C++ (like cout, mout). The main idea of the file is to save you from writing too much FFI boilerplate.
The file is semi-stable, with continuous maintenance and new additions on the way.
As for math.h and the like, they will be wrapped later but they're still experimental because they're too complex compared to the other libraries.

# C-- Standard Library (`stdc--.h`) - API Reference Wiki

This document provides a comprehensive mapping and comparison between the standard C library functions and the custom C-- macros implemented via `foreign "C"`.

---

## 1. `<stdio.h>` - Input and Output

### Standard Output & Formatting
| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `fdopen(0, "r\0")` | `mget_stdin()` | Opens the standard input stream (stdin) as a file pointer for reading. |
| `puts(str)` | `mout(str)` | Writes a string to stdout, appending a newline character automatically. |
| `printf(...)` | `moutf(...)` | Prints a formatted string to the standard output stream (stdout). |
| `putchar(c)` | `moutc(c)` | Writes a single character to the standard output stream (stdout). |
| `fputs(str, stream)` | `fmout(stream, str)` | Writes a raw string to a specified file or output stream. |
| `fputc(c, stream)` | `fmoutc(c, stream)` | Writes a single character to a specified file or output stream. |
| `fprintf(stream, ...)` | `fmoutf(stream, ...)` | Prints a formatted string to a specified file or output stream. |
| `sprintf(buf, ...)` | `smoutf(buf, ...)` | Formats and stores a string into a local buffer (risky if unchecked). |
| `snprintf(buf, n, ...)`| `snmoutf(buf, n, ...)`| Formats and stores up to `n` characters into a buffer safely to prevent overflows. |
| `vprintf(fmt, args)` | `vmoutf(fmt, args)` | Prints a formatted string to stdout using a variable argument list (`va_list`). |
| `vfprintf(stream, fmt, args)`| `vfmoutf(stream, fmt, args)`| Prints a formatted string to a stream using a variable argument list. |
| `vsprintf(buf, fmt, args)`| `vsmoutf(buf, fmt, args)`| Writes a formatted string to a buffer using a variable argument list. |
| `vsnprintf(buf, n, fmt, args)`| `vsnmoutf(buf, n, fmt, args)`| Writes a bounded formatted string to a buffer using a variable argument list. |

### Standard Input & Parsing
| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `scanf(...)` | `min(...)` | Reads and parses formatted input from the standard input stream (stdin). |
| `getchar()` | `minc()` | Reads the next single character from the standard input stream. |
| `fgetc(stream)` | `fminc(stream)` | Reads the next single character from a specified input stream. |
| `fgets(buf, n, stream)` | `fmin(buf, n, stream)` | Reads a line or up to `n-1` characters from a stream into a buffer. |
| `fscanf(stream, ...)` | `fminf(stream, ...)` | Reads and parses formatted data from a specified input stream. |
| `sscanf(buf, ...)` | `sminf(buf, ...)` | Reads and parses formatted data directly from a raw string buffer. |
| `vscanf(fmt, args)` | `vminf(fmt, args)` | Scans formatted input from stdin using a variable argument list. |
| `vfscanf(stream, fmt, args)`| `vfminf(stream, fmt, args)`| Scans formatted input from a stream using a variable argument list. |
| `vsscanf(buf, fmt, args)`| `vsminf(buf, fmt, args)`| Scans formatted data from a buffer using a variable argument list. |
| `ungetc(c, stream)` | `mungetc(c, stream)` | Pushes a character back into the input stream so it can be read again. |

### File Operations & System I/O
| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `fopen(path, mode)` | `mopen(path, mode)` | Opens a file at the specified path with a given access mode (r, w, a, etc.). |
| `freopen(path, mode, stream)`| `mreopen(path, mode, stream)`| Reassigns an existing stream pointer to a different file or mode. |
| `fclose(stream)` | `mclose(stream)` | Closes an open file stream and flushes all associated buffers. |
| `fflush(stream)` | `mflush(stream)` | Forces a write of all buffered data to the underlying disk or stream. |
| `remove(path)` | `mremove(path)` | Deletes the file specified by the given system path. |
| `rename(old, new_)` | `mrename(old, new_)` | Renames or moves a file from an old path to a new location. |
| `tmpfile()` | `mtmpfile()` | Creates a temporary binary file that is automatically deleted when closed. |
| `tmpnam(buf)` | `mtmpname(buf)` | Generates a unique, valid temporary filename string. |
| `fread(ptr, size, cnt, stream)`| `mread(ptr, size, cnt, stream)`| Reads raw binary data blocks from a file stream into a memory buffer. |
| `fwrite(ptr, size, cnt, stream)`| `mwrite(ptr, size, cnt, stream)`| Writes raw binary data blocks from a memory buffer to a file stream. |

### Positioning & Error Handling
| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `fseek(stream, off, whence)`| `mseek(stream, off, whence)`| Moves the file position indicator to a specific offset relative to `whence`. |
| `ftell(stream)` | `mtell(stream)` | Returns the current byte offset position within the given file stream. |
| `rewind(stream)` | `mrewind(stream)` | Resets the file position indicator to the very beginning of the file. |
| `fgetpos(stream, pos)` | `mgetpos(stream, pos)` | Stores the current file position into a platform-specific `fpos_t` object. |
| `fsetpos(stream, pos)` | `msetpos(stream, pos)` | Restores the file position using a previously saved `fpos_t` object. |
| `feof(stream)` | `meof(stream)` | Checks if the End-of-File indicator has been set for the stream. |
| `ferror(stream)` | `merror(stream)` | Checks if an error indicator has been set on the specified stream. |
| `clearerr(stream)` | `mclearerr(stream)` | Clears both error and EOF indicators for the given file stream. |
| `perror(str)` | `mperror(str)` | Prints a custom string followed by the system error message for `errno`. |
| `setbuf(stream, buf)` | `msetbuf(stream, buf)` | Assigns an alternative buffer for stream operations (or disables buffering). |
| `setvbuf(stream, buf, mode, size)`| `msetvbuf(stream, buf, mode, size)`| Controls buffering mode (Full, Line, Unbuffered) and size for a stream. |

---

## 2. `<stdlib.h>` - General Utilities & Memory Management

### Dynamic Memory
| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `malloc(size)` | `mnew(size)` | Allocates an uninitialized block of memory of the specified size in bytes. |
| `calloc(cnt, size)` | `mnewz(cnt, size)` | Allocates memory for an array of elements, initializing all bytes to zero. |
| `realloc(ptr, size)` | `mresize(ptr, size)` | Resizes a previously allocated memory block, preserving existing data. |
| `free(ptr)` | `mfree(ptr)` | Deallocates a memory block back to the OS to prevent memory leaks. |

### Process Control
| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `exit(code)` | `mexit(code)` | Terminates the calling process normally, performing standard cleanups. |
| `abort()` | `mab_ort()` | Terminates the process abnormally by raising the `SIGABRT` signal. |
| `quick_exit(code)` | `mquick_exit(code)` | Terminates the process quickly without running standard `atexit` cleanups. |
| `atexit(func)` | `matexit(func)` | Registers a function to be executed automatically when the program exits. |
| `at_quick_exit(func)` | `mat_quick_exit(func)` | Registers a function to be executed upon a `quick_exit` call. |

### Numeric Transformations
| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `atoi(str)` | `mtoi(str)` | Converts an ASCII string to a standard 32-bit integer (`int`). |
| `atol(str)` | `mtol(str)` | Converts an ASCII string to a long integer (`long`). |
| `atoll(str)` | `mtoll(str)` | Converts an ASCII string to a long long integer (`long long`). |
| `atof(str)` | `mtof(str)` | Converts an ASCII string to a double-precision floating-point number. |
| `strtol(str, end, base)` | `mstrtol(str, end, base)` | Converts a string to a long integer with radix control and error bounds. |
| `strtoul(str, end, base)`| `mstrtoul(str, end, base)`| Converts a string to an unsigned long integer with radix control. |
| `strtoll(str, end, base)`| `mstrtoll(str, end, base)`| Converts a string to a signed long long integer. |
| `strtoull(str, end, base)`| `mstrtoull(str, end, base)`| Converts a string to an unsigned long long integer. |
| `strtof(str, end)` | `mstrtof(str, end)` | Converts a string string to a single-precision float. |
| `strtod(str, end)` | `mstrtod(str, end)` | Converts a string to a double-precision floating-point number. |
| `strtold(str, end)` | `mstrtold(str, end)` | Converts a string to an extended-precision long double. |

### Randomization, Math & Searching
| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `rand()` | `mrand()` | Generates a pseudo-random integer up to `RAND_MAX`. |
| `srand(seed)` | `msrand(seed)` | Initializes the pseudo-random number generator with a starting seed. |
| `qsort(base, cnt, size, cmp)`| `mqsort(base, cnt, size, cmp)`| Sorts an array of data blocks using the Quick Sort algorithm. |
| `bsearch(key, base, cnt, size, cmp)`| `mbsearch(key, base, cnt, size, cmp)`| Performs a binary search over a pre-sorted array data block. |
| `abs(x)` | `mabs(x)` | Computes the absolute value of an integer. |
| `labs(x)` | `mlabs(x)` | Computes the absolute value of a long integer. |
| `llabs(x)` | `mllabs(x)` | Computes the absolute value of a long long integer. |
| `div(num, den)` | `mdiv(num, den)` | Computes quotient and remainder of an integer division as a structure. |
| `ldiv(num, den)` | `mldiv(num, den)` | Computes quotient and remainder of a long integer division. |
| `lldiv(num, den)` | `mlldiv(num, den)` | Computes quotient and remainder of a long long integer division. |

### Environment & Multibyte Conversions
| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `getenv(name)` | `mgetenv(name)` | Retrieves the value of an environment variable from the host system. |
| `putenv(name)` | `msetenv(name, val, ovr)` | *Warning:* Linked to `putenv` to set/modify system environment variables. |
| `putenv(name)` | `munsetenv(name)` | *Warning:* Linked to `putenv` to remove or clear environment variables. |
| `putenv(str)` | `mputenv(str)` | Directly introduces a string expression into the system environment block. |
| `system(cmd)` | `msystem(cmd)` | Passes an administrative command string to the host CLI interpreter. |
| `mblen(s, n)` | `mmblen(s, n)` | Determines the length in bytes of a multibyte character string. |
| `mbtowc(pwc, s, n)` | `mmbtowc(pwc, s, n)` | Converts a multibyte character sequence to a wide character (`wchar_t`). |
| `wctomb(s, wc)` | `mwctomb(s, wc)` | Converts a wide character down to a multibyte string sequence. |
| `mbstowcs(dst, src, n)` | `mmbstowcs(dst, src, n)`| Transforms a multibyte string array into a wide character array. |
| `wcstombs(dst, src, n)` | `mwcstombs(dst, src, n)`| Transforms a wide character array into a multibyte string array. |

---

## 3. `<string.h>` - String Manipulation & Memory Operations

### Cloning & Appending
| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `strcpy(dst, src)` | `mstrcpy(dst, src)` | Copies a null-terminated string completely from source to destination. |
| `strncpy(dst, src, n)` | `mstrncpy(dst, src, n)` | Copies up to `n` characters from a source text to a destination buffer. |
| `memcpy(dst, src, n)` | `mmemcpy(dst, src, n)` | Copies `n` bytes of raw memory from source to destination (non-overlapping). |
| `memmove(dst, src, n)` | `mmemmove(dst, src, n)` | Moves `n` bytes safely even if source and destination memory regions overlap. |
| `strcat(dst, src)` | `mstrcat(dst, src)` | Concatenates/appends the source string onto the end of the destination string. |
| `strncat(dst, src, n)` | `mstrncat(dst, src, n)` | Appends up to `n` characters from the source text to the destination string. |

### Evaluation & Parsing
| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `strcmp(a, b)` | `mstrcmp(a, b)` | Lexicographically compares two null-terminated strings. Returns 0 if identical. |
| `strncmp(a, b, n)` | `mstrncmp(a, b, n)` | Compares the first `n` characters of two strings. |
| `memcmp(a, b, n)` | `mmemcmp(a, b, n)` | Compares `n` raw bytes of two memory regions regardless of data type. |
| `strcoll(a, b)` | `mstrcoll(a, b)` | Compares two strings using the runtime system's active locale configurations. |
| `strxfrm(dst, src, n)` | `mstrxfrm(dst, src, n)` | Transforms a string into a locale-independent form for accelerated comparisons. |
| `strchr(str, c)` | `mstrchr(str, c)` | Scans a string for the first occurrence of character `c` and returns its pointer. |
| `strrchr(str, c)` | `mstrrchr(str, c)` | Scans a string for the last occurrence of character `c`. |
| `strstr(hay, needle)` | `mstrstr(hay, needle)` | Scans a main string for the first occurrence of a secondary substring. |
| `strpbrk(str, chars)` | `mstrpbrk(str, chars)` | Finds the first character in a string that matches any character in a given set. |
| `strspn(str, chars)` | `mstrspn(str, chars)` | Calculates the segment length of a string containing only specified characters. |
| `strcspn(str, chars)` | `mstrcspn(str, chars)` | Calculates the segment length of a string completely excluding specified characters. |
| `strtok(str, delim)` | `mstrtok(str, delim)` | Tokenizes (splits) a string sequentially based on delimiter characters. |
| `memchr(ptr, c, n)` | `mmemchr(ptr, c, n)` | Searches a block of `n` memory bytes for the first occurrence of value `c`. |
| `strlen(str)` | `mstrlen(str)` | Counts the number of characters in a string up to (excluding) the null terminator. |
| `strerror(errnum)` | `mstrerror(errnum)` | Resolves a system error code number into a readable English error string. |
| `memset(ptr, val, n)` | `mmemset(ptr, val, n)` | Populates `n` bytes of memory with a constant repeating byte value `val`. |
| `strdup(str)` | `mstrdup(str)` | Duplicates a string by allocating matching heap memory and copying into it. |

---

## 4. `<time.h>` - Time & Clocks

| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `time(t)` | `mtime(t)` | Retrieves the current UNIX Epoch timestamp (seconds elapsed since Jan 1, 1970). |
| `clock()` | `mclock()` | Measures CPU processing time consumed by the execution of the program. |
| `_difftime64(t1, t0)` | `mdifftime(t1, t0)`| Computes the strict floating-point difference in seconds between two 64-bit times. |
| `mktime(tm)` | `mmktime(tm)` | Normalizes a broken-down `tm` time structure into a flat calendar time timestamp. |
| `strftime(buf, n, fmt, tm)`| `mstrftime(buf, n, fmt, tm)`| Generates a custom formatted text representation of a time structure. |
| `asctime(tm)` | `masctime(tm)` | Converts a broken-down `tm` structure to a standardized human-readable string. |
| `ctime(t)` | `mctime(t)` | Direct conversion of a raw timestamp pointer into a standard timestamp string. |
| `gmtime(t)` | `mgmtime(t)` | Unpacks a timestamp into a broken-down time structure set to UTC (Greenwich). |
| `localtime(t)` | `mlocaltime(t)` | Unpacks a timestamp into a broken-down time structure set to Local system time. |

---

## 5. `<ctype.h>` - Character Handling

All these macros take a character byte and return an evaluation boolean (non-zero for true, 0 for false).

| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `isalpha(c)` | `misalpha(c)` | Evaluates if the character is an alphabetic letter (A-Z, a-z). |
| `isdigit(c)` | `misdigit(c)` | Evaluates if the character is a base-10 numerical digit (0-9). |
| `isalnum(c)` | `misalnum(c)` | Evaluates if the character is either alphabetic or numerical. |
| `isspace(c)` | `misspace(c)` | Evaluates if the character is a whitespace element (space, tab, newline). |
| `ispunct(c)` | `mispunct(c)` | Evaluates if the character is a symbol or punctuation mark. |
| `isupper(c)` | `misupper(c)` | Evaluates if the character is an uppercase letter. |
| `islower(c)` | `mislower(c)` | Evaluates if the character is a lowercase letter. |
| `iscntrl(c)` | `miscntrl(c)` | Evaluates if the character is an unprintable hardware control character. |
| `isprint(c)` | `misprint(c)` | Evaluates if the character is printable (includes space character). |
| `isgraph(c)` | `misgraph(c)` | Evaluates if the character is printable and has graphical output (excludes space). |
| `isblank(c)` | `misblank(c)` | Evaluates if the character is a standard space or horizontal tab. |
| `isxdigit(c)` | `misxdigit(c)` | Evaluates if the character belongs to the hexadecimal numeric system (0-9, a-f). |
| `toupper(c)` | `mtoupper(c)` | Transforms a lowercase alphabetical character to its uppercase variant. |
| `tolower(c)` | `mtolower(c)` | Transforms an uppercase alphabetical character to its lowercase variant. |

---

## 6. `<wchar.h>` & `<wctype.h>` - Wide Character Support

| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `wprintf(str)` | `mwout(str)` | Writes a raw wide-character string to the standard output. |
| `fwprintf(stream, ...)` | `mfwout(stream, ...)` | Prints formatted wide characters to a designated file or stream. |
| `swprintf(buf, ...)` | `mswout(buf, ...)` | Formats and dumps wide character strings into a targeted wide buffer. |
| `wscanf(...)` | `mwin(...)` | Scans formatted wide character inputs from standard system input. |
| `fwscanf(stream, ...)` | `mfwin(stream, ...)` | Parses formatted wide character elements out of an active file stream. |
| `swscanf(buf, ...)` | `mswin(buf, ...)` | Parses formatted wide character elements directly out of a wide text buffer. |
| `wcscpy(dst, src)` | `mwcscpy(dst, src)` | Clones a wide character string array into a targeted destination array. |
| `wcsncpy(dst, src, n)` | `mwcsncpy(dst, src, n)` | Safely clones up to `n` wide characters from source to destination. |
| `wcscat(dst, src)` | `mwcscat(dst, src)` | Concatenates a wide character string to the end of another. |
| `wcsncat(dst, src, n)` | `mwcsncat(dst, src, n)` | Appends a maximum of `n` wide characters onto a targeted wide buffer. |
| `wcscmp(a, b)` | `mwcscmp(a, b)` | Performs strict alphabetical comparisons between two wide strings. |
| `mwcsncmp(a, b, n)` | `mwcsncmp(a, b, n)` | *Self-reference Warning:* Compares up to `n` elements of wide character strings. |
| `wcschr(str, c)` | `mwcschr(str, c)` | Locates the primary pointer match of wide character `c` inside wide string `str`. |
| `wcsstr(hay, needle)` | `mwcsstr(hay, needle)` | Discovers the coordinate of a wide substring inside a main wide string. |
| `wcslen(str)` | `mwcslen(str)` | Determines the actual character count inside a wide character string. |
| `fgetwc(stream)` | `mfgetwc(stream)` | Pulls a single wide character frame out of an input file stream. |
| `fputwc(c, stream)` | `mfputwc(c, stream)` | Pushes a single wide character frame into an active output file stream. |
| `fgetws(buf, n, stream)`| `mfgetws(buf, n, stream)`| Reads a wide character line string out of an input stream into a wide buffer. |
| `fputws(str, stream)` | `mfputws(str, stream)` | Emits a complete wide character string directly into a designated file path. |
| `iswalpha(c)` | `miswalpha(c)` | Verifies if the wide character structure is alphabetical. |
| `iswdigit(c)` | `miswdigit(c)` | Verifies if the wide character structure is numerical. |
| `iswspace(c)` | `miswspace(c)` | Verifies if the wide character is an empty spaces or layout tab. |
| `iswupper(c)` | `miswupper(c)` | Verifies if the wide character structure is in capitalized formatting. |
| `iswlower(c)` | `miswlower(c)` | Verifies if the wide character structure is in lowercase formatting. |
| `towupper(c)` | `mtowupper(c)` | Upgrades a wide character structure directly to uppercase format. |
| `towlower(c)` | `mtowlower(c)` | Downgrades a wide character structure directly to lowercase format. |

---

## 7. `<inttypes.h>` - Maximum-Width Integers

| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `imaxabs(x)` | `mimaxabs(x)` | Calculates the absolute value of an integer of largest available native bit-depth. |
| `imaxdiv(num, den)` | `mimaxdiv(num, den)` | Divides maximum-width integers and returns the quotient and remainder. |
| `strtoimax(str, end, base)`| `mstrtoimax(str, end, base)`| Casts a character text string into a signed maximum-width int value. |
| `strtoumax(str, end, base)`| `mstrtoumax(str, end, base)`| Casts a character text string into an unsigned maximum-width int value. |
| `wcstoimax(str, end, base)`| `mwcstoimax(str, end, base)`| Casts a wide text string into a signed maximum-width integer value. |
| `wcstoumax(str, end, base)`| `mwcstoumax(str, end, base)`| Casts a wide text string into an unsigned maximum-width integer value. |

---

## 8. `<locale.h>` - Localization Controls

| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `setlocale(cat, locale)` | `msetlocale(cat, locale)`| Selects or reads the current system localization parameters. |
| `localeconv()` | `mlocaleconv()` | Acquires localized numeric and monetary structural presentation rules. |

---

## 9. `<setjmp.h>` - Non-Local Jumps (Execution Bypassing)

| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `setjmp(env)` | `msetjmp(env)` | Flags the local registers state and position for emergency stack recovery. |
| `longjmp(env, val)` | `mlongjmp(env, val)` | Forced execution skip that rewinds stack frame to the marked `setjmp` environment. |

---

## 10. `<signal.h>` - Hardware & Software Interrupts

| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `signal(sig, handler)` | `msignal(sig, handler)`| Configures a callback logic sequence to handle specific system signal interrupts. |
| `raise(sig)` | `mraise(sig)` | Programmatically issues a software interrupt signal right to the running instance. |

---

## 11. Assertions & Runtime Testing

| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `-` | `mcheck(cond)` | Blank compilation space anchor; reserved for passive conditional testing. |
| `abort()` | `massert(msg, file, line)`| Hard execution intercept; instantly kills the current binary instance on assertion failure. |

---

## 12. `<fenv.h>` - Floating-Point Hardware Environment

| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `feclearexcept(flags)` | `mfeclearexcept(flags)` | Clears the specified floating-point hardware exception status flags. |
| `fetestexcept(flags)` | `mfetestexcept(flags)` | Checks which floating-point hardware exception flags are currently set. |
| `fegetround()` | `mfegetround()` | Returns the active hardware rounding direction mode for float math. |
| `fesetround(mode)` | `mfesetround(mode)` | Establishes the preferred floating-point hardware rounding mode. |
| `fegetenv(envp)` | `mfegetenv(envp)` | Saves the entire state of the floating-point environment into an object. |
| `fesetenv(envp)` | `mfesetenv(envp)` | Restores a previously captured floating-point hardware state environment. |
| `feholdexcept(envp)` | `mfeholdexcept(envp)` | Clears active exceptions and saves state for uninterrupted precision math. |
| `feupdateenv(envp)` | `mfeupdateenv(envp)` | Restores a saved environment state but re-raises previously active exceptions. |
| `fegetexceptflag(flagp, ex)`| `mfegetexceptflag(flagp, ex)`| Stores the current state of specific floating-point exception flags. |
| `fesetexceptflag(flagp, ex)`| `mfesetexceptflag(flagp, ex)`| Re-establishes selected exception flags from a saved configuration flag. |
| `feraiseexcept(flags)` | `mferaiseexcept(flags)` | Explicitly forces the generation of specified floating-point hardware exceptions. |

---

## 13. `<stdatomic.h>` - Thread-Safe Atomic Actions

| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `atomic_init(obj, val)` | `matomic_init(obj, val)` | Initializes an atomic variable with a starting thread-safe value. |
| `atomic_load(obj)` | `matomic_load(obj)` | Atomically safely reads and returns the current state of an atomic variable. |
| `atomic_store(obj, val)` | `matomic_store(obj, val)` | Atomically safely overwrites an atomic variable with a new value. |
| `atomic_exchange(obj, val)`| `matomic_exchange(obj, val)`| Replaces value of an atomic target and yields its previous contents in return. |
| `atomic_fetch_add(obj, val)`| `matomic_fetch_add(obj, val)`| Adds a value to an atomic object, returning the object's value prior to addition. |
| `atomic_fetch_sub(obj, val)`| `matomic_fetch_sub(obj, val)`| Subtracts a value from an atomic object, returning its value prior to subtraction. |
| `atomic_fetch_and(obj, val)`| `matomic_fetch_and(obj, val)`| Computes bitwise AND with an atomic object, returning the older value. |
| `atomic_fetch_or(obj, val)` | `matomic_fetch_or(obj, val)` | Computes bitwise OR with an atomic object, returning the older value. |
| `atomic_fetch_xor(obj, val)`| `matomic_fetch_xor(obj, val)`| Computes bitwise XOR with an atomic object, returning the older value. |
| `atomic_compare_exchange_strong(...)`| `matomic_compare_exchange_strong(...)`| Strictly compares and updates an atomic target if it matches expectation. |
| `atomic_compare_exchange_weak(...)`| `matomic_compare_exchange_weak(...)`| Performs a lighter, loop-friendly compare-and-swap (can spuriously fail). |
| `atomic_flag_test_and_set(f)`| `matomic_flag_test_and_set(f)`| Tests a boolean flag state and forces its value to true in an indivisible step. |
| `atomic_flag_clear(f)` | `matomic_flag_clear(f)` | Atomically safely drops a boolean flag state back to false. |
| `atomic_thread_fence(order)`| `matomic_thread_fence(order)`| Erects a memory pipeline fence across threads to enforce precise sequencing. |
| `atomic_signal_fence(order)`| `matomic_signal_fence(order)`| Erects an instruction fence ordering memory actions within the current thread. |

---

## 14. `<threads.h>` - Low-Level Thread Concurrency

### Execution Threads
| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `thrd_create(t, func, arg)`| `mthrd_create(t, func, arg)`| Spawns a parallel hardware thread to run a specific function concurrently. |
| `thrd_join(t, res)` | `mthrd_join(t, res)` | Blocks the caller thread until target thread `t` completes, catching its return code. |
| `thrd_detach(t)` | `mthrd_detach(t)` | Disconnects the thread so its system resources automatically free up upon termination. |
| `thrd_exit(res)` | `mthrd_exit(res)` | Instantly terminates the calling thread, returning an exit result code. |
| `thrd_current()` | `mthrd_current()` | Captures the unique runtime platform identifier of the calling thread. |
| `thrd_equal(a, b)` | `mthrd_equal(a, b)` | Validates whether two thread ID records refer to the exact same thread instance. |
| `thrd_sleep(dur, rem)` | `mthrd_sleep(dur, rem)` | Suspends thread execution for a designated duration period block. |
| `thrd_yield()` | `mthrd_yield()` | Volunteers to give up remaining CPU time slices, letting other threads execute. |

### Mutex Locks
| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `mtx_init(m, type)` | `mmtx_init(m, type)` | Initializes a mutual exclusion lock object (Mutex) of a specific type. |
| `mtx_lock(m)` | `mmtx_lock(m)` | Acquires a mutex lock, blocking execution indefinitely if already claimed. |
| `mtx_trylock(m)` | `mmtx_trylock(m)` | Attempts to secure a mutex lock immediately without blocking if it fails. |
| `mtx_unlock(m)` | `mmtx_unlock(m)` | Releases a mutex lock, allowing waiting threads to proceed. |
| `mtx_destroy(m)` | `mmtx_destroy(m)` | Cleans up and destroys a mutex lock object, freeing system allocations. |

### Condition Variables & Local Storage
| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `cnd_init(c)` | `mcnd_init(c)` | Initializes a condition variable for managing thread wake/sleep signals. |
| `cnd_signal(c)` | `mcnd_signal(c)` | Wakes up at least one thread currently waiting on the condition variable. |
| `cnd_broadcast(c)` | `mcnd_broadcast(c)` | Wakes up all threads currently blocked and waiting on the condition variable. |
| `cnd_wait(c, m)` | `mcnd_wait(c, m)` | Unlocks a mutex and blocks the thread until signaled by another thread. |
| `cnd_destroy(c)` | `mcnd_destroy(c)` | Destroys a condition variable, releasing internal system resources. |
| `tss_create(key, dtor)` | `mtss_create(key, dtor)` | Instantiates a Thread-Specific Storage data key mapping with an optional destructor. |
| `tss_get(key)` | `mtss_get(key)` | Retrieves thread-isolated unique storage values linked with the target key. |
| `tss_set(key, val)` | `mtss_set(key, val)` | Stores thread-isolated unique data values under the designated storage key. |
| `tss_delete(key)` | `mtss_delete(key)` | Removes a thread-specific storage key completely and drops allocations. |
| `call_once(flag, func)` | `mcall_once(flag, func)` | Guarantees that a configuration function executes exactly once, even across threads. |

---

## 15. `<uchar.h>` - Unicode UTF Transformation Adapters

| Standard C Function | Custom C-- Macro | English Description |
| :--- | :--- | :--- |
| `mbrtoc16(pc16, s, n, ps)`| `mmbrtoc16(pc16, s, n, ps)`| Converts a multibyte character token string into a 16-bit UTF-16 character. |
| `c16rtomb(s, c16, ps)` | `mc16rtomb(s, c16, ps)` | Translates a UTF-16 wide value down into a standard multibyte string array. |
| `mbrtoc32(pc32, s, n, ps)`| `mmbrtoc32(pc32, s, n, ps)`| Converts a multibyte character token string into a 32-bit UTF-32 character. |
| `c32rtomb(s, c32, ps)` | `mc32rtomb(s, c32, ps)` | Translates a UTF-32 wide value down into a standard multibyte string array. |
