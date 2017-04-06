#include "matrix.h"

/* Error logging function */
static int log_error (int err, char* func, char* var);

/* Run on startup to seed rng. This is temporary, atleast till the move to 
 * CUDA is done. */
__attribute__((constructor))
static void seed_rng() {
	srand(time(NULL));
}


int init_matrix(matrix_t* m, unsigned int rows, unsigned int columns) {
	if (m == NULL) return 1;

	m->rows = rows;
	m->columns = columns;

	m->matrix = malloc(sizeof(double*) * rows);

	for (int i = 0; i < rows; i++) { 
		m->matrix[i] = malloc(sizeof(double) * columns);
		
		/* Ensure all values are set to 0 */
		for (int j = 0; j < columns; j++) 
			m->matrix[i][j] = 0;
	}
	return 0;
}


int matrix_vector_dot(matrix_t* m, matrix_t* vec, matrix_t** result) {
	
	if (m == NULL || vec == NULL || *result == NULL || result == NULL)
		return log_error(NULL_ARG, "matrix_vector_dot", "");

	if ( (vec->rows > 1 && vec->columns > 1) || vec->columns < 1 || vec->rows < 1) 
		return log_error(NOT_VECTOR, "matrix_vector_dot", "matrix_t* vec");
	
	if (m->rows < 1 || m->columns < 1)
		return log_error(ZERO_DIM_MATRIX, "matrix_vector_dot", "matrix_t* m");
	
	if (m->columns != vec->rows)
		return log_error(MATRIX_WRONG_DIM, "matrix_vector_dot", "");

	init_matrix(*result, m->rows, 1);
	
	/* init_matrix() garuntees that all values are set to 0 on init, so
	 * we can immediately use the += operator */
	for (int i = 0; i < m->rows; i++) {
		for (int j = 0; j < m->columns; j++) {
			(*result)->matrix[i][0] += m->matrix[i][j] * vec->matrix[j][0];
		}
	}
	return SUCCESS;
}


int vector_scalar_addition (matrix_t* m, double scalar) {	
	if (m == NULL)
		return log_error(NULL_ARG, "vector_scalar_addition", "matrix_t* m");
	
	if (m->rows > 1 && m->columns > 1)
		return log_error(NOT_VECTOR, "vector_scalar_addition", "matrix_t* m");

	if (m->rows < 1 || m->columns < 1)
		return log_error(ZERO_DIM_MATRIX, "vector_scalar_addition", "matrix_t* m");

	for(int i = 0; i < m->rows; i++) 
		m->matrix[i][0] += scalar;	
	return 0;
}


int function_on_vector (matrix_t* vec, double (*f)(double)) {
	if (f == NULL)
		return log_error(NULL_ARG, "function_on_vector", "double (*f)(double)");
	
	for (int i = 0; i < vec->rows; i++) 
		vec->matrix[i][0] = (*f)(vec->matrix[i][0]);
	return SUCCESS;
}


int function_on_matrix (matrix_t* m, double(*f)(double)) {
	if (m == NULL || f == NULL) 
		return log_error(NULL_ARG, "function_on_matrix", "");

	if (m->rows < 1 || m->columns < 1)
		return log_error(ZERO_DIM_MATRIX, "function_on_matrix", "matrix_t* m");

	for (int i = 0; i < m->rows; i++) {
		for (int j = 0; j < m->columns; j++) {
			m->matrix[i][j] = (*f)(m->matrix[i][j]);
		}
	}
	return SUCCESS;
}


int matrix_subtraction (matrix_t* m, matrix_t* n, matrix_t** result) {
	
	if (m == NULL || n == NULL || result == NULL || *result == NULL)
		return log_error(NULL_ARG, "matrix_subtraction", "");
	
	if (m->rows != n->rows || m->columns != n->columns)
		return log_error(MATRIX_WRONG_DIM, "matrix_subtraction", "");

	init_matrix(*result, m->rows, m->columns);
	
	for (int i = 0; i < m->rows; i++) {
		for (int j = 0; j < m->columns; j++) {
			(*result)->matrix[i][j] = m->matrix[i][j] - n->matrix[i][j];
		}
	}
	return SUCCESS;
}


