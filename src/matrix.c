#include "matrix.h"


matrix_t* init_matrix(unsigned int rows, unsigned int columns) {
	matrix_t* m = malloc(sizeof(matrix_t));
	
	m->rows = rows;
	m->columns = columns;

	m->matrix = malloc(sizeof(double*) * rows);

	for (int i = 0; i < rows; i++) 
		m->matrix[i] = malloc(sizeof(double));

	return m;
}

matrix_t* matrix_vector_product(matrix_t* m, matrix_t* vec) {
	
	matrix_t* result = init_matrix(vec->rows, 1);
	int i, j;
		
	for (i = 0; i < m->rows; i++) {
		result->matrix[i][0] = 0;

		for (j = 0; j < m->columns; j++) {
			result->matrix[i][0] += m->matrix[i][j] * vec->matrix[j][0];
		}
	}
	return result;
}

int vector_scalar_addition (matrix_t* m, double scalar) {	
	for(int i = 0; i < m->rows; i++) {
		m->matrix[i][0] += scalar;
	}
	
	return 1;
}
