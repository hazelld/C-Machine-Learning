#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "cml.h"

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
error_t init_matrix(matrix_t** m, unsigned int rows, unsigned int columns);


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
error_t matrix_vector_mult(matrix_t* m, matrix_t* vec, matrix_t** result);


/* matrix_scalar_mult
 *	
 *	This function multiplies a matrix by a given scalar in place. 
 *
 */
error_t matrix_scalar_mult(matrix_t* m, double scalar);


/* vector_scalar_addition
*
* 	This function adds a constant to each element of a vector.
*
* 	Returns:
*	0 => Success
*	_ => Failure
*
*/
error_t vector_scalar_addition (matrix_t* m, double scalar);


/* function_on_matrix
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
error_t map_matrix (matrix_t* m, double(*f)(double));


/* map_vector
*
* 	Functionally the same as function_on_matrix, however this
* 	only applys the function to a column-wise matrix_t.
*
* 	Returns:
* 	0 => Success
* 	_ => Failure
*/
error_t map_vector (matrix_t* vec, double (*f)(double));


/* matrix_subtraction
 *	
 *	This function subtracts the values of matrix n from 
 *	the values of matrix m. This function requires m & n to
 *	have equal dimensions.
 *
 *	Returns:
 *	0 => Success
 *	_ => Failure
 *
 */
error_t matrix_subtraction (matrix_t* m, matrix_t* n, matrix_t** result);


/*	transpose
 *	
 *	This function transposes a matrix inplace. 
 *
 *	Return:
 *		0 => Success, m now holds the transposed matrix
 *		_ => Failure, m is unchanged
 */
error_t transpose (matrix_t** m);


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
error_t multiply_vector(matrix_t* m, matrix_t* n, matrix_t** result);


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
error_t kronecker_vectors (matrix_t* vec1, matrix_t* vec2, matrix_t** result);


/* copy_matrix
 *	
 *	Used to copy a matrix into another. One must be an allocated matrix that 
 *	has been initialized. The second must be allocated already.
 *
 */
error_t copy_matrix (matrix_t* src, matrix_t** dest);


/*	free_matrix
 *
 *	This function frees all resources associated with a matrix, including 
 *	the top level struct matrix. 
 *
 *	Returns:
 *	0 => Success
 *	_ => Failure
 */
error_t free_matrix(matrix_t* matrix);


/* print_matrix
 *
 * 	Print the matrix out for debugging purposes.
 */
void print_matrix (FILE* fh, matrix_t* m);


#endif

