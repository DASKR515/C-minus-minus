/*
 *  rts.h  --  GHC RTS / STG wrapper for Cmm   (companion of stdc--.h)
 *  target : GHC 9.12.4        build : gmm -opt 2 -rts main.cmm -o app
 *
 *  TAGS
 *    [V]  the NAME exists in libHSrts-1.0.3.a (checked with nm on the real GHC 9.12.4
 *         install), or the function linked and ran on your machine.
 *         Signatures are from GHC headers / memory unless the line says "ran".
 *    [N]  NOT found in libHSrts-1.0.3.a. It may live in another GHC library
 *         (isDouble* linked fine for you) or may not exist. Use at your own risk.
 *    [U]  not checked at all.
 *
 *  RULES
 *   1. An unused  import  costs nothing. Only functions you actually call must link.
 *   2. r_*  macros = foreign "C".  Use:   (x) = r_isnan(d);     or   r_free(p);
 *      Pointer parameters get "ptr" INSIDE the macro. Do not write "ptr" yourself,
 *      except in the variadic ones (r_msg, r_err, r_barf): there you write  fmt "ptr".
 *   3. K_*  macros = Cmm calling convention (call / jump)  [U]. Primops and closures
 *      need an initialised RTS context (BaseReg, Hp, Sp). They are NOT safe from a
 *      bare main(). See section 4.
 *   4. Cmm integer comparisons (< > <= >=) are UNSIGNED. For signed use %lts %gts ...
 *   5. Initialise every local variable at the top of the function (liveness panic).
 *   6. Do not pass float64 to printf-like variadics. Print the bits (R_D2BITS).
 */

#pragma once
#include "Cmm.h"


/* =====================================================================
 * 1. C FUNCTIONS EXPORTED BY THE RTS   (link with -rts)
 * ===================================================================== */

/* --- floating point classification --- */
/* NOT in libHSrts (nm), but isDouble* linked and ran for you under -rts,
   so gmm pulls them from another GHC library. Names confirmed by running. */
import "C" isDoubleNaN;            /* [V] ran. StgInt f(StgDouble) */
import "C" isDoubleInfinite;       /* [V] ran */
import "C" isDoubleFinite;         /* [V] ran */
import "C" isDoubleDenormalized;   /* [V] ran */
import "C" isDoubleNegativeZero;   /* [V] ran */
import "C" isFloatNaN;             /* [N] not in libHSrts, never tested */
import "C" isFloatInfinite;        /* [N] */
import "C" isFloatFinite;          /* [N] */
import "C" isFloatDenormalized;    /* [N] */
import "C" isFloatNegativeZero;    /* [N] */
import "C" rintDouble;             /* [N] */
import "C" rintFloat;              /* [N] */

/* --- encode / decode (StgPrimFloat.c) --- */
import "C" __int_encodeDouble;     /* [V] ran. StgDouble f(I_ j, I_ e)  = j * 2^e */
import "C" __word_encodeDouble;    /* [V] StgDouble f(W_ j, I_ e) */
import "C" __int_encodeFloat;      /* [V] StgFloat  f(I_ j, I_ e) */
import "C" __word_encodeFloat;     /* [V] StgFloat  f(W_ j, I_ e) */
import "C" __2Int_encodeDouble;    /* [N] private in StgPrimFloat.h, not exported */
import "C" __decodeDouble_Int64;   /* [V] ran. StgInt f(StgInt64 *mant, StgDouble d); RETURNS the exponent */
import "C" __decodeDouble_2Int;    /* [V] void f(I_ *sign, W_ *hi, W_ *lo, I_ *exp, StgDouble d) */
import "C" __decodeFloat_Int;      /* [V] void f(I_ *man, I_ *exp, StgFloat f) */
import "C" hs_mulIntMayOflo;       /* [V] name only */
import "C" hs_word2float32;        /* [V] name only */
import "C" hs_word2float64;        /* [V] name only */

/* --- bit tricks (rts/prim/*.c) --- [V] all names exist */
import "C" hs_popcnt8;  import "C" hs_popcnt16;  import "C" hs_popcnt32;  import "C" hs_popcnt64;  
import "C" hs_clz8;  import "C" hs_clz16;  import "C" hs_clz32;  import "C" hs_clz64;  
import "C" hs_ctz8;  import "C" hs_ctz16;  import "C" hs_ctz32;  import "C" hs_ctz64;  
import "C" hs_bswap16;  import "C" hs_bswap32;  import "C" hs_bswap64;  
import "C" hs_bitrev8;  import "C" hs_bitrev16;  import "C" hs_bitrev32;  import "C" hs_bitrev64;  
import "C" hs_pdep8;  import "C" hs_pdep16;  import "C" hs_pdep32;  import "C" hs_pdep64;  
import "C" hs_pext8;  import "C" hs_pext16;  import "C" hs_pext32;  import "C" hs_pext64;  

