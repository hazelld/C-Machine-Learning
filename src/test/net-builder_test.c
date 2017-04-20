#include <stdlib.h>
#include <stdio.h>
#include "munit.h"
#include "err.h"
#include "net.h"
#include "net-internal.h"

/* test_build_layer()
 *
 * This function tests the build_layer() functionality from net-builder.c for:
 * 	-> Handles NULL args
 *	-> Sets the proper values
 *
 */
static MunitResult
test_build_layer (const MunitParameter params[], void* data) {
	(void) params;
	(void) data;
	
	layer_type lt = input;
	int bias = 0;
	int nodes = 10;
	act_func_t act_type = SIGMOID;
	activation_f actf;
	get_activation_f(&actf, act_type, NULL, NULL);
	
	/* First try to break the function */
	layer* broke_l = build_layer(NULL, lt, bias, nodes, actf);
	munit_assert(broke_l == NULL);

	/* Now try real input */
	layer* l = build_layer(&l, lt, bias, nodes, actf);

	/* Check the values worked */
	munit_assert(l->ltype == lt);
	munit_assert(l->using_bias == bias);
	munit_assert(l->output_nodes == nodes);
	munit_assert(l->actf.type == act_type);

	/* Clean up */
	free_layer(l);
}


/* Set up the test suite */
static MunitTest test_suite_tests[] = {
	{(char*) "build_layer", test_build_layer, NULL, NULL,
		MUNIT_TEST_OPTION_NONE, NULL},
	{NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

/* Declare test suite */
static const MunitSuite test_suite = {
	(char*) "net-builder/",
	test_suite_tests,
	NULL,
	1,
	MUNIT_SUITE_OPTION_NONE
};


int main (int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
	return munit_suite_main(&test_suite, (void*) "munit", argc, argv);
}
