#include "../commons/types.h"
#include "../commons/sol.h"
#include "optimizer-common-defs.h"

typedef struct {
    tiadso_optimizer_common_config_t common_config;
} bruteforce_config_t;

/**
 * @note pSol should be a pointer to struct with allocated coordinates
*/
void tiadso_optimizers_bruteforce_optimize_loss(TIADSO_LOSS_FN(fLoss), bruteforce_config_t* pConfig, sol_t* pSol);