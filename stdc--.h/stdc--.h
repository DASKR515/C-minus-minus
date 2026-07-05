/*
              --    by JABIR aka <DASKR>      
              --    This stdc--.h file is a wrapper for C libraries for C-- . Almost all the names are invented by me, mostly as jokes trolling C++ (like cout, mout).
              --    The main idea of the file is to save you from writing too much FFI boilerplate. The file is semi-stable,
              --    with continuous maintenance and new additions on the way. 
              --    As for math.h and the like, 
              --    they will be wrapped later but they're still experimental because they're too complex compared to the other libraries.
              --    ver <1.0.1>
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀   
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠰⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⡄⠀⠀⠸⡞⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⢠⠆⠀⠀⠀⠀⠀⡇⡔⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠇⠀⠀⡾⡵⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⢸⡇⠀⠀⠀⠀⠀⣶⣭⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡷⡭⠀⠀⡛⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⢺⡇⠀⠀⠀⠀⠀⣿⡗⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠸⢹⠇⠀⠸⢯⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⢳⡩⣯⢤⣶⣦⣤⣽⣥⣥⠀⠀⠀⠀⡔⣩⠤⠾⢷⠀⠀⠀⣗⢭⠀⠀⡻⣛⠀⠀⠀⠀⠀⠀⠀⡠⠤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠸⡿⣿⠿⠛⠛⠛⢿⢻⠉⠀⠀⠀⣸⠞⠁⣠⠏⣧⠇⠀⠀⡽⡇⠀⠀⡷⡇⠀⠀⠀⠀⠀⣤⡁⣸⡤⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⢏⣿⠀⠀⠀⠀⠀⣿⣽⠀⠀⠀⠀⢫⣧⣾⠿⠉⠀⠀⠀⠀⢭⡿⠀⠀⡷⡝⠀⠀⠀⠀⣬⡷⠋⠉⢫⡏⢿⡂⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⡍⠫⠀⠀⠀⠀⠀⠙⠃⠀⠀⠀⠀⠀⣿⣮⣗⠦⠤⣄⡀⠀⡟⡇⠀⠀⢯⡇⠀⠀⠀⢌⡇⠀  ⠀⢹⠠⠃⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠳⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠛⠿⠿⠃⠀⠸⣿⠀⠀⢸⡗⢤⠀⠀⠀⠯⠧⡀⠀⠀⣠⠏⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢿⠔⠃⠀⠀⠀⠉⠛⠭⡓⠉⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣶⢟⣓⢏⢳⠀⢀⠤⡖⢦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡏⣼⣿⣯⢿⢇⣇⣿⣾⣿⣏⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⡽⣿⠮⣯⣟⣿⡟⣉⠿⣿⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢍⣷⡷⣿⣯⢟⢿⣾⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⣿⣿⣿⡿⠕⠊⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢩⠟⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
*/


#pragma once
#include "Cmm.h"

#define mget_stdin()           foreign "C" fdopen(0, "r\0")
#define mout(str)              foreign "C" puts(str)
#define moutf(...)             foreign "C" printf(__VA_ARGS__)
#define moutc(c)               foreign "C" putchar(c)
#define fmout(stream, str)      foreign "C" fputs(str, stream)
#define fmoutc(c, stream)      foreign "C" fputc(c, stream)
#define fmoutf(stream, ...)    foreign "C" fprintf(stream, __VA_ARGS__)
#define smoutf(buf, ...)       foreign "C" sprintf(buf, __VA_ARGS__)
#define snmoutf(buf, n, ...)   foreign "C" snprintf(buf, n, __VA_ARGS__)
#define vmoutf(fmt, args)      foreign "C" vprintf(fmt, args)
#define vfmoutf(stream, fmt, args)  foreign "C" vfprintf(stream, fmt, args)
#define vsmoutf(buf, fmt, args)     foreign "C" vsprintf(buf, fmt, args)
#define vsnmoutf(buf, n, fmt, args) foreign "C" vsnprintf(buf, n, fmt, args)