/* --- atomics (rts/prim/atomic.c) --- [V] all names exist. p = address */
import "C" hs_atomic_add8;  import "C" hs_atomic_add16;  import "C" hs_atomic_add32;  import "C" hs_atomic_add64;  
import "C" hs_atomic_sub8;  import "C" hs_atomic_sub16;  import "C" hs_atomic_sub32;  import "C" hs_atomic_sub64;  
import "C" hs_atomic_and8;  import "C" hs_atomic_and16;  import "C" hs_atomic_and32;  import "C" hs_atomic_and64;  
import "C" hs_atomic_or8;  import "C" hs_atomic_or16;  import "C" hs_atomic_or32;  import "C" hs_atomic_or64;  
import "C" hs_atomic_xor8;  import "C" hs_atomic_xor16;  import "C" hs_atomic_xor32;  import "C" hs_atomic_xor64;  
import "C" hs_atomic_nand8;  import "C" hs_atomic_nand16;  import "C" hs_atomic_nand32;  import "C" hs_atomic_nand64;  
import "C" hs_cmpxchg8;  import "C" hs_cmpxchg16;  import "C" hs_cmpxchg32;  import "C" hs_cmpxchg64;  
import "C" hs_xchg8;  import "C" hs_xchg16;  import "C" hs_xchg32;  import "C" hs_xchg64;  
import "C" hs_atomicread8;  import "C" hs_atomicread16;  import "C" hs_atomicread32;  import "C" hs_atomicread64;  
import "C" hs_atomicwrite8;  import "C" hs_atomicwrite16;  import "C" hs_atomicwrite32;  import "C" hs_atomicwrite64;  

/* --- time / system --- [V] names exist */
import "C" getMonotonicNSec;       /* StgWord64 f(void), ns */
import "C" getProcessCPUTime;
import "C" getProcessElapsedTime;
import "C" getCurrentThreadCPUTime;
import "C" getUnixEpochTime;
import "C" getPageSize;
import "C" getPhysicalMemorySize;
import "C" getNumberOfProcessors;
import "C" getPageFaults;

/* --- messages / exit --- [V] */
import "C" debugBelch;             /* printf-like, stderr */
import "C" errorBelch;
import "C" barf;                   /* fatal */
import "C" stg_exit;               /* void f(int) */
import "C" shutdownHaskellAndExit; /* void f(int, int) */
import "C" traceUserMsg;
import "C" traceUserMarker;

/* --- RTS lifecycle and GC --- [V] names exist. Need hs_init first. */
import "C" hs_init;                /* void f(int *argc, char ***argv) */
import "C" hs_init_ghc;
import "C" hs_init_with_rtsopts;
import "C" hs_exit;
import "C" hs_exit_nowait;
import "C" hs_perform_gc;
import "C" hs_thread_done;
import "C" hs_set_argv;
import "C" hs_free_stable_ptr;
import "C" hs_free_fun_ptr;
import "C" hs_try_putmvar;
import "C" performGC;
import "C" performMajorGC;
import "C" performBlockingMajorGC;
import "C" performTryPutMVar;
/* performMinorGC: [N] not in libHSrts, removed */
import "C" setNumCapabilities;
import "C" getProgArgv;
import "C" getFullProgArgv;
import "C" getProgEnvv;
import "C" setProgArgv;
import "C" getRTSStats;            /* void f(RTSStats*) */
import "C" getRTSStatsEnabled;
import "C" getAllocations;
import "C" setKeepCAFs;
import "C" rts_isThreaded;         /* replaces rtsSupportsBoundThreads */
import "C" rts_isProfiled;
import "C" rts_isDynamic;
import "C" rts_isDebugged;
import "C" rts_isTracing;

/* --- capabilities, threads, memory --- [V] */
import "C" rts_lock;               /* Capability* f(void) */
import "C" rts_unlock;             /* void f(Capability*) */
import "C" rts_getThreadId;
import "C" rts_pause;
import "C" rts_resume;
import "C" rts_unsafeGetMyCapability;
import "C" allocate;               /* StgPtr f(Capability*, W_ words) */
import "C" allocateMightFail;
import "C" allocatePinned;
import "C" dirty_MUT_VAR;
import "C" dirty_MVAR;
import "C" dirty_TVAR;
import "C" dirty_TSO;
import "C" dirty_STACK;
import "C" createIOThread;
import "C" createStrictIOThread;
import "C" scheduleWaitThread;
import "C" yieldThread;
import "C" setThreadLabel;
import "C" getStablePtr;
import "C" deRefStablePtr;
import "C" freeStablePtr;
import "C" freeHaskellFunctionPtr;
import "C" newCAF;

/* --- Haskell value marshalling (RtsAPI.h) --- [V] names exist.
       rts_mkX(Capability*, X) returns HaskellObj; rts_getX(HaskellObj) returns X. */
