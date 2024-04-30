#include "bruteforce.h"

static const char* TAG = "bruteforce";

static __thread TIADSO_LOSS_FN(__fLoss);
static __thread bruteforce_config_t __config;

void __bruteforce_step(sol_t* pSol, BaseType_t* pVal) {
    sol_randomize(pSol, __config.common_config.coordinate_left_bound, __config.common_config.coordinate_right_bound);
    *pVal = __fLoss(pSol);
}

void tiadso_optimizers_bruteforce_optimize_loss(TIADSO_LOSS_FN(fLoss), bruteforce_config_t* pConfig, sol_t* pSol) {
    tiadso_optimization_common_config_check(&pConfig->common_config, true, TAG);
    __fLoss = fLoss;
    __config = *pConfig;

    BaseType_t minValue, minValueCandidate;
    sol_t bestSol = SOL_CREATE(__config.common_config.num_dimensions);
    sol_coordinates_alloc(&bestSol);
    sol_t bestSolCandidate = SOL_CREATE(__config.common_config.num_dimensions);
    sol_coordinates_alloc(&bestSolCandidate);
    
    __bruteforce_step(&bestSol, &minValue);
    if (__config.common_config.stop_condition.type == LIMIT_ITERS) {
        for (uint64_t i = 1; i < __config.common_config.stop_condition.param.num_iters; i++) {
            __bruteforce_step(&bestSolCandidate, &minValueCandidate);
            if (minValueCandidate < minValue) {
                sol_coordinates_cpy(&bestSol, &bestSolCandidate);
                minValue = minValueCandidate;
            }
        }
    } else { // __config.common_config.stop_condition.type == AWAIT_ACCURACY
        while (minValue > __config.common_config.stop_condition.param.accuracy) {
            __bruteforce_step(&bestSolCandidate, &minValueCandidate);
            if (minValueCandidate < minValue) {
                sol_coordinates_cpy(&bestSol, &bestSolCandidate);
                minValue = minValueCandidate;
            }
        }
    }


    sol_coordinates_free(&bestSolCandidate);
    sol_coordinates_cpy(pSol, &bestSol);
    sol_coordinates_free(&bestSol);
}