#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <linux/random.h>

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
matrix_t* matrix_vector_dot(matrix_t* m, matrix_t* vec);


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

int function_on_matrix (matrix_t* m, double(*f)(double));

matrix_t* matrix_subtraction (matrix_t* m, matrix_t* n);

/*	transpose
 *	
 *	This function transposes a matrix inplace. 
 *
 *	Return:
 *		0 => Success, m now holds the transposed matrix
 *		_ => Failure, m is unchanged
 */
int transpose (matrix_t** m);


/*	transpose_r
 *
 *	This function returns a transposed matrix, leaving the 
 *	original matrix intact.
 *
 */
matrix_t* transpose_r (matrix_t* const m);


/*	multiply_vector
 *
 *	Multiply two equal dimensional vectors together.
 *	
 *	ie.
 *	 _  _		 _  _		 _    _
 *	| x1 |		| y1 |		| x1y1 |
 *	| .. |	x	| .. |	=	| ...  |
 *	| xn |		| yn |		| xnyn |
 *   -  -		 -  -		 -	  -
 *	
 *	Returns the result, the original two vectors are untouched.
 */
matrix_t* multiply_vector(matrix_t* m, matrix_t* n);


matrix_t* subtract_vector(matrix_t* m, matrix_t* n);

/*	random_matrix
 *
 *	This function initializes a matrix and populates it with uniform random
 *	values on the interval [-interval, interval]. The values placed in the 
 *	matrix are doubles. 
 *
 */
matrix_t* random_matrix (unsigned int rows, unsigned int columns, double interval);


/* kronecker_vectors
 *
 * This function applys the kronecker product across two vectors, one
 * being a horizontal vector, one being vertical.
 *
 * ie.
 *	 _  _		 
 *	| x1 |		 -     -
 *	| x2 |	k	| y1 y2 |
 *	| x3 |		 -     -
 *	 -  -		
 *	   _         -
 *	  | x1y1 x1y2 |
 *	= | x2y1 x2y2 |
 *	  | x3y1 x3y2 |
 *	   -         -
 *
 * Note this function returns NULL if it is not supplied with 2 
 * 1 dimensional vectors.
 */
matrix_t* kronecker_vectors (matrix_t* vec1, matrix_t* vec2);


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

