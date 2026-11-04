# rts.h wiki
{ by ALEXIS }

Status tags: **V** = the name exists in `libHSrts-1.0.3.a` (checked with `nm`) or the function ran correctly. **N** = not found in the RTS library. **U** = not checked.
Signatures come from GHC headers and memory unless a row says it ran.
Types: `W` = word (bits64), `I` = signed word, `D` = float64, `F` = float32, `ptr` = pointer.

## How to call

```c
(result) = r_name(args);   // function with a result
r_name(args);              // no result, or result ignored
```

Do not write `"ptr"` yourself in `r_` macros. Exception: `r_msg`, `r_err`, `r_barf`
(variadic), where you write `fmt "ptr"`.

---

## 1. Floating-point classification

All return `0` or `1` in a word.

| Macro | C function | Args | Status |
|---|---|---|---|
| `r_isnan(d)` | `isDoubleNaN` | D | V (ran) |
| `r_isinf(d)` | `isDoubleInfinite` | D | V (ran) |
| `r_isfinite(d)` | `isDoubleFinite` | D | V (ran) |
| `r_isdenorm(d)` | `isDoubleDenormalized` | D | V (ran) |
| `r_isnegzero(d)` | `isDoubleNegativeZero` | D | V (ran) |
| `r_fisnan(f)` | `isFloatNaN` | F | N (not in libHSrts) |
| `r_fisinf(f)` | `isFloatInfinite` | F | N (not in libHSrts) |
| `r_fisfinite(f)` | `isFloatFinite` | F | N (not in libHSrts) |
| `r_fisdenorm(f)` | `isFloatDenormalized` | F | N (not in libHSrts) |
| `r_fisnegzero(f)` | `isFloatNegativeZero` | F | N (not in libHSrts) |
| `r_rint(d)` | `rintDouble` | D, returns D | N (not in libHSrts) |
| `r_frint(f)` | `rintFloat` | F, returns F | N (not in libHSrts) |

## 2. Encode (build a float from integer parts)

Result is `j * 2^e`.

| Macro | C function | Args | Returns | Status |
|---|---|---|---|---|
| `r_enc_i(j, e)` | `__int_encodeDouble` | I, I | D | V (ran: 37, -1 gives 18.5) |
| `r_enc_w(j, e)` | `__word_encodeDouble` | W, I | D | V (source) |
| `r_enc_2i(hi, lo, e)` | `__2Int_encodeDouble` | I, I, I | D | N (private, not exported) |
| `r_fenc_i(j, e)` | `__int_encodeFloat` | I, I | F | V (source) |
| `r_fenc_w(j, e)` | `__word_encodeFloat` | W, I | F | V (source) |

## 3. Decode (split a float into mantissa and exponent)

| Macro | C function | Notes | Status |
|---|---|---|---|
| `r_dec64(mp, d)` | `__decodeDouble_Int64` | Writes the 64-bit mantissa to `bits64[mp]`. **Returns the exponent.** Value = mantissa * 2^exp. | V (ran) |
| `r_dec2(sp, hp, lp, ep, d)` | `__decodeDouble_2Int` | Writes sign (+1/-1), high 32 bits, low 32 bits of the mantissa, and exponent to four pointers. No return value. | V (source) |
| `r_fdec(mp, ep, f)` | `__decodeFloat_Int` | Writes mantissa and exponent for a float32. | V (source) |

```c
(ex) = r_dec64(buf + 8, d);
// mantissa: bits64[buf + 8], exponent: ex
```

Observed: 3.7 gives mantissa 8331659310635418, exponent -51.

## 4. Bit tricks (`rts/prim/*.c`) - all U

All take and return a word. Widths 8, 16, 32, 64 unless noted.

| Macros | Operation |
|---|---|
| `r_popcnt8/16/32/64(x)` | population count |
| `r_clz8/16/32/64(x)` | count leading zeros |
| `r_ctz8/16/32/64(x)` | count trailing zeros |
| `r_bswap16/32/64(x)` | byte swap (no 8-bit version) |
| `r_bitrev8/16/32/64(x)` | reverse bits |
| `r_pdep8/16/32/64(src, mask)` | parallel bit deposit |
| `r_pext8/16/32/64(src, mask)` | parallel bit extract |

## 5. Atomics (`rts/prim/atomic.c`) - all U

`p` is a pointer to memory. Widths 32 and 64 are wrapped. The 8 and 16-bit versions
exist in the RTS under the same pattern but have no macros yet.

