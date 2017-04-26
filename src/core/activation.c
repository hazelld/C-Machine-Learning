#include <stdlib.h>
#include <math.h>
#include "net.h"
#include "net-internal.h"

/* Activation functions and their derivative */
static double sigmoid_f (double x);
static double sigmoid_fp (double x);
/* static double tanh_f (double x);  C math lib provides this */
static double tanh_fp (double x);


/* Short-hand for setting values in the activation_f type, keeps the switch statement 
 * in get_activation_f cleaner 
 */
static void set_activation_f (activation_f* actf, act_func_t type, act_func af, act_func ap) {
	actf->type = type;
	actf->af = af;
	actf->ap = ap;
}

/* get_activation_f() */
error_t get_activation_f (activation_f* actf, act_func_t type, act_func af, act_func ap ) {
	
	if (actf == NULL) return E_NULL_ARG;

	switch (type) {
		case SIGMOID:
			set_activation_f(actf, type, sigmoid_f, sigmoid_fp);
			break;

		case TANH:
			set_activation_f(actf, type, tanh, tanh_fp);
			break;

		case CUSTOM:
			if (af == NULL || ap == NULL) 
				return E_NO_CALLBACK_GIVEN;

			set_activation_f(actf, type, af, ap);
			break;
	}
	
	return E_SUCCESS;
}


static double sigmoid_f (double x) {
	return 1 / (1 + exp(-x));
}

static double sigmoid_fp (double x) {
	return x * (1 - x);
}

/* dx tanh(x) = 1 - tanh^2(x) */
static double tanh_fp (double x) {
	double fx = tanh(x);
	return 1 - (fx * fx);
}
