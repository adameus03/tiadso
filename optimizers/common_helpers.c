#include "optimizer-common-defs.h"

bool tiadso_optimization_common_config_check(tiadso_optimizer_common_config_t* pConfig, bool terminate, const char* log_tag) {
    bool is_valid = !(pConfig->stop_condition.type == LIMIT_ITERS && pConfig->stop_condition.param.num_iters == 0);
    if (!is_valid) {
        TIADSO_LOGE(log_tag, "Invalid contents of tiadso_optimizer_common_config_t structure detected.");
        if (terminate) {
            exit(EXIT_FAILURE);
        }
    }
    return is_valid;
}