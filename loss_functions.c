#include "loss_functions.h"
#include "config.h"
#include "commons/log.h"

static const char* TAG = "loss_functions";

// inspired by https://stackoverflow.com/questions/49941645/get-sum-of-values-stored-in-m256d-with-sse-avx
double loss_functions_aggregation_helpers_hsum512d(__m512d v) {
    __m256d vl256 = _mm512_castpd512_pd256(v);
    __m256d vh256 = _mm512_extractf64x4_pd(v, 1);
    vl256 = _mm256_add_pd(vl256, vh256);

    __m128d vll128 = _mm256_castpd256_pd128(vl256);
    __m128d vlh128 = _mm256_extractf128_pd(vl256, 1);
    vll128 = _mm_add_pd(vll128, vlh128);

    __m128d vllh64dup = _mm_unpackhi_pd(vll128, vll128);
    return _mm_cvtsd_f64(_mm_add_sd(vll128, vllh64dup));
}

float loss_functions_aggregation_helpers_hsum512(__m512 v) {
    __m256 vl256 = _mm512_castps512_ps256(v);
    //__m256 vh256 = 
    //vl256 = _mm256_add_ps(vl256, vh256);
    TIADSO_LOGE(TAG, "Not implemented!");
    exit(EXIT_FAILURE);
    return 0.0;
}

double loss_functions_aggregation_helpers_hsum256d(__m256d v) {
    __m128d vl128 = _mm256_castpd256_pd128(v);
    __m128d vh128 = _mm256_extractf128_pd(v, 1);
    vl128 = _mm_add_pd(vl128, vh128);

    __m128d vlh64dup = _mm_unpackhi_pd(vl128, vl128);
    return _mm_cvtsd_f64(_mm_add_sd(vl128, vlh64dup));
}

float loss_functions_aggregation_helpers(__m256 v) {
    __m128 vl128 = _mm256_castps256_ps128(v);
    __m128 vh128 = _mm256_extractf128_ps(v, 1);
    vl128 = _mm_add_ps(vl128, vh128);

    /* v
        aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
        bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
        cccccccccccccccccccccccccccccccc
        dddddddddddddddddddddddddddddddd
        eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
        ffffffffffffffffffffffffffffffff
        gggggggggggggggggggggggggggggggg
        hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh

    */

   /* vl128
        aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
        bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb
        cccccccccccccccccccccccccccccccc
        dddddddddddddddddddddddddddddddd

      vh128
        eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
        ffffffffffffffffffffffffffffffff
        gggggggggggggggggggggggggggggggg
        hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
    
   */
  
  /*
    vl128
        [0:31] a+e (f32)
        [32:63] b+f (f32)
        [64:95] c+g (f32)
        [96:127] d+h (f32)
  */

  /*
    vlh64dup
       [0:31] c+g (f32)
       [32:63] c+g (f32)
       [95:64] d+h (f32)
       [96:127] d+h (f32)
  */

 /*
    vll64dup
       [0:31] a+e (f32)
       [32:63] a+e (f32)
       [95:64] b+f (f32)
       [96:127] b+f (f32)
  */
    __m128 vlh64dup = _mm_unpackhi_ps(vl128, vl128);
    __m128 vll64dup = _mm_unpacklo_ps(vl128, vl128);

    __m128 s = _mm_add_ps(vlh64dup, vll64dup);
    /*
    s
       [0:31] a+c+e+g (f32)
       [32:63] a+c+e+g (f32)
       [95:64] b+d+f+h (f32)
       [96:127] b+d+f+h (f32)
  */
    return _mm_extract_ps(s, 0) + _mm_extract_ps(s, 3);
}

BaseType_t loss_functions__sphere(sol_t* pSol) {
    BaseType_t sum = (BaseType_t) 0.0;
    #if CONFIG_USE_AVX512_FMA == 1
        for (uint64_t i = 0; i < pSol->num_coordinate_packets; i++) {
            Avx512BaseType_t* pCoordinatePacket = pSol->pCoordinatePackets + i;
            #if CONFIG_USE_DOUBLE_PRECISION == 1
                Avx512BaseType_t square = _mm512_mul_pd(*pCoordinatePacket, *pCoordinatePacket);
                sum += loss_functions_hsum512d(square);
            #else
                Avx512BaseType_t square = _mm512_mul_ps(*pCoordinatePacket, *pCoordinatePacket);
                sum += loss_functions_hsum512(square);
            #endif
        }
    #else
    #endif
}