#define min(...)               foreign "C" scanf(__VA_ARGS__)
#define minc()                 foreign "C" getchar()
#define mfinc(stream)          foreign "C" fgetc(stream)
#define mfin(buf, n, stream)   foreign "C" fgets(buf, n, stream)
#define mfinf(stream, ...)     foreign "C" fscanf(stream, __VA_ARGS__)
#define sminf(buf, ...)        foreign "C" sscanf(buf, __VA_ARGS__)
#define vminf(fmt, args)       foreign "C" vscanf(fmt, args)
#define vfminf(stream, fmt, args)  foreign "C" vfscanf(stream, fmt, args)
#define vsminf(buf, fmt, args)     foreign "C" vsscanf(buf, fmt, args)
#define mungetc(c, stream)     foreign "C" ungetc(c, stream)

#define mopen(path, mode)      foreign "C" fopen(path, mode)
#define mreopen(path, mode, stream) foreign "C" freopen(path, mode, stream)
#define mclose(stream)         foreign "C" fclose(stream)
#define mflush(stream)         foreign "C" fflush(stream)
#define mremove(path)          foreign "C" remove(path)
#define mrename(old, new_)          foreign "C" rename(old, new_)
#define mtmpfile()             foreign "C" tmpfile()
#define mtmpname(buf)          foreign "C" tmpnam(buf)

#define mread(ptr, size, cnt, stream)  foreign "C" fread(ptr, size, cnt, stream)
#define mwrite(ptr, size, cnt, stream) foreign "C" fwrite(ptr, size, cnt, stream)

#define mseek(stream, off, whence) foreign "C" fseek(stream, off, whence)
#define mtell(stream)           foreign "C" ftell(stream)
#define mrewind(stream)         foreign "C" rewind(stream)
#define mgetpos(stream, pos)    foreign "C" fgetpos(stream, pos)
#define msetpos(stream, pos)    foreign "C" fsetpos(stream, pos)

#define meof(stream)            foreign "C" feof(stream)
#define merror(stream)          foreign "C" ferror(stream)
#define mclearerr(stream)       foreign "C" clearerr(stream)
#define mperror(str)            foreign "C" perror(str)

#define msetbuf(stream, buf)    foreign "C" setbuf(stream, buf)
#define msetvbuf(stream, buf, mode, size) foreign "C" setvbuf(stream, buf, mode, size)

#define mnew(size)              foreign "C" malloc(size)
#define mnewz(cnt, size)        foreign "C" calloc(cnt, size)
#define mresize(ptr, size)      foreign "C" realloc(ptr, size)
#define mfree(ptr)              foreign "C" free(ptr)

#define mexit(code)             foreign "C" exit(code)
#define mabort()                foreign "C" abort()
#define mquick_exit(code)       foreign "C" quick_exit(code)
#define matexit(func)           foreign "C" atexit(func)
#define mat_quick_exit(func)    foreign "C" at_quick_exit(func)

#define mtoi(str)               foreign "C" atoi(str)
#define mtol(str)               foreign "C" atol(str)
#define mtoll(str)              foreign "C" atoll(str)
#define mtof(str)               foreign "C" atof(str)
#define mstrtol(str, end, base)     foreign "C" strtol(str, end, base)
#define mstrtoul(str, end, base)    foreign "C" strtoul(str, end, base)
#define mstrtoll(str, end, base)    foreign "C" strtoll(str, end, base)
#define mstrtoull(str, end, base)   foreign "C" strtoull(str, end, base)
#define mstrtof(str, end)       foreign "C" strtof(str, end)
#define mstrtod(str, end)       foreign "C" strtod(str, end)
#define mstrtold(str, end)      foreign "C" strtold(str, end)

#define mrand()                 foreign "C" rand()
#define msrand(seed)            foreign "C" srand(seed)

#define mqsort(base, cnt, size, cmp)   foreign "C" qsort(base, cnt, size, cmp)
#define mbsearch(key, base, cnt, size, cmp) foreign "C" bsearch(key, base, cnt, size, cmp)

