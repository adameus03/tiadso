#include "../config.h"

#if CONFIG_USE_DOUBLE_PRECISION == 1
    #define BaseType_t double
    #if CONFIG_USE_AVX512_FMA == 1
    #define Avx512BaseType_t __m512d
    #elif CONFIG_USE_AVX2_FMA == 1
    #define Avx2BaseType_t __m256d
    #endif
#else
    #define BaseType_t float
    #if CONFIG_USE_AVX512_FMA == 1
    #define Avx512BaseType_t __m512
    #elif CONFIG_USE_AVX2_FMA == 1
    #define Avx2BaseType_t __m256
    #endif
#endif