int transpose (matrix_t** m) {
	matrix_t* old_matrix = *m;
	matrix_t* new_matrix = malloc(sizeof(matrix_t));
	init_matrix(new_matrix, (*m)->columns, (*m)->rows);
	
	for (int i = 0; i < new_matrix->columns; i++) {
		for (int j = 0; j < new_matrix->rows; j++) {
			new_matrix->matrix[j][i] = old_matrix->matrix[i][j]; 
		}
	}

	/* Free memory of old matrix and point m to new one */
	free_matrix(old_matrix);
	*m = new_matrix;
	return 0;
}


matrix_t* transpose_r (matrix_t* const m) {
	matrix_t* new_matrix = malloc(sizeof(matrix_t));
	init_matrix(new_matrix, m->columns, m->rows);

	for (int i = 0; i < new_matrix->columns; i++) {
		for (int j = 0; j < new_matrix->rows; j++) {
			new_matrix->matrix[j][i] = m->matrix[i][j]; 
		}
	}
	return new_matrix;	
}


int multiply_vector(matrix_t* m, matrix_t* n, matrix_t** result) {
	if (m == NULL || n == NULL || result == NULL || *result == NULL)
		return log_error(NULL_ARG, "multiply_vector", "");

	if (m->columns > 1 || n->columns > 1)
		return log_error(NOT_VECTOR, "multiply_vector", "");
	
	if (m->columns < 1 || m->rows < 1 || n->rows < 1 || n->columns < 1)
		return log_error(ZERO_DIM_MATRIX, "multiply_vector", "");
	
	if (m->rows != n->rows)
		return log_error(MATRIX_WRONG_DIM, "multiply_vector", "matrix_t* m, n");

	init_matrix(*result, m->rows, m->columns);
	for (int i = 0; i < m->rows; i++) 
		(*result)->matrix[i][0] = m->matrix[i][0] * n->matrix[i][0];
	return SUCCESS;
}


matrix_t* random_matrix (unsigned int rows, unsigned int columns, double interval) {
	matrix_t* random_matrix = malloc(sizeof(matrix_t));
	init_matrix(random_matrix, rows, columns);
	double div = RAND_MAX / (interval * 2);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			random_matrix->matrix[i][j] = -interval + (rand() / div);
		}
	}	
	return random_matrix;
}


int kronecker_vectors (matrix_t* vec1, matrix_t* vec2, matrix_t** result) {
	if (vec1 == NULL || vec2 == NULL || result == NULL || *result == NULL)
		return log_error(NULL_ARG, "kronecker_vectors", "");
	
	unsigned int rows, columns;
	matrix_t *vertical_v, *horiz_v;

	/* Determine matrix dimensions and which is horizontal vector */
	if (vec1->rows == 1 && vec1->columns > 1) {
		rows = vec2->rows;
		columns = vec1->columns;
		horiz_v = vec1;
		vertical_v = vec2;
	} else if (vec2->rows == 1 && vec2->columns >= 1) {
		rows = vec1->rows;
		columns = vec2->columns;
		horiz_v = vec2;
		vertical_v = vec1;
	} else { 
		return log_error(NOT_VECTOR, "kronecker_vectors", "");	
	}

	init_matrix(*result, rows, columns);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			(*result)->matrix[i][j] = vertical_v->matrix[i][0] * horiz_v->matrix[0][j];
		}
	}	

	return SUCCESS;
}


int free_matrix (matrix_t* m) {
	if (m == NULL) 
		return 1;

	for (int i = 0; i < m->rows; i++)
		free(m->matrix[i]);
	free(m->matrix);
	free(m);
	return 0;
}


static int log_error (int err, char* func, char* var) {
	char* err_str;

	switch (err) {

		case NULL_ARG: 
			err_str = "Argument passed to function is NULL.";
			break;

		case MATRIX_WRONG_DIM:
			err_str = "Matrix dimensions do not match expected.";
			break;

		case ZERO_DIM_MATRIX:
			err_str = "Matrix has dimensions less than 1.";
			break;

		case NOT_VECTOR:
			err_str = "Expected a 1 dimension vector, got a matrix.";
			break;
		
		default:
			err_str = "Unrecognized error code.";
			break;
	}

	fprintf(stderr, "Error: %s Occured in: %s(). Offending variable:%s\n", \
			err_str, func, var);
	
	return FAILURE;
}