#define mabs(x)                 foreign "C" abs(x)
#define mlabs(x)                foreign "C" labs(x)
#define mllabs(x)               foreign "C" llabs(x)
#define mdiv(num, den)          foreign "C" div(num, den)
#define mldiv(num, den)         foreign "C" ldiv(num, den)
#define mlldiv(num, den)        foreign "C" lldiv(num, den)

#define mgetenv(name)           foreign "C" getenv(name)
#define msetenv(name, val, ovr)  foreign "C" setenv(name, val, ovr)
#define munsetenv(name)         foreign "C" unsetenv(name)
#define mputenv(str)            foreign "C" putenv(str)
#define msystem(cmd)            foreign "C" system(cmd)

#define mmblen(s, n)            foreign "C" mblen(s, n)
#define mmbtowc(pwc, s, n)      foreign "C" mbtowc(pwc, s, n)
#define mwctomb(s, wc)          foreign "C" wctomb(s, wc)
#define mmbstowcs(dst, src, n)  foreign "C" mbstowcs(dst, src, n)
#define mwcstombs(dst, src, n)  foreign "C" wcstombs(dst, src, n)

#define mstrcpy(dst, src)           foreign "C" strcpy(dst, src)
#define mstrncpy(dst, src, n)       foreign "C" strncpy(dst, src, n)
#define mmemcpy(dst, src, n)        foreign "C" memcpy(dst, src, n)
#define mmemmove(dst, src, n)       foreign "C" memmove(dst, src, n)

#define mstrcat(dst, src)           foreign "C" strcat(dst, src)
#define mstrncat(dst, src, n)       foreign "C" strncat(dst, src, n)

#define mstrcmp(a, b)               foreign "C" strcmp(a, b)
#define mstrncmp(a, b, n)           foreign "C" strncmp(a, b, n)
#define mmemcmp(a, b, n)            foreign "C" memcmp(a, b, n)
#define mstrcoll(a, b)              foreign "C" strcoll(a, b)
#define mstrxfrm(dst, src, n)       foreign "C" strxfrm(dst, src, n)

#define mstrchr(str, c)             foreign "C" strchr(str, c)
#define mstrrchr(str, c)            foreign "C" strrchr(str, c)
#define mstrstr(hay, needle)        foreign "C" strstr(hay, needle)
#define mstrpbrk(str, chars)        foreign "C" strpbrk(str, chars)
#define mstrspn(str, chars)         foreign "C" strspn(str, chars)
#define mstrcspn(str, chars)        foreign "C" strcspn(str, chars)
#define mstrtok(str, delim)         foreign "C" strtok(str, delim)
#define mmemchr(ptr, c, n)          foreign "C" memchr(ptr, c, n)

#define mstrlen(str)                foreign "C" strlen(str)
#define mstrerror(errnum)           foreign "C" strerror(errnum)
#define mmemset(ptr, val, n)        foreign "C" memset(ptr, val, n)
#define mstrdup(str)                foreign "C" strdup(str)

#define mtime(t)                    foreign "C" time(t)
#define mclock()                    foreign "C" clock()
#define mdifftime(t1, t0)           foreign "C" difftime(t1, t0)
#define mmktime(tm)                 foreign "C" mktime(tm)

#define mstrftime(buf, n, fmt, tm)  foreign "C" strftime(buf, n, fmt, tm)
#define masctime(tm)                foreign "C" asctime(tm)
#define mctime(t)                   foreign "C" ctime(t)

#define mgmtime(t)                  foreign "C" gmtime(t)
#define mlocaltime(t)               foreign "C" localtime(t)

#define misalpha(c)             foreign "C" isalpha(c)
#define misdigit(c)             foreign "C" isdigit(c)
#define misalnum(c)             foreign "C" isalnum(c)
#define misspace(c)             foreign "C" isspace(c)
#define mispunct(c)             foreign "C" ispunct(c)
#define misupper(c)             foreign "C" isupper(c)
#define mislower(c)             foreign "C" islower(c)
#define miscntrl(c)             foreign "C" iscntrl(c)
#define misprint(c)             foreign "C" isprint(c)
#define misgraph(c)             foreign "C" isgraph(c)
#define misblank(c)             foreign "C" isblank(c)
#define misxdigit(c)            foreign "C" isxdigit(c)

