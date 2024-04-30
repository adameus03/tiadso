#include "../config.h"
#include <stdint.h>
#include <stdlib.h>
#include "typedefs.h"

#if CONFIG_USE_AVX512_FMA == 1 || CONFIG_USE_AVX2_FMA == 1
#include <immintrin.h>
#endif

typedef struct {
    uint64_t num_coordinates;
    BaseType_t* pCoordinates;
#if CONFIG_USE_AVX512_FMA == 1 || CONFIG_USE_AVX2_FMA == 1
    uint64_t num_coordinate_packets;
    #if CONFIG_USE_AVX512_FMA == 1
    Avx512BaseType_t* pCoordinatePackets;
    #elif CONFIG_USE_AVX2_FMA == 1
    Avx2BaseType_t* pCoordinatePackets;
    #endif
#endif
} sol_t;

#define SOL_CREATE(n) ((sol_t){ .num_coordinates = (n) })

static inline void sol_coordinates_alloc(sol_t* pSol) {
#if CONFIG_USE_AVX512_FMA == 1 || CONFIG_USE_AVX2_FMA
    #if CONFIG_USE_AVX512_FMA == 1
    uint64_t num_coords_per_packet = (sizeof(Avx512BaseType_t) / sizeof(BaseType_t));
    #elif CONFIG_USE_AVX2_FMA == 1
    uint64_t num_coords_per_packet = (sizeof(Avx2BaseType_t) / sizeof(BaseType_t));
    #endif
    pSol->num_coordinate_packets = pSol->num_coordinates / num_coords_per_packet;
    if (pSol->num_coordinates % num_coords_per_packet) {
        pSol->num_coordinate_packets++;
    }
    #if CONFIG_USE_AVX512_FMA == 1
    pSol->pCoordinatePackets = (Avx512BaseType_t*) aligned_alloc(sizeof(BaseType_t), pSol->num_coordinate_packets * sizeof(Avx512BaseType_t));
    #elif CONFIG_USE_AVX2_FMA == 1
    pSol->pCoordinatePackets = (Avx2BaseType_t*) aligned_alloc(sizeof(BaseType_t), pSol->num_coordinate_packets * sizeof(Avx2BaseType_t));
    #endif
    pSol->pCoordinates = (BaseType_t*)pSol->pCoordinatePackets;
#else
    pSol->pCoordinates = (BaseType_t*) malloc(sizeof(BaseType_t) * pSol->num_coordinates);
#endif
}

static inline void sol_coordinates_free(sol_t* pSol) {
    pSol->num_coordinates = 0;
#if CONFIG_USE_AVX512_FMA == 1 || CONFIG_USE_AVX2_FMA == 1
    pSol->num_coordinate_packets = 0;
    free(pSol->pCoordinatePackets);
#else
    free(pSol->pCoordinates);
#endif
}

typedef struct {
    uint64_t pop_size;
    uint64_t sol_num_dimensions;
    sol_t* pMembers;
} sol_population_t;

static inline void sol_population_members_alloc(sol_population_t* pPopulation) {
    pPopulation->pMembers = (sol_t*) malloc(sizeof(sol_t) * pPopulation->pop_size);
}

static inline void sol_population_members_free(sol_population_t* pPopulation) {
    free(pPopulation->pMembers);
}

void sol_randomize(sol_t* pSol);
void sol_population_randomized_init(sol_population_t* pPopulation);