import "C" rts_mkInt;     import "C" rts_mkInt8;   import "C" rts_mkInt16;  import "C" rts_mkInt32;  import "C" rts_mkInt64;
import "C" rts_mkWord;    import "C" rts_mkWord8;  import "C" rts_mkWord16; import "C" rts_mkWord32; import "C" rts_mkWord64;
import "C" rts_mkChar;    import "C" rts_mkBool;   import "C" rts_mkDouble; import "C" rts_mkFloat;
import "C" rts_mkPtr;     import "C" rts_mkFunPtr; import "C" rts_mkStablePtr; import "C" rts_mkString;
import "C" rts_getInt;    import "C" rts_getInt8;  import "C" rts_getInt16;  import "C" rts_getInt32;  import "C" rts_getInt64;
import "C" rts_getWord;   import "C" rts_getWord8; import "C" rts_getWord16; import "C" rts_getWord32; import "C" rts_getWord64;
import "C" rts_getChar;   import "C" rts_getBool;  import "C" rts_getDouble; import "C" rts_getFloat;
import "C" rts_getPtr;    import "C" rts_getFunPtr; import "C" rts_getStablePtr;
import "C" rts_apply;     import "C" rts_eval;     import "C" rts_evalIO;   import "C" rts_evalLazyIO;
import "C" rts_checkSchedStatus;


/* =====================================================================
 * 2. SHORT MACROS FOR THE C FUNCTIONS ABOVE   (prefix r_)
 *    "ptr" is added automatically to pointer parameters.
 * ===================================================================== */

/* classification: (x) = r_isnan(d);   returns 0/1 */
#define r_isnan(d)           foreign "C" isDoubleNaN(d)
#define r_isinf(d)           foreign "C" isDoubleInfinite(d)
#define r_isfinite(d)        foreign "C" isDoubleFinite(d)
#define r_isdenorm(d)        foreign "C" isDoubleDenormalized(d)
#define r_isnegzero(d)       foreign "C" isDoubleNegativeZero(d)
#define r_fisnan(f)          foreign "C" isFloatNaN(f)
#define r_fisinf(f)          foreign "C" isFloatInfinite(f)
#define r_fisfinite(f)       foreign "C" isFloatFinite(f)
#define r_fisdenorm(f)       foreign "C" isFloatDenormalized(f)
#define r_fisnegzero(f)      foreign "C" isFloatNegativeZero(f)
#define r_rint(d)            foreign "C" rintDouble(d)
#define r_frint(f)           foreign "C" rintFloat(f)

/* encode: (d) = r_enc_i(37, -1);   -> 18.5 */
#define r_enc_i(j, e)        foreign "C" __int_encodeDouble(j, e)
#define r_enc_w(j, e)        foreign "C" __word_encodeDouble(j, e)
#define r_enc_2i(hi, lo, e)  foreign "C" __2Int_encodeDouble(hi, lo, e)
#define r_fenc_i(j, e)       foreign "C" __int_encodeFloat(j, e)
#define r_fenc_w(j, e)       foreign "C" __word_encodeFloat(j, e)

/* decode: (exp) = r_dec64(addr_of_mant, d);   mantissa goes to bits64[addr_of_mant] */
#define r_dec64(mp, d)               foreign "C" __decodeDouble_Int64(mp "ptr", d)
#define r_dec2(sp, hp, lp, ep, d)    foreign "C" __decodeDouble_2Int(sp "ptr", hp "ptr", lp "ptr", ep "ptr", d)
#define r_fdec(mp, ep, f)            foreign "C" __decodeFloat_Int(mp "ptr", ep "ptr", f)

/* bit tricks: (n) = r_popcnt64(x); */
#define r_popcnt8(x)    foreign "C" hs_popcnt8(x)
#define r_popcnt16(x)   foreign "C" hs_popcnt16(x)
#define r_popcnt32(x)   foreign "C" hs_popcnt32(x)
#define r_popcnt64(x)   foreign "C" hs_popcnt64(x)
#define r_clz8(x)       foreign "C" hs_clz8(x)
#define r_clz16(x)      foreign "C" hs_clz16(x)
#define r_clz32(x)      foreign "C" hs_clz32(x)
#define r_clz64(x)      foreign "C" hs_clz64(x)
#define r_ctz8(x)       foreign "C" hs_ctz8(x)
#define r_ctz16(x)      foreign "C" hs_ctz16(x)
#define r_ctz32(x)      foreign "C" hs_ctz32(x)
#define r_ctz64(x)      foreign "C" hs_ctz64(x)
#define r_bswap16(x)    foreign "C" hs_bswap16(x)
#define r_bswap32(x)    foreign "C" hs_bswap32(x)
#define r_bswap64(x)    foreign "C" hs_bswap64(x)
#define r_bitrev8(x)    foreign "C" hs_bitrev8(x)
#define r_bitrev16(x)   foreign "C" hs_bitrev16(x)
#define r_bitrev32(x)   foreign "C" hs_bitrev32(x)
#define r_bitrev64(x)   foreign "C" hs_bitrev64(x)
#define r_pdep8(s, m)   foreign "C" hs_pdep8(s, m)
#define r_pdep16(s, m)  foreign "C" hs_pdep16(s, m)
#define r_pdep32(s, m)  foreign "C" hs_pdep32(s, m)
#define r_pdep64(s, m)  foreign "C" hs_pdep64(s, m)
#define r_pext8(s, m)   foreign "C" hs_pext8(s, m)
#define r_pext16(s, m)  foreign "C" hs_pext16(s, m)
#define r_pext32(s, m)  foreign "C" hs_pext32(s, m)
#define r_pext64(s, m)  foreign "C" hs_pext64(s, m)