| Macro | Operation | Returns |
|---|---|---|
| `r_add32/64(p, v)` | `*p += v` | old value |
| `r_sub32/64(p, v)` | `*p -= v` | old value |
| `r_and32/64(p, v)` | `*p &= v` | old value |
| `r_or32/64(p, v)` | `*p \|= v` | old value |
| `r_xor32/64(p, v)` | `*p ^= v` | old value |
| `r_nand32/64(p, v)` | `*p = ~(*p & v)` | old value |
| `r_cas32/64(p, old, new)` | compare and swap | value read from `*p` |
| `r_xchg32/64(p, v)` | exchange | old value |
| `r_aread32/64(p)` | atomic read | value |
| `r_awrite32/64(p, v)` | atomic write | none |

## 6. Time - U

| Macro | C function | Returns |
|---|---|---|
| `r_nsec()` | `getMonotonicNSec` | monotonic time in nanoseconds (64-bit) |

## 7. Messages - U

Variadic. You write `"ptr"` on the format string yourself.

| Macro | C function | Notes |
|---|---|---|
| `r_msg(fmt "ptr", ...)` | `debugBelch` | printf-like, to stderr |
| `r_err(fmt "ptr", ...)` | `errorBelch` | error message |
| `r_barf(fmt "ptr", ...)` | `barf` | fatal, does not return |

## 8. RTS lifecycle, GC, capabilities, threads

These need an initialised RTS (`r_hs_init` first) unless noted.

| Macro | C function | Notes | Status |
|---|---|---|---|
| `r_hs_init(argc, argv)` | `hs_init` | pointers to argc and argv | U |
| `r_hs_exit()` | `hs_exit` | shut the RTS down | U |
| `r_gc()` | `performMajorGC` | major collection | U |
| `r_gc_blocking()` | `performBlockingMajorGC` | blocking major collection | V |
| `r_gc_hs()` | `hs_perform_gc` | the FFI-level GC entry | U |
| `r_lock()` | `rts_lock` | returns a Capability pointer | U |
| `r_unlock(cap)` | `rts_unlock` | release it | U |
| `r_thread_id(tso)` | `rts_getThreadId` | thread id of a TSO | V (source) |
| `r_ncpu()` | `getNumberOfProcessors` | | U |
| `r_set_ncap(n)` | `setNumCapabilities` | | U |
| `r_is_threaded()` | `rts_isThreaded` | | V |
| `r_argv(argc, argv)` | `getProgArgv` | program arguments | U |
| `r_alloc(cap, words)` | `allocate` | allocate on the Haskell heap | U |
| `r_alloc_pinned(cap, w)` | `allocatePinned` | pinned allocation | U |
| `r_free_sptr(sp)` | `hs_free_stable_ptr` | release a StablePtr | U |
| `r_dirty_tso(tso)` | `dirty_TSO` | mark a TSO dirty | V (source) |

Imported but without a macro yet: `hs_thread_done`, `hs_free_fun_ptr`, `performGC`, `dirty_MUT_VAR`.

---

## 9. IEEE 754 bit macros (pure Cmm, no linking)

`raw` is the bit pattern in a `bits64` variable (for float32, the low 32 bits).

### Double (1 | 11 | 52)

| Macro | Meaning |
|---|---|
| `R_D_SIGN(raw)` | sign bit (0 or 1) |
| `R_D_EXP(raw)` | biased exponent (0 to 2047) |
| `R_D_FRAC(raw)` | 52-bit fraction |
| `R_D_ABS(raw)` | bits with the sign cleared |
| `R_D_HI32(raw)` / `R_D_LO32(raw)` | upper / lower 32 bits |
| `R_D_IS_INF(raw)` | true for +inf and -inf |
| `R_D_IS_NAN(raw)` | true for any NaN |
| `R_D_IS_ZERO(raw)` | true for +0 and -0 |
| `R_D_IS_DENORM(raw)` | exponent 0 and fraction non-zero |
| `R_D_BIAS` | 1023 |

### Float (1 | 8 | 23)

`R_F_SIGN`, `R_F_EXP`, `R_F_FRAC`, `R_F_ABS`, `R_F_IS_INF`, `R_F_IS_NAN`, `R_F_BIAS` (127).
Same meaning as the double versions.

### Bitcast through memory

These are statements. End them with `;`. `buf` is an address of at least 8 bytes.

| Macro | Effect |
|---|---|
| `R_BITS2D(buf, raw, d)` | `d` becomes the float64 with the bits of `raw` |
| `R_D2BITS(buf, d, raw)` | `raw` becomes the bits of `d` |

```c
R_BITS2D(buf, 0x400d99999999999a, d);   // d = 3.7
R_D2BITS(buf, d, raw);                  // raw = 0x400d99999999999a
```

---

## 10. STG symbols (section 4 of rts.h)

All names below were checked against `nm` of the real library (V). How to call them is still untested (U).
Note: `stg_doubleToWord64zh` does not exist. The real names are `stg_castDoubleToWord64zh`, `stg_castWord64ToDoublezh`, `stg_castFloatToWord32zh`, `stg_castWord32ToFloatzh`.

