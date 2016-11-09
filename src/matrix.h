#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <stdlib.h>
#include <stdio.h>


typedef struct matrix_t {
	double** matrix;
	unsigned int rows; // m
	unsigned int columns; //n
} matrix_t;


matrix_t* init_matrix(unsigned int rows, unsigned int columns);

matrix_t* matrix_vector_product(matrix_t* m, matrix_t* vec);

int vector_scalar_addition (matrix_t* m, double scalar);

int free_matrix(matrix_t* matrix);


#endif