/* atomics: (old) = r_add64(addr, 1);   addr is a pointer */
#define r_add32(p, v)        foreign "C" hs_atomic_add32(p "ptr", v)
#define r_add64(p, v)        foreign "C" hs_atomic_add64(p "ptr", v)
#define r_sub32(p, v)        foreign "C" hs_atomic_sub32(p "ptr", v)
#define r_sub64(p, v)        foreign "C" hs_atomic_sub64(p "ptr", v)
#define r_and32(p, v)        foreign "C" hs_atomic_and32(p "ptr", v)
#define r_and64(p, v)        foreign "C" hs_atomic_and64(p "ptr", v)
#define r_or32(p, v)         foreign "C" hs_atomic_or32(p "ptr", v)
#define r_or64(p, v)         foreign "C" hs_atomic_or64(p "ptr", v)
#define r_xor32(p, v)        foreign "C" hs_atomic_xor32(p "ptr", v)
#define r_xor64(p, v)        foreign "C" hs_atomic_xor64(p "ptr", v)
#define r_nand32(p, v)       foreign "C" hs_atomic_nand32(p "ptr", v)
#define r_nand64(p, v)       foreign "C" hs_atomic_nand64(p "ptr", v)
#define r_cas32(p, o, n)     foreign "C" hs_cmpxchg32(p "ptr", o, n)
#define r_cas64(p, o, n)     foreign "C" hs_cmpxchg64(p "ptr", o, n)
#define r_xchg32(p, v)       foreign "C" hs_xchg32(p "ptr", v)
#define r_xchg64(p, v)       foreign "C" hs_xchg64(p "ptr", v)
#define r_aread32(p)         foreign "C" hs_atomicread32(p "ptr")
#define r_aread64(p)         foreign "C" hs_atomicread64(p "ptr")
#define r_awrite32(p, v)     foreign "C" hs_atomicwrite32(p "ptr", v)
#define r_awrite64(p, v)     foreign "C" hs_atomicwrite64(p "ptr", v)

#define r_add8(p, v)        foreign "C" hs_atomic_add8(p "ptr", v)
#define r_add16(p, v)        foreign "C" hs_atomic_add16(p "ptr", v)
#define r_sub8(p, v)        foreign "C" hs_atomic_sub8(p "ptr", v)
#define r_sub16(p, v)        foreign "C" hs_atomic_sub16(p "ptr", v)
#define r_and8(p, v)        foreign "C" hs_atomic_and8(p "ptr", v)
#define r_and16(p, v)        foreign "C" hs_atomic_and16(p "ptr", v)
#define r_or8(p, v)        foreign "C" hs_atomic_or8(p "ptr", v)
#define r_or16(p, v)        foreign "C" hs_atomic_or16(p "ptr", v)
#define r_xor8(p, v)        foreign "C" hs_atomic_xor8(p "ptr", v)
#define r_xor16(p, v)        foreign "C" hs_atomic_xor16(p "ptr", v)
#define r_nand8(p, v)        foreign "C" hs_atomic_nand8(p "ptr", v)
#define r_nand16(p, v)        foreign "C" hs_atomic_nand16(p "ptr", v)
#define r_cas8(p, o, n)     foreign "C" hs_cmpxchg8(p "ptr", o, n)
#define r_xchg8(p, v)       foreign "C" hs_xchg8(p "ptr", v)
#define r_aread8(p)         foreign "C" hs_atomicread8(p "ptr")
#define r_awrite8(p, v)     foreign "C" hs_atomicwrite8(p "ptr", v)
#define r_cas16(p, o, n)     foreign "C" hs_cmpxchg16(p "ptr", o, n)
#define r_xchg16(p, v)       foreign "C" hs_xchg16(p "ptr", v)
#define r_aread16(p)         foreign "C" hs_atomicread16(p "ptr")
#define r_awrite16(p, v)     foreign "C" hs_atomicwrite16(p "ptr", v)

