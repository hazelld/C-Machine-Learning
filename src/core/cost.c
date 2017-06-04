#include <math.h>
#include "cml.h"
#include "cml-internal.h"
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
static double cross_entropy_cost (matrix_t* o, matrix_t* e);
static error_t cross_entropy_gradient (matrix_t* o, matrix_t* e, matrix_t** result);


/* calculate_cost_func() */
double calculate_cost_func (net* n, matrix_t* expected) 
{
	matrix_t* output = n->layers[n->layer_count - 1]->output;

	switch (n->costf) {
		case QUADRATIC:
			return quadratic_cost(output, expected);
		case CROSS_ENTROPY:
			return cross_entropy_cost(output, expected);
	}
}


/* calculate_cost_gradient() */
error_t calculate_cost_gradient(net* n, matrix_t* expected, matrix_t** result) 
{
	matrix_t* output = n->layers[n->layer_count - 1]->output;

	switch (n->costf) {
		case QUADRATIC:
			return quadratic_gradient(output, expected, result);
		case CROSS_ENTROPY:
			return cross_entropy_gradient(output, expected, result);
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
static double quadratic_cost (matrix_t* o, matrix_t* e) 
{
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
static error_t quadratic_gradient (matrix_t* o, matrix_t* e, matrix_t** result) 
{
	return matrix_subtraction(o, e, result);
}


/**
 * cross_entropy_cost() - Computes the total sum of the cross-entropy cost 
 * function.
 *	
 *	@o: Output of the network
 *	@e: Expected output of the network
 *
 */
static double cross_entropy_cost (matrix_t* o, matrix_t* e) 
{
	double sum = 0.0;
	for (int i = 0; i < o->rows; i++) {
		double expected = e->matrix[i][0];
		double output = o->matrix[i][0];
		sum += ((expected * log(output)) + ((1 - expected) * log(1-output)));
	}
	return sum * -1;
}

/**
 * cross_entropy_gradient() - Computes the gradient wrt each output of the network,
 * returns a vector holding all the gradients.
 *
 * @o: Output of the network
 * @e: Expected output of the network
 * @result: Location of where to put the result of the calculation
 *
 * Note that it _may_ be possible for a division by zero error here if the output 
 * value is 1. If this is used with the sigmoid function this should not cause an
 * issue as the range is (0,1), but the value may be rounded up to 1 if it is very 
 * close. Not sure at the moment how to fix this, but need to come back to it 
 * eventually.
 */
static error_t cross_entropy_gradient (matrix_t* o, matrix_t* e, matrix_t** result)
{
		
	if (o->rows != e->rows || o->columns != e->columns)
		return E_MATRIX_WRONG_DIM;
	matrix_t* res = malloc(sizeof(matrix_t));
	init_matrix(res, o->rows, o->columns);

	for (int i = 0; i < o->rows; i++) {
		double expected = e->matrix[i][0];
		double output = o->matrix[i][0];
		res->matrix[i][0] = (output-expected) / ((1-output) * (output));
	}
	*result = res;
	return E_SUCCESS;
}


