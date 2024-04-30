#include <stdbool.h>
#include <stdlib.h>
#include <argp.h>
#include "commons/log.h"
#include "commons/sol.h"

#include "loss_functions.h"
#include "optimizers/bruteforce.h"


#if CONFIG_USE_RDRND == 0
#include <time.h>
#endif

static const char* TAG = "main";

const char *argp_program_version = "TiadSO 0.0";
const char *argp_program_bug_address = "<242524@edu.p.lodz.pl> <242511@edu.p.lodz.pl>";
static char doc[] = "TiadSO is a library with swarm optimization metaheuristics implementations equipped with a basic command line interface.";
static char args_doc[] = /*"[FILENAME]..."*/"";

static struct argp_option options[] = { 
    { .name = "max-iters", .key = 'i', .arg = "max-iters_arg", .flags = 0, .doc = ""},
    { .name = "accuracy", .key = 'a', .arg = "accuracy_arg", .flags = 0, .doc = ""},
    { .name = "dimensions", .key = 'd', .arg = "dimensions_arg", .flags = 0, .doc = ""},
    { .name = "fname", .key = 'f', .arg = "fname_arg", .flags = 0, .doc = ""},
    { 0 } 
};

struct arguments { 
    /*enum { } mode;
    bool isCaseInsensitive;*/
    tiadso_optimization_stop_condition_t stop_condition;
    uint64_t num_dimensions;
    TIADSO_LOSS_FN(loss_fn);
    BaseType_t loss_arg_left_bound;
    BaseType_t loss_arg_right_bound;
};

static void get_loss_fn_name(TIADSO_LOSS_FN(fn), char* pBuf, size_t bufSize) {
    if (fn == loss_functions__sphere) {
        snprintf(pBuf, bufSize, "sphere");
    } else {
        snprintf(pBuf, bufSize, "unknown");
    }
}

static void get_stop_condition_name(tiadso_optimization_stop_condition_t* pStopCondition, char* pBuf, size_t bufSize) {
    if (pStopCondition->type == LIMIT_ITERS) {
        snprintf(pBuf, bufSize, "LIMIT_ITERS");
    } else if (pStopCondition->type == AWAIT_ACCURACY) {
        snprintf(pBuf, bufSize, "AWAIT_ACCURACY");
    } else {
        snprintf(pBuf, bufSize, "<UNKNOWN>");
    }
}

#define STOP_CONDITION_NAME_BUF_SIZE 64
#define LOSS_FN_NAME_BUF_SIZE 64
static void print_arguments(struct arguments* pArgs) {
    char stopConditionName[STOP_CONDITION_NAME_BUF_SIZE];
    get_stop_condition_name(&pArgs->stop_condition, stopConditionName, STOP_CONDITION_NAME_BUF_SIZE);
    char lossFnName[LOSS_FN_NAME_BUF_SIZE];
    get_loss_fn_name(pArgs->loss_fn, lossFnName, LOSS_FN_NAME_BUF_SIZE);

    tiadso_log(TIADSO_LOG_INFO, TAG, "Arguments:");
    tiadso_log(TIADSO_LOG_INFO, TAG, "\tStop condition type: %s", stopConditionName);
    if (pArgs->stop_condition.type == AWAIT_ACCURACY) {
        tiadso_log(TIADSO_LOG_INFO, TAG, "\tAccuracy: %f", pArgs->stop_condition.param.accuracy);
    }
    tiadso_log(TIADSO_LOG_INFO, TAG, "\tIterations limit: %ld", pArgs->stop_condition.param.num_iters);
    tiadso_log(TIADSO_LOG_INFO, TAG, "\tNum dimensions: %ld", pArgs->num_dimensions);
    tiadso_log(TIADSO_LOG_INFO, TAG, "\tLoss function: %s", lossFnName);
    tiadso_log(TIADSO_LOG_INFO, TAG, "\tLeft loss function arguments bound: %f", pArgs->loss_arg_left_bound);
    tiadso_log(TIADSO_LOG_INFO, TAG, "\tRight loss function arguments bound: %f", pArgs->loss_arg_right_bound);
    tiadso_log(TIADSO_LOG_INFO, TAG, "==============================\n");
}

