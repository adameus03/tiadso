#ifndef __COMMONS_TYPES_H
#define __COMMONS_TYPES_H

#include "../config.h"
#include <immintrin.h>

#if CONFIG_USE_DOUBLE_PRECISION == 1
    typedef double BaseType_t;
    #if CONFIG_USE_AVX512_FMA == 1
    typedef __m512d Avx512BaseType_t
    #elif CONFIG_USE_AVX2_FMA == 1
    typedef __m256d Avx2BaseType_t;
    #endif
#else
    typedef float BaseType_t;
    #if CONFIG_USE_AVX512_FMA == 1
    typedef __m512 Avx512BaseType_t;
    #elif CONFIG_USE_AVX2_FMA == 1
    typedef __m256 Avx2BaseType_t;
    #endif
#endif

#define TIADSO_LOSS_FN(name) BaseType_t (*name)(sol_t*)

#endif