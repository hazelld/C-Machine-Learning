#include <math.h>
#include "cml.h"
#include "net-internal.h"
#include "matrix.h"

/* The cost functions are all defined below, eventually there may be support for 
 * custom ones much like activation functions, however there are some issues with 
 * this. Since the gradient function has to deal with the matrix_t type, this exposes
 * this part of the program that is other-wise hidden from the user. For now, most of 
 * the main functions will (eventually) be implemented. 
 *
 */

/* Static defines */
static double quadratic_cost (matrix_t* o, matrix_t* e);
static error_t quadratic_gradient (matrix_t* o, matrix_t* e, matrix_t** result);

/* calculate_cost_func() */
double calculate_cost_func (net* n, matrix_t* expected) {

	matrix_t* output = n->layers[n->layer_count - 1]->output;

	switch (n->costf) {
		case QUADRATIC:
			return quadratic_cost(output, expected);
	}
}


/* calculate_cost_gradient() */
error_t calculate_cost_gradient(net* n, matrix_t* expected, matrix_t** result) {
	matrix_t* output = n->layers[n->layer_count - 1]->output;

	switch (n->costf) {
		case QUADRATIC:
			return quadratic_gradient(output, expected, result);
	}
}


/* quadratic_cost()
 *
 *	This implements the quadratic cost function.
 *
 * Arguments:
 * 	o => Output vector from network
 * 	e => Expected vector from network
 */
static double quadratic_cost (matrix_t* o, matrix_t* e) {
	
	double sum_squares = 0.0;
	for (int i = 0; i < o->rows; i++) {
		sum_squares += pow(o->matrix[i][0] - e->matrix[i][0], 2);
	}
	return 0.5 * sum_squares;
}


/* quadratic_gradient()
 *
 * 	This function computes the vector that is the quadratic gradient function:
 * 	(actual - expected). It returns a vector that holds the gradients.
 *	
 * Arguments:
 * 	o => Output vector from network
 * 	e => Expected vector from network
 */
static error_t quadratic_gradient (matrix_t* o, matrix_t* e, matrix_t** result) {
	return matrix_subtraction(o, e, result);
}
