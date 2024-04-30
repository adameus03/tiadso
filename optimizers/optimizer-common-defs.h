#ifndef __OPTIMIZERS_OPTIMIZER_COMMON_DEFS
#define __OPTIMIZERS_OPTIMIZER_COMMON_DEFS

#include "../commons/types.h"
#include <stdint.h>
#include <stdbool.h>
#include "../commons/log.h"

typedef struct {
    enum stop_types {
        LIMIT_ITERS, 
        AWAIT_ACCURACY
    } type;

    union {
        uint64_t num_iters;
        BaseType_t accuracy;
    } param;
    
} tiadso_optimization_stop_condition_t;

typedef struct {
    tiadso_optimization_stop_condition_t stop_condition;
    uint32_t num_dimensions;
    BaseType_t coordinate_left_bound;
    BaseType_t coordinate_right_bound;
} tiadso_optimizer_common_config_t;

extern bool tiadso_optimization_common_config_check(tiadso_optimizer_common_config_t* pConfig, bool terminate, const char* log_tag);

#endif