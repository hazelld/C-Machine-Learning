#include <assert.h>
#include "matrix.h"
#include "mpool.h"

/* 
 * This is the memory pool for the matrix_t* data type. For full docs, see the
 * docs in libs/mpool. These are thread safe, so it is fine to have as global.
 */
struct mpool* matrix_pool = NULL;

void __setup_mpool(int count) 
{	
	mpool_error e = init_mpool(sizeof(matrix_t), count, &matrix_pool);
	assert(e == MPOOL_SUCCESS);
}

/* Run on startup to seed rng. This is temporary, atleast till the move to 
 * CUDA is done. */
__attribute__((constructor))
static void seed_rng() {
	srand(time(NULL));
	__setup_mpool(100);
}


/* Free the memory pool */
__attribute__((destructor)) 
static void end() 
{
	free_mpool(matrix_pool);	
}

error_t init_matrix(matrix_t** m, unsigned int rows, unsigned int columns) 
{
	if (m == NULL) return E_NULL_ARG;
	if (matrix_pool == NULL) __setup_mpool(100);

	mpool_error e;

try:	
	*m = (matrix_t*) mpool_alloc(matrix_pool, &e);
	
	/* Ensure there was enough room to alloc */
	if (e != MPOOL_SUCCESS) {
		print_mpool_error(stdout, "Could not alloc", e);
		printf("Trying to realloc\n");
		e = mpool_realloc(mpool_capacity(matrix_pool) + 50, matrix_pool);
		
		if (e != MPOOL_SUCCESS) {
			print_mpool_error(stdout, "Could not realloc", e);
			printf("Failed to realloc\n");
			exit(-1);
		}
		goto try;
	}
		
	(*m)->rows = rows;
	(*m)->columns = columns;
	(*m)->matrix = malloc(sizeof(double*) * rows);

	for (int i = 0; i < rows; i++) { 
		(*m)->matrix[i] = malloc(sizeof(double) * columns);
		
		/* Ensure all values are set to 0 */
		for (int j = 0; j < columns; j++) 
			(*m)->matrix[i][j] = 0;
	}
	return E_SUCCESS;
}


error_t matrix_vector_mult(matrix_t* m, matrix_t* vec, matrix_t** result) 
{
	
	if (m == NULL || vec == NULL || result == NULL)
		return E_NULL_ARG;

	if ( (vec->rows > 1 && vec->columns > 1) || vec->columns < 1 || vec->rows < 1) 
		return E_NOT_VECTOR;
	
	if (m->rows < 1 || m->columns < 1)
		return E_ZERO_DIM_MATRIX;
	
	if (m->columns != vec->rows)
		return E_MATRIX_WRONG_DIM;

	init_matrix(result, m->rows, 1);
	
	/* init_matrix() garuntees that all values are set to 0 on init, so
	 * we can immediately use the += operator */
	for (int i = 0; i < m->rows; i++) {
		for (int j = 0; j < m->columns; j++) {
			(*result)->matrix[i][0] += m->matrix[i][j] * vec->matrix[j][0];
		}
	}
	return E_SUCCESS;
}


error_t matrix_scalar_mult (matrix_t* m, double scalar) 
{
	if (m == NULL) return E_NULL_ARG;

	for (int i = 0; i < m->rows; i++) {
		for (int j = 0; j < m->columns; j++) {
			m->matrix[i][j] *= scalar;
		}
	}	
	return E_SUCCESS;
}

error_t vector_scalar_addition (matrix_t* m, double scalar) 
{	
	if (m == NULL)
		return E_NULL_ARG;
	
	if (m->rows > 1 && m->columns > 1)
		return E_NOT_VECTOR;

	if (m->rows < 1 || m->columns < 1)
		return E_ZERO_DIM_MATRIX;

	for(int i = 0; i < m->rows; i++) 
		m->matrix[i][0] += scalar;	
	
	return E_SUCCESS;
}


error_t map_vector (matrix_t* vec, double (*f)(double)) 
{
	if (f == NULL)
		return E_NULL_ARG;
	
	for (int i = 0; i < vec->rows; i++) 
		vec->matrix[i][0] = (*f)(vec->matrix[i][0]);
	
	return E_SUCCESS;
}


error_t map_matrix (matrix_t* m, double(*f)(double)) 
{
	if (m == NULL || f == NULL) 
		return E_NULL_ARG;

	if (m->rows < 1 || m->columns < 1)
		return E_ZERO_DIM_MATRIX;

	for (int i = 0; i < m->rows; i++) {
		for (int j = 0; j < m->columns; j++) {
			m->matrix[i][j] = (*f)(m->matrix[i][j]);
		}
	}
	return E_SUCCESS;
}


