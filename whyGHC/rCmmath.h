// -- I absolutely hate this code. I was only testing things, and I don't want to use this approach ever again.


#ifndef R_CMMATH_H
#define R_CMMATH_H

import "C" rintDouble;
import "C" __decodeDouble_2Int;
import "C" __decodeDouble_Int64;
import "C" stg_doubleToWord64zh;
import "C" stg_word64ToDoublezh;
import "C" isDoubleNaN;
import "C" isDoubleInfinite;
import "C" isDoubleFinite;
import "C" isDoubleDenormalized;
import "C" isDoubleNegativeZero;

#define cmm_rint(x)           rintDouble(x)
#define cmm_double_to_bits(d) stg_doubleToWord64zh(d)
#define cmm_bits_to_double(w) stg_word64ToDoublezh(w)
#define cmm_is_nan(d)         isDoubleNaN(d)
#define cmm_is_infinite(d)    isDoubleInfinite(d)
#define cmm_is_finite(d)      isDoubleFinite(d)
#define cmm_is_denormal(d)    isDoubleDenormalized(d)
#define cmm_is_neg_zero(d)    isDoubleNegativeZero(d)

#endif