Imports are always safe. Calling is the hard part.

| Kind | Naming | How to use | Safe from a bare `main`? |
|---|---|---|---|
| Primop | `stg_*zh` | `K_CALL` / `K_JUMP` | No, needs a Haskell context |
| Closure | `*_closure` | `import CLOSURE`, take the address | Address only |
| Info table | `*_info` | read fields, never call | Read only |

Name encoding: `#` is `zh`, `z` is `zz`, `Z` is `ZZ`, `_` is `zu`.
Example: `freezeArray#` is `stg_freezzeArrayzh`.

### Primops imported

- **Byte arrays:** `stg_newByteArrayzh`, `stg_newPinnedByteArrayzh`, `stg_newAlignedPinnedByteArrayzh`, `stg_isByteArrayPinnedzh`, `stg_isMutableByteArrayPinnedzh`, `stg_shrinkMutableByteArrayzh`, `stg_resizzeMutableByteArrayzh`
- **Arrays:** `stg_newArrayzh`, `stg_newSmallArrayzh`, `stg_unsafeThawArrayzh`, `stg_unsafeThawSmallArrayzh`, `stg_cloneArrayzh`, `stg_cloneMutableArrayzh`, `stg_freezzeArrayzh`, `stg_thawArrayzh`, `stg_cloneSmallArrayzh`, `stg_cloneSmallMutableArrayzh`, `stg_freezzeSmallArrayzh`, `stg_thawSmallArrayzh`, `stg_casArrayzh`, `stg_casSmallArrayzh`
- **MutVar:** `stg_newMutVarzh`, `stg_atomicModifyMutVar2zh`, `stg_atomicModifyMutVarzuzh`, `stg_casMutVarzh`
- **MVar:** `stg_newMVarzh`, `stg_takeMVarzh`, `stg_putMVarzh`, `stg_readMVarzh`, `stg_tryTakeMVarzh`, `stg_tryPutMVarzh`, `stg_tryReadMVarzh`, `stg_isEmptyMVarzh`
- **STM and IOPort:** `stg_newTVarzh`, `stg_readTVarzh`, `stg_readTVarIOzh`, `stg_writeTVarzh`, `stg_atomicallyzh`, `stg_retryzh`, `stg_catchRetryzh`, `stg_catchSTMzh`, `stg_newIOPortzh`, `stg_readIOPortzh`, `stg_writeIOPortzh`
- **Exceptions:** `stg_catchzh`, `stg_raisezh`, `stg_raiseIOzh`, `stg_raiseUnderflowzh`, `stg_raiseOverflowzh`, `stg_raiseDivZZerozh`, `stg_maskAsyncExceptionszh`, `stg_unmaskAsyncExceptionszh`, `stg_maskUninterruptiblezh`, `stg_getMaskingStatezh`
- **Threads:** `stg_forkzh`, `stg_forkOnzh`, `stg_yieldzh`, `stg_labelThreadzh`, `stg_isCurrentThreadBoundzh`, `stg_noDuplicatezh`, `stg_threadStatuszh`, `stg_killThreadzh`, `stg_delayzh`, `stg_waitReadzh`, `stg_waitWritezh`, `stg_setThreadAllocationCounterzh`
- **Stable, weak, misc:** `stg_makeStablePtrzh`, `stg_deRefStablePtrzh`, `stg_makeStableNamezh`, `stg_mkWeakzh`, `stg_mkWeakNoFinalizzerzh`, `stg_deRefWeakzh`, `stg_finalizzeWeakzh`, `stg_addCFinalizzerToWeakzh`, `stg_keepAlivezh`, `stg_traceEventzh`, `stg_traceMarkerzh`, `stg_getSparkzh`, `stg_numSparkszh`, `stg_newBCOzh`, `stg_mkApUpd0zh`, `stg_unpackClosurezh`, `stg_getApStackValzh`
- **Delimited continuations:** `stg_newPromptTagzh`, `stg_promptzh`, `stg_control0zh`

### Generic apply and GC entry points

- Apply (fast): `stg_ap_0_fast`, `stg_ap_v_fast`, `stg_ap_p_fast`, `stg_ap_pp_fast`, `stg_ap_ppp_fast`, `stg_ap_n_fast`, `stg_ap_d_fast`, `stg_ap_f_fast`, `stg_ap_l_fast`
- Apply (info): `stg_ap_0_info`, `stg_ap_v_info`, `stg_ap_p_info`, `stg_ap_pp_info`, `stg_ap_n_info`, `stg_ap_d_info`, `stg_ap_f_info`, `stg_ap_l_info`
- GC / heap check: `stg_gc_noregs`, `stg_gc_unpt_r1`, `stg_gc_unbx_r1`, `stg_gc_pp`, `stg_gc_d1`, `stg_gc_f1`, `stg_gc_l1`, `stg_gc_enter_1`, `stg_gc_fun`, `stg_returnToStackTop`

