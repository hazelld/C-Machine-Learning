#include <stdlib.h>
#include <stdio.h>
#include "munit.h"
#include "cml.h"
#include "cml-internal.h"


/* test_vector_scalar_addition
 *
 * 	Want to test the vector_scalar_addition for the following:
 * 		-> Handles NULL arguments
 * 		-> Handles non-vectors
 * 		-> Handles zero dimension vector (uninit)
 * 		-> Properly adds values
 *
 */
static MunitResult
test_vector_scalar_addition (const MunitParameter params[], void* data) {

	error_t err;
	double addition_values[3] = { -4, 0, 3 };
	double vec_values[3] = { -1, 0, 1 };

	/* Silence compiler warnings about unused args */
	(void) params;
	(void) data;
	
	/* NULL matrix_t */
	matrix_t* vec = NULL;
	err = vector_scalar_addition(vec, 0);
	munit_assert(err == E_NULL_ARG);
	
	/* Non-vector matrix */
	vec = malloc(sizeof(matrix_t));
	init_matrix(vec, 2, 2);
	err = vector_scalar_addition(vec, 0);
	munit_assert(err == E_NOT_VECTOR);
	free_matrix(vec);

	/* Zero dimension vector */
	vec = malloc(sizeof(matrix_t));
	init_matrix(vec, 0, 0);
	err = vector_scalar_addition(vec, 0);
	munit_assert(err == E_ZERO_DIM_MATRIX);
	free_matrix(vec);

	/* Make sure it properly adds values */
	matrix_t* base = malloc(sizeof(matrix_t));
	vec = malloc(sizeof(matrix_t));
	init_matrix(vec, 3, 1);

	for (int i = 0; i < 3; i++) 
		vec->matrix[i][0] = vec_values[i];
	copy_matrix(vec, base);

	/* Try all values in addition_values */ 
	for (int i = 0; i < 3; i++) {
		err = vector_scalar_addition(vec, addition_values[i]);
		munit_assert(err == E_SUCCESS);

		/* Make sure the values added properly */
		for (int j = 0; j < 3; j++) 
			munit_assert(vec->matrix[j][0] == (base->matrix[j][0] + addition_values[i]));
		
		free_matrix(vec);
		vec = malloc(sizeof(matrix_t));
		copy_matrix(base, vec);
	}

	return MUNIT_OK;
}



/* Set up the test suite */
static MunitTest test_suite_tests[] = {
	{(char*) "vector_scalar_addition", test_vector_scalar_addition, NULL, NULL,
		MUNIT_TEST_OPTION_NONE, NULL},
	{NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

/* Declare test suite */
static const MunitSuite test_suite = {
	(char*) "matrix/",
	test_suite_tests,
	NULL,
	1,
	MUNIT_SUITE_OPTION_NONE
};


int main (int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
	return munit_suite_main(&test_suite, (void*) "munit", argc, argv);
}