#define mtoupper(c)             foreign "C" toupper(c)
#define mtolower(c)             foreign "C" tolower(c)

#define mwout(str)                  foreign "C" wprintf(str)
#define mfwout(stream, ...)         foreign "C" fwprintf(stream, __VA_ARGS__)
#define mswout(buf, ...)            foreign "C" swprintf(buf, __VA_ARGS__)

#define mwin(...)                   foreign "C" wscanf(__VA_ARGS__)
#define mfwin(stream, ...)          foreign "C" fwscanf(stream, __VA_ARGS__)
#define mswin(buf, ...)             foreign "C" swscanf(buf, __VA_ARGS__)

#define mwcscpy(dst, src)           foreign "C" wcscpy(dst, src)
#define mwcsncpy(dst, src, n)       foreign "C" wcsncpy(dst, src, n)
#define mwcscat(dst, src)           foreign "C" wcscat(dst, src)
#define mwcsncat(dst, src, n)       foreign "C" wcsncat(dst, src, n)

#define mwcscmp(a, b)               foreign "C" wcscmp(a, b)
#define mwcsncmp(a, b, n)           foreign "C" wcsncmp(a, b, n)
#define mwcschr(str, c)             foreign "C" wcschr(str, c)
#define mwcsstr(hay, needle)        foreign "C" wcsstr(hay, needle)
#define mwcslen(str)                foreign "C" wcslen(str)

#define mfgetwc(stream)             foreign "C" fgetwc(stream)
#define mfputwc(c, stream)          foreign "C" fputwc(c, stream)
#define mfgetws(buf, n, stream)     foreign "C" fgetws(buf, n, stream)
#define mfputws(str, stream)        foreign "C" fputws(str, stream)

#define miswalpha(c)             foreign "C" iswalpha(c)
#define miswdigit(c)             foreign "C" iswdigit(c)
#define miswspace(c)             foreign "C" iswspace(c)
#define miswupper(c)             foreign "C" iswupper(c)
#define miswlower(c)             foreign "C" iswlower(c)
#define mtowupper(c)             foreign "C" towupper(c)
#define mtowlower(c)             foreign "C" towlower(c)

#define mimaxabs(x)                 foreign "C" imaxabs(x)
#define mimaxdiv(num, den)          foreign "C" imaxdiv(num, den)
#define mstrtoimax(str, end, base)  foreign "C" strtoimax(str, end, base)
#define mstrtoumax(str, end, base)  foreign "C" strtoumax(str, end, base)
#define mwcstoimax(str, end, base)  foreign "C" wcstoimax(str, end, base)
#define mwcstoumax(str, end, base)  foreign "C" wcstoumax(str, end, base)

#define msetlocale(cat, locale)     foreign "C" setlocale(cat, locale)
#define mlocaleconv()                foreign "C" localeconv()

#define msetjmp(env)             foreign "C" setjmp(env)
#define mlongjmp(env, val)       foreign "C" longjmp(env, val)

#define msignal(sig, handler)    foreign "C" signal(sig, handler)
#define mraise(sig)              foreign "C" raise(sig)
/*
problem 
#define mcheck(cond)             
#define massert(msg, file, line) foreign "C" abort()
*/
#define POP() foreign "C" printf("pop is badboy\n")

#define mfeclearexcept(flags)       foreign "C" feclearexcept(flags)
#define mfetestexcept(flags)        foreign "C" fetestexcept(flags)
#define mfegetround()                foreign "C" fegetround()
#define mfesetround(mode)           foreign "C" fesetround(mode)
#define mfegetenv(envp)             foreign "C" fegetenv(envp)
#define mfesetenv(envp)             foreign "C" fesetenv(envp)
#define mfeholdexcept(envp)         foreign "C" feholdexcept(envp)
#define mfeupdateenv(envp)          foreign "C" feupdateenv(envp)
#define mfegetexceptflag(flagp, ex) foreign "C" fegetexceptflag(flagp, ex)
#define mfesetexceptflag(flagp, ex) foreign "C" fesetexceptflag(flagp, ex)
#define mferaiseexcept(flags)       foreign "C" feraiseexcept(flags)

