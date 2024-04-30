#include <stdbool.h>
#include <stdlib.h>
#include <argp.h>
#include "commons/log.h"
#include "commons/sol.h"

#if CONFIG_USE_RDRND == 0
#include <time.h>
#endif


static const char* TAG = "main";

const char *argp_program_version = "TiadSO 0.0";
const char *argp_program_bug_address = "<242524@edu.p.lodz.pl> <242511@edu.p.lodz.pl>";
static char doc[] = "TiadSO is a library with swarm optimization metaheuristics implementations equipped with a basic command line interface.";
static char args_doc[] = /*"[FILENAME]..."*/"";

static struct argp_option options[] = { 
    /*{ .name = "", .key = 'l', .arg = 0, .flags = 0, .doc = ""},
    { .name = "", .key = 'w', .arg = 0, .flags = 0, .doc = ""},
    { .name = "", .key = 'i', .arg = 0, .flags = 0, .doc = ""},*/
    
    { 0 } 
};

struct arguments { 
    /*enum { } mode;
    bool isCaseInsensitive;*/
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments* pArgs = state->input;
    switch (key) {
        /*case '': pArgs-> = ; break;
        case '': pArgs-> = ; break;
        case '': pArgs->isCaseInsensitive = true; break;*/
        case ARGP_KEY_ARG: return 0;
        default: return ARGP_ERR_UNKNOWN;
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

int main(int argc, char** argv) {
#if CONFIG_USE_RDRND == 0
    srand(time(0));
#endif

    struct arguments arguments = {};

    /*arguments.mode = CHARACTER_MODE;
    arguments.isCaseInsensitive = false;*/

    argp_parse(
        &argp,      //__argp
        argc,       //__argc
        argv,       //__argv
        0,          //__flags
        0,          //__arg_index
        &arguments  //__input
    );
    
    TIADSO_LOGI(TAG, "Starting application");

    __m512 v512 = _mm512_setr_ps(1.0f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f, 4.5f,
                                5.0f, 5.5f, 6.0f, 6.5f, 7.0f, 7.5f, 8.0f, 8.5f);
    
    float* pVals = (float*)&v512;
    for (uint64_t i = 0; i < sizeof(__m512) / sizeof(float); i++) {
        printf("%f ", pVals[i]);
    }
    
    printf("\n");

    return 0;
}