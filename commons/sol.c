#include "sol.h"
#include "log.h"

#if CONFIG_USE_RDRND == 1
#include <immintrin.h>
#endif

static const char* TAG = "sol";

#define RDRAND_ERR_CHECK(retval) if ((retval)) { TIADSO_LOGI(TAG, "_rdrand32_step failed."); exit(EXIT_FAILURE); }

void sol_randomize(sol_t* pSol) {
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
void sol_population_randomized_init(sol_population_t* pPopulation) {
    for (uint64_t i = 0; i < pPopulation->pop_size; i++) {
        sol_t* pSol = pPopulation->pMembers + i;
        *pSol = SOL_CREATE(pPopulation->sol_num_dimensions);
        sol_randomize(pSol);
    }
}