error_t matrix_subtraction (matrix_t* m, matrix_t* n, matrix_t** result) 
{
	
	if (m == NULL || n == NULL || result == NULL)
		return E_NULL_ARG;
	
	if (m->rows != n->rows || m->columns != n->columns)
		return E_MATRIX_WRONG_DIM;

	init_matrix(result, m->rows, m->columns);
	
	for (int i = 0; i < m->rows; i++) {
		for (int j = 0; j < m->columns; j++) {
			(*result)->matrix[i][j] = m->matrix[i][j] - n->matrix[i][j];
		}
	}
	return E_SUCCESS;
}


error_t transpose (matrix_t** m) 
{
	matrix_t* old_matrix = *m;
	//matrix_t* new_matrix = malloc(sizeof(matrix_t));
	matrix_t* new_matrix;
	init_matrix(&new_matrix, (*m)->columns, (*m)->rows);
	
	for (int i = 0; i < new_matrix->columns; i++) {
		for (int j = 0; j < new_matrix->rows; j++) {
			new_matrix->matrix[j][i] = old_matrix->matrix[i][j]; 
		}
	}

	/* Free memory of old matrix and point m to new one */
	free_matrix(old_matrix);
	*m = new_matrix;
	return E_SUCCESS;
}


matrix_t* transpose_r (matrix_t* const m) 
{
	//matrix_t* new_matrix = malloc(sizeof(matrix_t));
	matrix_t* new_matrix;
	init_matrix(&new_matrix, m->columns, m->rows);

	for (int i = 0; i < new_matrix->columns; i++) {
		for (int j = 0; j < new_matrix->rows; j++) {
			new_matrix->matrix[j][i] = m->matrix[i][j]; 
		}
	}
	return new_matrix;	
}


error_t multiply_vector(matrix_t* m, matrix_t* n, matrix_t** result) 
{
	if (m == NULL || n == NULL || result == NULL)
		return E_NULL_ARG;

	if (m->columns > 1 || n->columns > 1)
		return E_NOT_VECTOR;
	
	if (m->columns < 1 || m->rows < 1 || n->rows < 1 || n->columns < 1)
		return E_ZERO_DIM_MATRIX;
	
	if (m->rows != n->rows)
		return E_MATRIX_WRONG_DIM;

	init_matrix(result, m->rows, m->columns);
	for (int i = 0; i < m->rows; i++) 
		(*result)->matrix[i][0] = m->matrix[i][0] * n->matrix[i][0];
	return E_SUCCESS;
}


matrix_t* random_matrix (unsigned int rows, unsigned int columns, double interval) 
{
	//matrix_t* random_matrix = malloc(sizeof(matrix_t));
	matrix_t* random_matrix;
	init_matrix(&random_matrix, rows, columns);
	double div = RAND_MAX / (interval * 2);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			random_matrix->matrix[i][j] = -interval + (rand() / div);
		}
	}	
	return random_matrix;
}


error_t kronecker_vectors (matrix_t* vec1, matrix_t* vec2, matrix_t** result) 
{
	if (vec1 == NULL || vec2 == NULL || result == NULL)
		return E_NULL_ARG;
	
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
		return E_NOT_VECTOR;	
	}

	init_matrix(result, rows, columns);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			(*result)->matrix[i][j] = vertical_v->matrix[i][0] * horiz_v->matrix[0][j];
		}
	}	

	return E_SUCCESS;
}


error_t copy_matrix (matrix_t* src, matrix_t** dest) 
{
	if (src == NULL || dest == NULL)
		return E_NULL_ARG;

	init_matrix(dest, src->rows, src->columns);

	for (int i = 0; i < src->rows; i++) {
		for (int j = 0; j < src->columns; j++) {
			(*dest)->matrix[i][j] = src->matrix[i][j];
		}
	}
	return E_SUCCESS;
}

error_t free_matrix (matrix_t* m) 
{
	if (m == NULL) 
		return E_NULL_ARG;

	for (int i = 0; i < m->rows; i++) {
		free(m->matrix[i]);
		m->matrix[i] = NULL;
	}
	free(m->matrix);
	
	/* 
	 * Since mpool only fake frees the memory, to ensure we don't use old pointers,
	 * set the pointers to NULL 
	 */
	m->matrix = NULL;
	mpool_error e = mpool_dealloc(m, matrix_pool);
	assert(e == MPOOL_SUCCESS);
	return E_SUCCESS;
}


void print_matrix (FILE* fh, matrix_t* m) 
{
	for (int i = 0; i < m->rows; i++) {
		for (int j = 0; j < m->columns; j++) {
			fprintf(fh, "%lf ", m->matrix[i][j]);
		}
		fprintf(fh, "\n");
	}
}