static void set_loss_function_and_bounds(char* fname, TIADSO_LOSS_FN(lossFn_out), BaseType_t* pLeftBound, BaseType_t* pRightBound) {
    if (!strcmp("sphere", fname)) {
        lossFn_out = loss_functions__sphere;
        *pLeftBound = LOSS_FUNCTION_SPHERE_COORD_MIN;
        *pRightBound = LOSS_FUNCTION_SPHERE_COORD_MAX;
    } else {
        TIADSO_LOGE(TAG, "Unknown function name.");
        exit(EXIT_FAILURE);
    }
}

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments* pArgs = state->input;
    switch (key) {
        case 'i':
            pArgs->stop_condition.param.num_iters = atoll(arg);  
            break;
        case 'a': 
            pArgs->stop_condition.type = AWAIT_ACCURACY;
            pArgs->stop_condition.param.accuracy = atof(arg); 
            break;
        case 'd': 
            pArgs->num_dimensions = atoll(arg);
            break;
        case 'f': 
            set_loss_function_and_bounds(arg, pArgs->loss_fn, &pArgs->loss_arg_left_bound, &pArgs->loss_arg_right_bound); 
            break;
        case ARGP_KEY_ARG: 
            return 0;
        default: 
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { 
    .options = options, 
    .parser = parse_opt, 
    .args_doc = args_doc, 
    .doc = doc, 
    .children = 0, 
    .help_filter = 0, 
    .argp_domain = 0 
};


#define ARGP_ERROR_CHECK(retval) if ((retval)) { TIADSO_LOGI(TAG, "Parsing error"); exit(EXIT_FAILURE); }
#define COORDINATES_STR_BUFSIZE 1024

int main(int argc, char** argv) {
#if CONFIG_USE_RDRND == 0
    #if CONFIG_USE_RDRND_AS_SEED == 0
    srand(time(0));
    #else
    unsigned int _r;
    RDRAND_ERR_CHECK(_rdrand32_step(&_r));
    srand(_r);
    #endif

#endif

    struct arguments arguments = { //defaults
        .stop_condition = {
            .type = LIMIT_ITERS,
            .param.num_iters = 100
        },
        .loss_arg_left_bound = LOSS_FUNCTION_SPHERE_COORD_MIN,
        .loss_arg_right_bound = LOSS_FUNCTION_SPHERE_COORD_MAX,
        .loss_fn = loss_functions__sphere,
        .num_dimensions = 2
    };

    /*arguments.mode = CHARACTER_MODE;
    arguments.isCaseInsensitive = false;*/

    int rv = argp_parse(
        &argp,      //__argp
        argc,       //__argc
        argv,       //__argv
        0,          //__flags
        0,          //__arg_index
        &arguments  //__input
    );
    if (rv) {
        tiadso_log(TIADSO_LOG_ERROR, TAG, "argp_parse returned with an error code (%d).", rv);
    };

    print_arguments(&arguments);
    
    TIADSO_LOGI(TAG, "Starting application");

    sol_t solution = SOL_CREATE(arguments.num_dimensions);
    sol_coordinates_alloc(&solution);
    
    bruteforce_config_t bruteforceConfig = {
        .common_config = {
            .coordinate_left_bound = arguments.loss_arg_left_bound,
            .coordinate_right_bound = arguments.loss_arg_right_bound,
            .num_dimensions = arguments.num_dimensions,
            .stop_condition = arguments.stop_condition
        }
    };

    tiadso_optimizers_bruteforce_optimize_loss(loss_functions__sphere, &bruteforceConfig, &solution);
    char str[COORDINATES_STR_BUFSIZE];
    sol_coordinates_to_str(&solution, str, COORDINATES_STR_BUFSIZE);
    BaseType_t f = loss_functions__sphere(&solution);
    tiadso_log(TIADSO_LOG_INFO, TAG, "Best solution: %s", str);
    tiadso_log(TIADSO_LOG_INFO, TAG, "Loss: %f", f);
    sol_coordinates_free(&solution);
    TIADSO_LOGI(TAG, "Exiting application");
    
    

    return 0;
}