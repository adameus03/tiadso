#include "loss_functions.h"
#include "config.h"
#include "commons/log.h"

static const char* TAG = "loss_functions";

#if CONFIG_USE_AVX512_FMA == 1
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

#elif CONFIG_USE_AVX2_FMA == 1
double loss_functions_aggregation_helpers_hsum256d(__m256d v) {
    __m128d vl128 = _mm256_castpd256_pd128(v);
    __m128d vh128 = _mm256_extractf128_pd(v, 1);
    vl128 = _mm_add_pd(vl128, vh128);

    __m128d vlh64dup = _mm_unpackhi_pd(vl128, vl128);
    return _mm_cvtsd_f64(_mm_add_sd(vl128, vlh64dup));
}

float loss_functions_aggregation_helpers_hsum256(__m256 v) {
    __m128 vl128 = _mm256_castps256_ps128(v);
    __m128 vh128 = _mm256_extractf128_ps(v, 1);
    vl128 = _mm_add_ps(vl128, vh128);

    __m128 vlh64dup = _mm_unpackhi_ps(vl128, vl128);
    __m128 vll64dup = _mm_unpacklo_ps(vl128, vl128);

    __m128 s = _mm_add_ps(vlh64dup, vll64dup);
    return _mm_extract_ps(s, 0) + _mm_extract_ps(s, 3);
}
#endif

BaseType_t loss_functions__sphere(sol_t* pSol) {
    BaseType_t sum = (BaseType_t) 0.0;
#if CONFIG_USE_AVX512_FMA == 1 || CONFIG_USE_AVX2_FMA
    for (uint64_t i = 0; i < pSol->num_coordinate_packets; i++) {
    #if CONFIG_USE_AVX512_FMA == 1
        Avx512BaseType_t* pCoordinatePacket;
        Avx512BaseType_t square;
    #elif CONFIG_USE_AVX2_FMA == 1
        Avx2BaseType_t* pCoordinatePacket;
        Avx2BaseType_t square;
    #endif
        pCoordinatePacket = pSol->pCoordinatePackets + i;

    #if CONFIG_USE_AVX512_FMA == 1
        #if CONFIG_USE_DOUBLE_PRECISION == 1
        square = _mm512_mul_pd(*pCoordinatePacket, *pCoordinatePacket);
        sum += loss_functions_aggregation_helpers_hsum512d(square);
        #else
        square = _mm512_mul_ps(*pCoordinatePacket, *pCoordinatePacket);
        sum += loss_functions_aggregation_helpers_hsum512(square);
        #endif
    #elif CONFIG_USE_AVX2_FMA == 1
        #if CONFIG_USE_DOUBLE_PRECISION == 1
        square = _mm256_mul_pd(*pCoordinatePacket, *pCoordinatePacket);
        square = _mm256_mul_pd(*pCoordinatePacket, *pCoordinatePacket);
        square = _mm256_mul_pd(*pCoordinatePacket, *pCoordinatePacket);
        //TIADSO_LOGI(TAG, "Calling hsum helper");
        sum += loss_functions_aggregation_helpers_hsum256d(square);
        square = _mm256_mul_pd(*pCoordinatePacket, *pCoordinatePacket);
        square = _mm256_mul_pd(*pCoordinatePacket, *pCoordinatePacket);
        square = _mm256_mul_pd(*pCoordinatePacket, *pCoordinatePacket);
        #else
        square = _mm256_mul_ps(*pCoordinatePacket, *pCoordinatePacket);
        sum += loss_functions_aggregation_helpers_hsum256(square);
        #endif
    #endif
    }
#else
    for (uint64_t i = 0; i < pSol->num_coordinates; i++) {
        BaseType_t* pCoordinate = pSol->pCoordinates + i;
        BaseType_t square = *pCoordinate * *pCoordinate;
        sum += square;
    }
#endif
    return sum;
}