### Info tables

`stg_upd_frame_info`, `stg_bh_upd_frame_info`, `stg_enter_info`, `stg_stop_thread_info`,
`stg_catch_frame_info`, `stg_catch_retry_frame_info`, `stg_catch_stm_frame_info`,
`stg_atomically_frame_info`, `stg_IND_info`, `stg_BLACKHOLE_info`, `stg_CAF_BLACKHOLE_info`,
`stg_WHITEHOLE_info`, `stg_ARR_WORDS_info`, `stg_MUT_VAR_CLEAN_info`, `stg_MUT_VAR_DIRTY_info`,
`stg_MVAR_CLEAN_info`, `stg_MVAR_DIRTY_info`, `stg_TVAR_CLEAN_info`,
`stg_MUT_ARR_PTRS_CLEAN_info`, `stg_MUT_ARR_PTRS_DIRTY_info`, `stg_MUT_ARR_PTRS_FROZEN_CLEAN_info`,
`stg_SMALL_MUT_ARR_PTRS_CLEAN_info`, `stg_WEAK_info`, `stg_DEAD_WEAK_info`,
`stg_STABLE_NAME_info`, `stg_TSO_info`, `stg_STACK_info`, `stg_TREC_CHUNK_info`

### Closures

`stg_END_TSO_QUEUE_closure`, `stg_NO_FINALIZER_closure`, `stg_NO_TREC_closure`,
`stg_END_STM_WATCH_QUEUE_closure`, `stg_END_STM_CHUNK_LIST_closure`,
`stg_INTLIKE_closure`, `stg_CHARLIKE_closure`, `stg_MSG_NULL_closure`

---

## 11. Cmm calling convention macros - all U

| Macro | Expands to | Use |
|---|---|---|
| `K_CALL(res, f, args...)` | `(res) = call f(args)` | call and get a result |
| `K_CALLV(f, args...)` | `call f(args)` | call, no result |
| `K_JUMP(f, args...)` | `jump f(args)` | tail call, never returns |

Shortcuts (same restrictions):

| Macro | Calls |
|---|---|
| `K_new_bytes(res, n)` | `stg_newByteArrayzh` |
| `K_new_pinned(res, n)` | `stg_newPinnedByteArrayzh` |
| `K_new_array(res, n, init)` | `stg_newArrayzh` |
| `K_new_mutvar(res, v)` | `stg_newMutVarzh` |
| `K_new_mvar(res)` | `stg_newMVarzh` |
| `K_yield()` | `stg_yieldzh` |
| `K_make_sptr(res, v)` | `stg_makeStablePtrzh` |

---

## Rules to remember

1. Initialise every local variable at the top of the function.
2. Do not write `"ptr"` in `r_` macros, except the variadic message ones.
3. Do not pass `float64` to `printf`. Print bits with `R_D2BITS`.
4. Cmm `<`, `>`, `<=`, `>=` on integers are believed to be unsigned (unconfirmed). Use `%lts`, `%gts` for signed.
5. Primops and closures are not callable from a bare `main`.
6. If a symbol does not link, remove or fix its name. Nothing else breaks.

---

## 12. Added after checking `nm` (all names V)

- **System and time:** `r_cputime`, `r_elapsed`, `r_thread_cputime`, `r_epoch`, `r_pagesize`, `r_physmem`, `r_pagefaults`, `r_allocated`
- **Stats and exit:** `r_stats`, `r_stats_on`, `r_stg_exit`, `r_shutdown_exit`, `r_trace_msg`
- **Haskell value marshalling:** `r_mk_int`, `r_mk_word`, `r_mk_char`, `r_mk_bool`, `r_mk_double`, `r_mk_string`, `r_get_int`, `r_get_word`, `r_get_char`, `r_get_bool`, `r_get_double`, `r_apply`
- **StablePtr:** `r_stable_get`, `r_stable_deref`, `r_stable_free2`
- **8 and 16-bit atomics:** `r_add8/16`, `r_sub8/16`, `r_and8/16`, `r_or8/16`, `r_xor8/16`, `r_nand8/16`, `r_cas8/16`, `r_xchg8/16`, `r_aread8/16`, `r_awrite8/16`
- **Imported only (no macro):** the rest of the `rts_mk*` / `rts_get*` family, `rts_eval*`, `rts_pause`, `rts_resume`, `hs_init_ghc`, `hs_try_putmvar`, `createIOThread`, `scheduleWaitThread`, compact-region primops, `stg_cast*`, `stg_decodeDouble*`, `stg_copy*Arrayzh`, extra apply, block and return entry points.