/* system / time / stats */
#define r_cputime()          foreign "C" getProcessCPUTime()
#define r_elapsed()          foreign "C" getProcessElapsedTime()
#define r_thread_cputime()   foreign "C" getCurrentThreadCPUTime()
#define r_epoch()            foreign "C" getUnixEpochTime()
#define r_pagesize()         foreign "C" getPageSize()
#define r_physmem()          foreign "C" getPhysicalMemorySize()
#define r_pagefaults()       foreign "C" getPageFaults()
#define r_allocated()        foreign "C" getAllocations()
#define r_stats(p)           foreign "C" getRTSStats(p "ptr")
#define r_stats_on()         foreign "C" getRTSStatsEnabled()
#define r_stg_exit(code)     foreign "C" stg_exit(code)
#define r_shutdown_exit(c, f) foreign "C" shutdownHaskellAndExit(c, f)
#define r_trace_msg(s)       foreign "C" traceUserMsg(s "ptr")

/* Haskell value marshalling: (obj) = r_mk_int(cap, 42);  (v) = r_get_int(obj); */
#define r_mk_int(cap, x)     foreign "C" rts_mkInt(cap "ptr", x)
#define r_mk_word(cap, x)    foreign "C" rts_mkWord(cap "ptr", x)
#define r_mk_char(cap, x)    foreign "C" rts_mkChar(cap "ptr", x)
#define r_mk_bool(cap, x)    foreign "C" rts_mkBool(cap "ptr", x)
#define r_mk_double(cap, d)  foreign "C" rts_mkDouble(cap "ptr", d)
#define r_mk_string(cap, s)  foreign "C" rts_mkString(cap "ptr", s "ptr")
#define r_get_int(o)         foreign "C" rts_getInt(o "ptr")
#define r_get_word(o)        foreign "C" rts_getWord(o "ptr")
#define r_get_char(o)        foreign "C" rts_getChar(o "ptr")
#define r_get_bool(o)        foreign "C" rts_getBool(o "ptr")
#define r_get_double(o)      foreign "C" rts_getDouble(o "ptr")
#define r_apply(cap, f, a)   foreign "C" rts_apply(cap "ptr", f "ptr", a "ptr")
#define r_stable_get(p)      foreign "C" getStablePtr(p "ptr")
#define r_stable_deref(sp)   foreign "C" deRefStablePtr(sp)
#define r_stable_free2(sp)   foreign "C" freeStablePtr(sp)

/* time */
#define r_nsec()             foreign "C" getMonotonicNSec()

/* messages: you write fmt "ptr" yourself:  r_msg(fmt "ptr", x);   */
#define r_msg(...)           foreign "C" debugBelch(__VA_ARGS__)
#define r_err(...)           foreign "C" errorBelch(__VA_ARGS__)
#define r_barf(...)          foreign "C" barf(__VA_ARGS__)

/* lifecycle / GC / capability */
#define r_hs_init(argc, argv)  foreign "C" hs_init(argc "ptr", argv "ptr")
#define r_hs_exit()            foreign "C" hs_exit()
#define r_gc()                 foreign "C" performMajorGC()
#define r_gc_blocking()        foreign "C" performBlockingMajorGC()
#define r_gc_hs()              foreign "C" hs_perform_gc()
#define r_lock()               foreign "C" rts_lock()
#define r_unlock(cap)          foreign "C" rts_unlock(cap "ptr")
#define r_thread_id(tso)       foreign "C" rts_getThreadId(tso "ptr")
#define r_ncpu()               foreign "C" getNumberOfProcessors()
#define r_set_ncap(n)          foreign "C" setNumCapabilities(n)
#define r_is_threaded()        foreign "C" rts_isThreaded()
#define r_argv(argc, argv)     foreign "C" getProgArgv(argc "ptr", argv "ptr")
#define r_alloc(cap, words)    foreign "C" allocate(cap "ptr", words)
#define r_alloc_pinned(cap, w) foreign "C" allocatePinned(cap "ptr", w)
#define r_free_sptr(sp)        foreign "C" hs_free_stable_ptr(sp)
#define r_dirty_tso(tso)       foreign "C" dirty_TSO(tso "ptr")


/* =====================================================================
 * 3. IEEE 754 BIT HELPERS  (pure Cmm, no import, no link risk)
 * ===================================================================== */

/* double: 1 | 11 | 52 */
#define R_D_SIGN(raw)     (((raw) >> 63) & 1)
#define R_D_EXP(raw)      (((raw) >> 52) & 0x7FF)
#define R_D_FRAC(raw)     ((raw) & 0xFFFFFFFFFFFFF)
#define R_D_ABS(raw)      ((raw) & 0x7FFFFFFFFFFFFFFF)
#define R_D_HI32(raw)     (((raw) >> 32) & 0xFFFFFFFF)
#define R_D_LO32(raw)     ((raw) & 0xFFFFFFFF)
#define R_D_IS_INF(raw)   (R_D_ABS(raw) == 0x7FF0000000000000)
#define R_D_IS_NAN(raw)   (R_D_ABS(raw) >  0x7FF0000000000000)
#define R_D_IS_ZERO(raw)  (R_D_ABS(raw) == 0)
#define R_D_IS_DENORM(raw) ((R_D_EXP(raw) == 0) && (R_D_FRAC(raw) != 0))
#define R_D_BIAS          1023

