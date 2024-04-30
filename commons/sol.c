#include "sol.h"
#include "log.h"
#include <stdio.h>

#if CONFIG_USE_RDRND == 1
#include <immintrin.h>
#endif

static const char* TAG = "sol";

void sol_randomize(sol_t* pSol, BaseType_t memberCoordinateLeftBound, BaseType_t memberCoordinateRightBound) {
    uint32_t _r = 0;
    for (uint64_t i = 0; i < pSol->num_coordinates; i++) {
        BaseType_t* pCoordinate = pSol->pCoordinates + i;
#if CONFIG_USE_RDRND == 1
        RDRAND_ERR_CHECK(_rdrand32_step(&_r));
        *pCoordinate = ((BaseType_t)_r) / (BaseType_t)UINT32_MAX;
#else
        _r = rand();
        *pCoordinate = ((BaseType_t)_r) / (BaseType_t)RAND_MAX;
#endif
        *pCoordinate = memberCoordinateLeftBound + (*pCoordinate) * (memberCoordinateRightBound - memberCoordinateLeftBound);  
    }

#if CONFIG_USE_AVX512_FMA == 1 || CONFIG_USE_AVX2_FMA == 1
    #if CONFIG_USE_AVX512_FMA == 1
    uint64_t num_coords_per_packet = (sizeof(Avx512BaseType_t) / sizeof(BaseType_t));
    #elif CONFIG_USE_AVX2_FMA == 1
    uint64_t num_coords_per_packet = (sizeof(Avx2BaseType_t) / sizeof(BaseType_t));
    #endif
    uint64_t rem = pSol->num_coordinates % num_coords_per_packet;
    BaseType_t* pRedundantCoordinates = pSol->pCoordinates + pSol->num_coordinates;
    if (rem) {
        for (uint64_t i = 0; i < num_coords_per_packet - rem; i++) {
            pRedundantCoordinates[i] = (BaseType_t)0;
        }
    }
#endif
}
void sol_population_randomized_init(sol_population_t* pPopulation, BaseType_t memberCoordinateLeftBound, BaseType_t memberCoordinateRightBound) {
    for (uint64_t i = 0; i < pPopulation->pop_size; i++) {
        sol_t* pSol = pPopulation->pMembers + i;
        //*pSol = SOL_CREATE(pPopulation->sol_num_dimensions);
        sol_randomize(pSol, memberCoordinateLeftBound, memberCoordinateRightBound);
    }
}

void sol_coordinates_to_str(sol_t* pSol, char* pStr, uint64_t strLen) {
    uint64_t strIndex = 0;
    for (uint64_t i = 0; i < pSol->num_coordinates - 1; i++) {
        BaseType_t coordinate = pSol->pCoordinates[i];
        strIndex += snprintf(pStr + strIndex, strLen - strIndex, "%f", coordinate);
        strIndex += snprintf(pStr + strIndex, strLen - strIndex, ", ");
    }
    BaseType_t coordinate = pSol->pCoordinates[pSol->num_coordinates - 1];
    snprintf(pStr + strIndex, strLen - strIndex, "%f", coordinate);
}