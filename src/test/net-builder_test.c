#include <stdlib.h>
#include <stdio.h>
#include "munit.h"
#include "cml.h"
#include "cml-internal.h"

/* Build a basic test network */
static net* _build_net(int hidden_layers, layer_type missing_lt);


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
	munit_assert_int((int)l->ltype, ==, (int)lt);
	munit_assert_int((int)l->using_bias, ==, (int)bias);
	munit_assert_int((int)l->output_nodes, ==, (int)nodes);
	munit_assert_int((int)l->actf.type, ==, (int)act_type);
	
	/* Clean up */
	error_t err = free_layer(l);
	munit_assert(err == E_SUCCESS);
	return MUNIT_OK;
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
		net* n = init_net(0.1, QUADRATIC);
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
	return MUNIT_OK;
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

	net* n = init_net(0.1, QUADRATIC);
	layer* l = build_layer(input, 0, 10, actf);

	err = add_layer(n, l);
	munit_assert(err == E_SUCCESS);
	err = add_layer(n, l);
	munit_assert(err == E_LAYER_ALREADY_IN_NET);
	free_net(n);

	return MUNIT_OK;
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

	net* n = init_net(1, QUADRATIC);
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
	return MUNIT_OK;
}


/* test_connect_net()
 *
 * 	Tests the basic functionality of the connect_net. It will randomly
 * 	add 1 input, 1 output, and $hidden_layer amount of hidden layers. This should
 * 	eventually find any issue, and will cover all cases. 
 *
 */
static MunitResult
test_connect_net(const MunitParameter params[], void* data) {
	error_t err;
	int hidden_layers = 5;

	net* n = _build_net(hidden_layers, hidden);
	
	err = connect_net(n);
	munit_assert_int((int)err, ==, (int)E_SUCCESS);

	/* Assert a few things */
	munit_assert_int((int)n->layers[0]->ltype, ==, (int)input);
	munit_assert_int((int)n->layers[n->layer_count-1]->ltype, ==, (int)output);

	for (int i = 1; i < n->layer_count - 1; i++) 
		munit_assert_int((int)n->layers[i]->ltype, ==, (int)hidden);

	munit_assert_int((int)n->layer_count, ==, (int)(hidden_layers + 2));
	free_net(n);

	return MUNIT_OK;
}


/* test_connect_net_no_input_layer()
 *
 * 	Test that connect_net() handles when no input layer is given 
 */
static MunitResult 
test_connect_net_no_input_layer (const MunitParameter params[], void* data) {
	error_t err;
	net* n = _build_net(5, input);
	err = connect_net(n);
	munit_assert_int((int)err, ==, (int)E_NO_INPUT_LAYER);
	free_net(n);
	return MUNIT_OK;
}


/* test_connect_net_no_output_layer()
 *
 *	Test that connect_net() handles when no output layer is given
 */
static MunitResult 
test_connect_net_no_output_layer (const MunitParameter params[], void* data) {
	error_t err;
	net* n = _build_net(5, output);
	err = connect_net(n);
	munit_assert_int((int)err, ==, (int)E_NO_OUTPUT_LAYER);
	free_net(n);
	return MUNIT_OK;
}


/* Set up the test suite */
static MunitTest test_suite_tests[] = {
	{(char*) "build_layer", test_build_layer, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
	{(char*) "add_layer/extra_layers", test_add_layer_extra_layers, NULL, NULL,
		MUNIT_TEST_OPTION_NONE, NULL},
	{(char*) "add_layer/duplicate_layers", test_add_layer_duplicate_layers, NULL, NULL,
		MUNIT_TEST_OPTION_NONE, NULL},
	{(char*) "add_layer", test_add_layer, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
	{(char*) "connect_net", test_connect_net, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
	{(char*) "connect_net/no_input_layer", test_connect_net_no_input_layer, NULL, NULL,
		MUNIT_TEST_OPTION_NONE, NULL},
	{(char*) "connect_net/no_output_layer", test_connect_net_no_output_layer, NULL, NULL,
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


/* _shuffle_layers() 
 *
 * Shuffle an array of layers to randomly add to a net.
 */
static void _shuffle_layers (layer** l, int size) {

	for (int i = 0; i < size - 1; i++) {
		int j = i + rand() / (RAND_MAX / (size - i) + 1);
		fprintf(stderr, "random number is: %d (MAX %d)\n", j, size);
		layer* buff = l[j];
		l[j] = l[i];
		l[i] = buff;
	}
}

/* _build_net() 
 *
 * 	Builds a basic net with the given amount of hidden layers. Also can 
 * 	build one with only hidden layers to try and break connect_net() 
 *
 */
static net* _build_net (int hidden_layers, layer_type missing_lt) {
	
	net* n = init_net(1, QUADRATIC);
	activation_f actf;
	get_activation_f(&actf, SIGMOID, NULL, NULL);
	
	/* Set up the layers */ 
	int added_layers = 0;
	layer** layers = NULL;

	/* All possible layers */
	if (missing_lt == hidden) {
		added_layers = 2;
		layers = malloc(sizeof(layer*) * 2);
		layers[0] = build_layer(input, 0, 10, actf);
		layers[1] = build_layer(output, 0, 10, actf);
	} else {
		added_layers = 1;
		layers = malloc(sizeof(layer*) * added_layers);
		
		if (missing_lt == input) 
			layers[0] = build_layer(output, 0, 10, actf);
		else 
			layers[0] = build_layer(input, 0, 10, actf);
	}

	for (int i = added_layers; i < hidden_layers + added_layers; i++) {
		layers = realloc(layers, sizeof(layer*) * (i + added_layers + 1) );
		layers[i] = build_layer(hidden, 0, 10, actf);
	}
	
	_shuffle_layers(layers, added_layers + hidden_layers);
	
	error_t err;
	for (int i = 0; i < hidden_layers + added_layers; i++) {

		err = add_layer(n, layers[i]);
		munit_assert_int((int)err, ==, (int)E_SUCCESS);
	}	

	free(layers);
	return n;
}