/* float: 1 | 8 | 23   (raw = 32 bits in a bits64) */
#define R_F_SIGN(raw)     (((raw) >> 31) & 1)
#define R_F_EXP(raw)      (((raw) >> 23) & 0xFF)
#define R_F_FRAC(raw)     ((raw) & 0x7FFFFF)
#define R_F_ABS(raw)      ((raw) & 0x7FFFFFFF)
#define R_F_IS_INF(raw)   (R_F_ABS(raw) == 0x7F800000)
#define R_F_IS_NAN(raw)   (R_F_ABS(raw) >  0x7F800000)
#define R_F_BIAS          127

/* bitcast through a scratch 8-byte buffer (statements, end with ;)
 *   R_BITS2D(buf, raw, d);     d   = float64 with the bits of raw
 *   R_D2BITS(buf, d, raw);     raw = bits64  with the bits of d       */
#define R_BITS2D(buf, raw, d)   bits64[buf] = (raw); d = float64[buf]
#define R_D2BITS(buf, d, raw)   float64[buf] = (d);  raw = bits64[buf]


/* =====================================================================
 * 4. STG SYMBOLS   names checked with nm (see tags); calling them is still [U]
 *    Imports are free. CALLING them is the hard part. Know the three kinds:
 *
 *    (a) primops      stg_*zh     Cmm procedures. Called with  call / jump  (K_ macros).
 *                                 Need a Haskell context: BaseReg, Hp, HpLim, Sp, a TSO.
 *                                 From a bare C main() they will crash or fault.
 *    (b) closures     *_closure   DATA. Use  import CLOSURE.  Take the address only.
 *    (c) info tables  *_info      DATA. Read fields, never call.
 *
 *    The names below follow PrimOps.cmm / Exception.cmm / StgMiscClosures.cmm.
 *    z-encoding: z = "zz", Z = "ZZ", # = "zh", _ = "zu".
 * ===================================================================== */

/* ---- (a) primops: arrays ---- */
import stg_newByteArrayzh;               /* newByteArray#              (W_ n)            */
import stg_newPinnedByteArrayzh;         /* newPinnedByteArray#        (W_ n)            */
import stg_newAlignedPinnedByteArrayzh;  /* (W_ n, W_ align)                             */
import stg_isByteArrayPinnedzh;
import stg_isMutableByteArrayPinnedzh;
import stg_shrinkMutableByteArrayzh;     /* (arr, W_ new_size)                           */
import stg_resizzeMutableByteArrayzh;    /* resizeMutableByteArray#  (z-encoded)         */
import stg_newArrayzh;                   /* newArray#                  (W_ n, init)      */
import stg_newSmallArrayzh;
import stg_unsafeThawArrayzh;
import stg_unsafeThawSmallArrayzh;
import stg_cloneArrayzh;
import stg_cloneMutableArrayzh;
import stg_freezzeArrayzh;               /* freezeArray#  (z-encoded)                    */
import stg_thawArrayzh;
import stg_cloneSmallArrayzh;
import stg_cloneSmallMutableArrayzh;
import stg_freezzeSmallArrayzh;
import stg_thawSmallArrayzh;
import stg_casArrayzh;
import stg_casSmallArrayzh;

/* ---- primops: mutable variables, MVar, TVar, STM ---- */
import stg_newMutVarzh;
import stg_atomicModifyMutVar2zh;
import stg_atomicModifyMutVarzuzh;
import stg_casMutVarzh;
import stg_newMVarzh;
import stg_takeMVarzh;
import stg_putMVarzh;
import stg_readMVarzh;
import stg_tryTakeMVarzh;
import stg_tryPutMVarzh;
import stg_tryReadMVarzh;
import stg_isEmptyMVarzh;
import stg_newTVarzh;
import stg_readTVarzh;
import stg_readTVarIOzh;
import stg_writeTVarzh;
import stg_atomicallyzh;
import stg_retryzh;
import stg_catchRetryzh;
import stg_catchSTMzh;
import stg_newIOPortzh;
import stg_readIOPortzh;
import stg_writeIOPortzh;

/* ---- primops: exceptions ---- */
import stg_catchzh;
import stg_raisezh;
import stg_raiseIOzh;
import stg_raiseUnderflowzh;
import stg_raiseOverflowzh;
import stg_raiseDivZZerozh;              /* raiseDivZero#  (ZZ = Z)                      */
import stg_maskAsyncExceptionszh;
import stg_unmaskAsyncExceptionszh;
import stg_maskUninterruptiblezh;
import stg_getMaskingStatezh;

