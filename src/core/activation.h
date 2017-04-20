#ifndef _ACTIVATION_H_
#define _ACTIVATION_H_

#include <stdlib.h>
#include <math.h>
#include "err.h"

/*	This is the signature of most activation functions. Just giving it an 
 *	easy to use name.
 */
typedef double (*act_func)(double);


/* activation_functions
 *
 * 	These are the type of activation functions. As more are added, this will 
 * 	change to reflect that. These are defined inside activation.c. 
 *
 * 	If the CUSTOM is selected, then the user will provide callbacks to their own 
 * 	activation function, and derivative of that function.
 *
 */
typedef enum activation_functions {
	SIGMOID,
	TANH,
	CUSTOM,
} act_func_t;


typedef struct activation_f {
	act_func_t type;
	act_func af;
	act_func ap;
} activation_f;


/* get_activation_f
 *
 * 	This function is used to create an activation function data structure. This function
 * 	will allocate the activation_f* if it is not allocated before the function is called.
 *
 * Arguments:
 *	actf -> Location of the pointer to activation_f type 
 *	type -> Type of activation function
 *	af -> Pointer to activation function, only needed when type is CUSTOM
 *	ap -> Pointer to the activation function's derivative, only needed when type is CUSTOM
 *
 *	Note: If type is not CUSTOM, then af and ap are ignored. 
 */
error_t get_activation_f (activation_f** actf, act_func_t type, act_func af, act_func ap);


#endif
