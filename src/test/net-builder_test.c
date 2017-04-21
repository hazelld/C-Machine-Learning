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
	
	layer_type lt = input;
	int bias = 0;
	int nodes = 10;
	act_func_t act_type = SIGMOID;
	activation_f actf;
	get_activation_f(&actf, act_type, NULL, NULL);
	
	/* Build the layer */
	layer* l = build_layer(lt, bias, nodes, actf);

	/* Check the values worked */
	munit_assert(l->ltype == lt);
	munit_assert(l->using_bias == bias);
	munit_assert(l->output_nodes == nodes);
	munit_assert(l->actf.type == act_type);

	/* Clean up */
	error_t err = free_layer(l);
	munit_assert(err == E_SUCCESS);
}

/* test_add_layer_extra_layers()
 *
 * 	This function tests that add_layer() catches trying to add extra input/output
 * 	layers, and returns the proper error codes.
 */
static MunitResult
test_add_layer_extra_layers(const MunitParameter params[], void* data) {
	
	error_t err, input_err, output_err;
	activation_f actf;
	get_activation_f(&actf, SIGMOID, NULL, NULL);
	layer_type types[2] = { input, output };

	input_err = E_TOO_MANY_INPUT_LAYERS;
	output_err = E_TOO_MANY_OUTPUT_LAYERS;

	for (int i = 0; i < 2; i++) {
		
		/* Build the structures */
		net* n = init_net();
		layer* l_one = build_layer(types[i], 0, 10, actf);
		layer* l_two = build_layer(types[i], 0, 10, actf);
		
		/* First addition should return OK, then error */
		err = add_layer(n, l_one);
		munit_assert(err == E_SUCCESS);
		err = add_layer(n, l_two);
		munit_assert(err == (types[i] == input ? input_err : output_err) );
		
		/* Free resources */
		err = free_layer(l_two);
		munit_assert(err == E_SUCCESS);

		err = free_net(n);
		munit_assert(err == E_SUCCESS);
	}
}


/* test_add_layer_duplicate_layers()
 *
 *	This function is to ensure you can't add a layer to the network twice. This 
 *	would cause some really weird issues I am sure, so let's prevent it.
 */
static MunitResult
test_add_layer_duplicate_layers(const MunitParameter params[], void* data) {
	error_t err;
	activation_f actf;
	get_activation_f(&actf, SIGMOID, NULL, NULL);

	net* n = init_net();
	layer* l = build_layer(input, 0, 10, actf);

	err = add_layer(n, l);
	munit_assert(err == E_SUCCESS);
	err = add_layer(n, l);
	munit_assert(err == E_LAYER_ALREADY_IN_NET);
	free_net(n);
}

/* test_add_layer()
 *
 *	This function tests the basic functionality of the add_layer() function.
 *	Once we add the layer, there are a few things to verify:
 *	-> The add_layer() function worked
 *	-> The net's layer_count variable is 1 larger than before
 *	-> The layer was properly added to the net's layer array
 *
 */
static MunitResult
test_add_layer(const MunitParameter params[], void* data) {

	error_t err;
	activation_f actf;
	get_activation_f(&actf, SIGMOID, NULL, NULL);
	int lcount;

	net* n = init_net();
	lcount = n->layer_count;
	munit_assert(lcount == 0);

	layer* input_layer = build_layer(input, 0, 10, actf);
	layer* hidden_layer = build_layer(hidden, 0, 10, actf);
	layer* output_layer = build_layer(hidden, 0, 10, actf);
	
	layer* layer_arr[3] = {input_layer, hidden_layer, output_layer};

	for (int i = 0; i < 3; i++) {
		err = add_layer(n, layer_arr[i]);
		munit_assert(err == E_SUCCESS);
		munit_assert(n->layer_count == (lcount + 1));
		munit_assert(n->layers[i] == layer_arr[i]);
		lcount = n->layer_count;
	}

	free_net(n);
}

/* Set up the test suite */
static MunitTest test_suite_tests[] = {
	{(char*) "build_layer", test_build_layer, NULL, NULL,
		MUNIT_TEST_OPTION_NONE, NULL},
	{(char*) "add_layer/extra_layers", test_add_layer_extra_layers, NULL, NULL,
		MUNIT_TEST_OPTION_NONE, NULL},
	{(char*) "add_layer/duplicate_layers", test_add_layer_duplicate_layers, NULL, NULL,
		MUNIT_TEST_OPTION_NONE, NULL},
	{(char*) "add_layer", test_add_layer, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
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