/* ---- primops: threads ---- */
import stg_forkzh;
import stg_forkOnzh;
import stg_yieldzh;
import stg_labelThreadzh;
import stg_isCurrentThreadBoundzh;
import stg_noDuplicatezh;
import stg_threadStatuszh;
import stg_killThreadzh;
import stg_delayzh;
import stg_waitReadzh;
import stg_waitWritezh;
import stg_setThreadAllocationCounterzh;

/* ---- primops: stable / weak / misc ---- */
import stg_makeStablePtrzh;
import stg_deRefStablePtrzh;
import stg_makeStableNamezh;
import stg_mkWeakzh;
import stg_mkWeakNoFinalizzerzh;         /* mkWeakNoFinalizer#  (z-encoded)              */
import stg_deRefWeakzh;
import stg_finalizzeWeakzh;              /* finalizeWeak#  (z-encoded)                   */
import stg_addCFinalizzerToWeakzh;
import stg_keepAlivezh;
import stg_traceEventzh;
import stg_traceMarkerzh;
import stg_getSparkzh;
import stg_numSparkszh;
import stg_newBCOzh;
import stg_mkApUpd0zh;
import stg_unpackClosurezh;
import stg_getApStackValzh;

/* ---- primops: delimited continuations (9.6+) ---- */
import stg_newPromptTagzh;
import stg_promptzh;
import stg_control0zh;


/* ---- more primops that exist in 9.12.4 (nm) [V] names ---- */
import stg_castDoubleToWord64zh;         /* castDoubleToWord64#  (the real name; stg_doubleToWord64zh does NOT exist) */
import stg_castWord64ToDoublezh;
import stg_castFloatToWord32zh;
import stg_castWord32ToFloatzh;
import stg_decodeDoublezu2Intzh;         /* decodeDouble_2Int# */
import stg_decodeDoublezuInt64zh;        /* decodeDouble_Int64# */
import stg_decodeFloatzuIntzh;           /* decodeFloat_Int# */
import stg_copyArrayzh;                  import stg_copyMutableArrayzh;
import stg_copySmallArrayzh;             import stg_copySmallMutableArrayzh;
import stg_shrinkSmallMutableArrayzh;
import stg_casIntArrayzh;                import stg_casInt8Arrayzh;   import stg_casInt16Arrayzh;
import stg_casInt32Arrayzh;              import stg_casInt64Arrayzh;
import stg_isByteArrayWeaklyPinnedzh;    import stg_isMutableByteArrayWeaklyPinnedzh;
import stg_closureSizzezh;               /* closureSize# */
import stg_listThreadszh;                import stg_threadLabelzh;
import stg_getThreadAllocationCounterzh; import stg_whereFromzh;
import stg_traceBinaryEventzh;           import stg_clearCCSzh;
import stg_paniczh;                      import stg_absentErrorzh;
import stg_compactNewzh;                 import stg_compactAddzh;     import stg_compactAddWithSharingzh;
import stg_compactSizzezh;               import stg_compactResizzezh; import stg_compactContainszh;
import stg_compactContainsAnyzh;         import stg_compactGetFirstBlockzh; import stg_compactGetNextBlockzh;
import stg_compactAllocateBlockzh;       import stg_compactFixupPointerszh; import stg_compactAddWorkerzh;

/* ---- more apply / return / block / scheduler entry points [V] names ---- */
import stg_ap_pv_fast;  import stg_ap_ppv_fast;  import stg_ap_pppv_fast;
import stg_ap_pppp_fast; import stg_ap_ppppp_fast; import stg_ap_pppppp_fast;
import stg_ap_v16_fast; import stg_ap_v32_fast;  import stg_ap_v64_fast;
import stg_ap_ppp_info;  import stg_ap_pppp_info;
import stg_AP_info;     import stg_AP_NOUPD_info; import stg_AP_STACK_info; import stg_PAP_info;
import stg_BCO_info;    import stg_IND_STATIC_info;
import stg_ret_v_info;  import stg_ret_p_info;  import stg_ret_n_info;
import stg_ret_d_info;  import stg_ret_f_info;  import stg_ret_l_info;  import stg_ret_t_info;
import stg_block_takemvar; import stg_block_putmvar; import stg_block_readmvar;
import stg_block_noregs;   import stg_block_blackhole; import stg_block_throwto; import stg_block_stmwait;
import stg_returnToSched;  import stg_returnToSchedNotPaused; import stg_returnToSchedButFirst;
import stg_yield_noregs;   import stg_yield_to_interpreter;
import stg_killMyself;     import stg_threadFinished; import stg_abort;
import stg_MSG_BLACKHOLE_info; import stg_MSG_THROWTO_info; import stg_MSG_TRY_WAKEUP_info;
import stg_BLOCKING_QUEUE_CLEAN_info; import stg_BLOCKING_QUEUE_DIRTY_info;
import stg_PROMPT_TAG_info; import stg_prompt_frame_info; import stg_CONTINUATION_info;
import stg_MUT_ARR_PTRS_FROZEN_DIRTY_info; import stg_SMALL_MUT_ARR_PTRS_DIRTY_info;
import stg_SMALL_MUT_ARR_PTRS_FROZEN_CLEAN_info; import stg_SMALL_MUT_ARR_PTRS_FROZEN_DIRTY_info;
import stg_TVAR_DIRTY_info; import stg_TREC_HEADER_info; import stg_COMPACT_NFDATA_CLEAN_info;

