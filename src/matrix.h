#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <stdlib.h>
#include <stdio.h>
#include <time.h>


typedef struct matrix_t {
	double** matrix;
	unsigned int rows; // m
	unsigned int columns; //n
} matrix_t;


/* init_matrix
 *
 *	This function initializes a matrix_t on the heap with 
 *	the specified dimensions. To free the matrix_t use
 *	free_matrix().
 *
 */
matrix_t* init_matrix(unsigned int rows, unsigned int columns);


/* matrix_vector_product
 *
 *	This function takes a matrix, and a vector (single column matrix) and
 *	will return a matrix that is the result of the matrix-vector product.
 *	
 *	The formula is:
 *
 *	 -                 -		 -    -
 *	| a11  a12 ...  a1n |		|  x1  |
 *	| ...               |	x 	|  ..  |
 *	| am1  am2 ... amn  |		|  xn  |
 *	 -                 -		 -    -
 *
 *    -                           -
 *   | a11x1 + a12x2 + ... + a1nxn |
 * = | ...                         |
 *   | am1x1 + am2x2 + ... + amnxn |
 *    -                           -
 *
 *    Note: the matrix m and vector vec are not deallocated in this function
 */
matrix_t* matrix_vector_product(matrix_t* m, matrix_t* vec);


/* vector_scalar_addition
 *
 * 	This function adds a constant to each element of a vector.
 *
 * 	Returns:
 *	0 => Success
 *	_ => Failure
 *
 */
int vector_scalar_addition (matrix_t* m, double scalar);


/* function_on_vector
 *
 *	This function applys a function onto each value of the matrix. 
 *
 *	If f(x) is the function to be applied, the formula is:
 *
 * 	 -              -		 -                        -
 * 	| a11 a12 .. a1n |		| f(a11) f(a12) ... f(a1n) |
 * 	| ...            |	=	| ...                      |
 * 	| am1 am2 .. amn |		| f(am1) f(am2) ... f(amn) |
 * 	 -              -		 -                        -
 *
 * 	 Returns:
 * 	 0 => Success 
 * 	 _ => Failure
 */
int function_on_vector (matrix_t* vec, double (*f)(double));


/*	random_matrix
 *
 *	This function initializes a matrix and populates it with uniform random
 *	values on the interval [-interval, interval]. The values placed in the 
 *	matrix are doubles. 
 *
 */
matrix_t* random_matrix (unsigned int rows, unsigned int columns, double interval);


/*	free_matrix
 *
 *	This function frees all resources associated with a matrix, including 
 *	the top level struct matrix. 
 *
 *	Returns:
 *	0 => Success
 *	_ => Failure
 */
int free_matrix(matrix_t* matrix);

#endif

