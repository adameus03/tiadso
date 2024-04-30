#include "commons/types.h"
#include "commons/sol.h"

#define LOSS_FUNCTION_SPHERE_COORD_MIN -100
#define LOSS_FUNCTION_SPHERE_COORD_MAX 100
#define LOSS_FUNCTION_SPHERE_ACCURACY 0.0001

BaseType_t loss_functions__sphere(sol_t* pSol);