/* ---- generic apply (ap) and heap-check / GC entry points ---- */
import stg_ap_0_fast;   import stg_ap_v_fast;   import stg_ap_p_fast;   import stg_ap_pp_fast;
import stg_ap_ppp_fast; import stg_ap_n_fast;   import stg_ap_d_fast;   import stg_ap_f_fast;
import stg_ap_l_fast;
import stg_ap_v_info;   import stg_ap_p_info;   import stg_ap_pp_info;
import stg_ap_n_info;   import stg_ap_d_info;   import stg_ap_f_info;   import stg_ap_l_info;
import stg_gc_noregs;   import stg_gc_unpt_r1;  import stg_gc_unbx_r1;  import stg_gc_pp;
import stg_gc_d1;       import stg_gc_f1;       import stg_gc_l1;       import __stg_gc_enter_1;
import __stg_gc_fun;    import stg_gc_fun_info; import stg_returnToStackTop;

/* ---- (c) info tables: frames and closure types ---- */
import stg_upd_frame_info;
import stg_bh_upd_frame_info;
import stg_enter_info;
import stg_stop_thread_info;
import stg_catch_frame_info;
import stg_catch_retry_frame_info;
import stg_catch_stm_frame_info;
import stg_atomically_frame_info;
import stg_IND_info;
import stg_BLACKHOLE_info;
import stg_CAF_BLACKHOLE_info;
import stg_WHITEHOLE_info;
import stg_ARR_WORDS_info;
import stg_MUT_VAR_CLEAN_info;
import stg_MUT_VAR_DIRTY_info;
import stg_MVAR_CLEAN_info;
import stg_MVAR_DIRTY_info;
import stg_TVAR_CLEAN_info;
import stg_MUT_ARR_PTRS_CLEAN_info;
import stg_MUT_ARR_PTRS_DIRTY_info;
import stg_MUT_ARR_PTRS_FROZEN_CLEAN_info;
import stg_SMALL_MUT_ARR_PTRS_CLEAN_info;
import stg_WEAK_info;
import stg_DEAD_WEAK_info;
import stg_STABLE_NAME_info;
import stg_TSO_info;
import stg_STACK_info;
import stg_TREC_CHUNK_info;

/* ---- (b) closures (data) ---- */
import CLOSURE stg_END_TSO_QUEUE_closure;
import CLOSURE stg_NO_FINALIZER_closure;
import CLOSURE stg_NO_TREC_closure;
import CLOSURE stg_END_STM_WATCH_QUEUE_closure;
import CLOSURE stg_END_STM_CHUNK_LIST_closure;
import CLOSURE stg_INTLIKE_closure;
import CLOSURE stg_CHARLIKE_closure;
import CLOSURE stg_STM_AWOKEN_closure;
import CLOSURE stg_dummy_ret_closure;


/* =====================================================================
 * 5. CMM CALLING CONVENTION   (prefix K_)   [U]
 *    K_CALL(res, f, args...)   (res) = call f(args);    returns to you
 *    K_CALLV(f, args...)       call f(args);            no result
 *    K_JUMP(f, args...)        jump f(args);            tail call, never returns
 *    Example (only valid inside a Haskell/RTS context):
 *        K_CALL(arr, stg_newByteArrayzh, 64);
 * ===================================================================== */
#define K_CALL(res, f, ...)    (res) = call f(__VA_ARGS__)
#define K_CALLV(f, ...)        call f(__VA_ARGS__)
#define K_JUMP(f, ...)         jump f(__VA_ARGS__)

/* a few named shortcuts (still [U], same restrictions) */
#define K_new_bytes(res, n)          K_CALL(res, stg_newByteArrayzh, n)
#define K_new_pinned(res, n)         K_CALL(res, stg_newPinnedByteArrayzh, n)
#define K_new_array(res, n, init)    K_CALL(res, stg_newArrayzh, n, init)
#define K_new_mutvar(res, v)         K_CALL(res, stg_newMutVarzh, v)
#define K_new_mvar(res)              K_CALL(res, stg_newMVarzh)
#define K_yield()                    K_CALLV(stg_yieldzh)
#define K_make_sptr(res, v)          K_CALL(res, stg_makeStablePtrzh, v)