#define matomic_init(obj, val)          foreign "C" atomic_init(obj, val)
#define matomic_load(obj)               foreign "C" atomic_load(obj)
#define matomic_store(obj, val)         foreign "C" atomic_store(obj, val)
#define matomic_exchange(obj, val)      foreign "C" atomic_exchange(obj, val)
#define matomic_fetch_add(obj, val)     foreign "C" atomic_fetch_add(obj, val)
#define matomic_fetch_sub(obj, val)     foreign "C" atomic_fetch_sub(obj, val)
#define matomic_fetch_and(obj, val)     foreign "C" atomic_fetch_and(obj, val)
#define matomic_fetch_or(obj, val)      foreign "C" atomic_fetch_or(obj, val)
#define matomic_fetch_xor(obj, val)     foreign "C" atomic_fetch_xor(obj, val)
#define matomic_compare_exchange_strong(obj, exp, val) foreign "C" atomic_compare_exchange_strong(obj, exp, val)
#define matomic_compare_exchange_weak(obj, exp, val) foreign "C" atomic_compare_exchange_weak(obj, exp, val)
#define matomic_flag_test_and_set(f)    foreign "C" atomic_flag_test_and_set(f)
#define matomic_flag_clear(f)           foreign "C" atomic_flag_clear(f)
#define matomic_thread_fence(order)     foreign "C" atomic_thread_fence(order)
#define matomic_signal_fence(order)     foreign "C" atomic_signal_fence(order)

#define mthrd_create(t, func, arg)      foreign "C" thrd_create(t, func, arg)
#define mthrd_join(t, res)              foreign "C" thrd_join(t, res)
#define mthrd_detach(t)                 foreign "C" thrd_detach(t)
#define mthrd_exit(res)                 foreign "C" thrd_exit(res)
#define mthrd_current()                 foreign "C" thrd_current()
#define mthrd_equal(a, b)               foreign "C" thrd_equal(a, b)
#define mthrd_sleep(dur, rem)           foreign "C" thrd_sleep(dur, rem)
#define mthrd_yield()                   foreign "C" thrd_yield()

#define mmtx_init(m, type)              foreign "C" mtx_init(m, type)
#define mmtx_lock(m)                    foreign "C" mtx_lock(m)
#define mmtx_trylock(m)                 foreign "C" mtx_trylock(m)
#define mmtx_unlock(m)                  foreign "C" mtx_unlock(m)
#define mmtx_destroy(m)                 foreign "C" mtx_destroy(m)

#define mcnd_init(c)                    foreign "C" cnd_init(c)
#define mcnd_signal(c)                  foreign "C" cnd_signal(c)
#define mcnd_broadcast(c)               foreign "C" cnd_broadcast(c)
#define mcnd_wait(c, m)                 foreign "C" cnd_wait(c, m)
#define mcnd_destroy(c)                 foreign "C" cnd_destroy(c)

#define mtss_create(key, dtor)          foreign "C" tss_create(key, dtor)
#define mtss_get(key)                   foreign "C" tss_get(key)
#define mtss_set(key, val)              foreign "C" tss_set(key, val)
#define mtss_delete(key)                foreign "C" tss_delete(key)

#define mcall_once(flag, func)          foreign "C" call_once(flag, func)

#define mmbrtoc16(pc16, s, n, ps)   foreign "C" mbrtoc16(pc16, s, n, ps)
#define mc16rtomb(s, c16, ps)       foreign "C" c16rtomb(s, c16, ps)
#define mmbrtoc32(pc32, s, n, ps)   foreign "C" mbrtoc32(pc32, s, n, ps)
#define mc32rtomb(s, c32, ps)       foreign "C" c32rtomb(s, c32, ps)
