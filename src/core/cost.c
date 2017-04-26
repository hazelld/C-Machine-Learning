#include <math.h>
#include "net.h"
#include "net-internal.h"

/* The cost functions are all defined below, eventually there may be support for 
 * custom ones much like activation functions, however there are some issues with 
 * this. Since the gradient function has to deal with the matrix_t type, this exposes
 * this part of the program that is other-wise hidden from the user. For now, most of 
 * the main functions will (eventually) be implemented. 
 *
 */

/* Static defines */
static double quadratic_cost (matrix_t* o, matrix_t* e);

/* calculate_cost_func() */
double calculate_cost_func (net* n, matrix_t* expected) {

	matrix_t* output = n->layers[n->layer_count - 1]->output;

	switch (n->costf) {
		case QUADRATIC:
			return quadratic_cost(output, expected);
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
	for (int i = 0; i < o->columns; i++) {
		sum_squares += pow(o->matrix[i][0] - e->matrix[i][0], 2);
	}
	return 0.5 * sum_squares;
}
