#include <stdlib.h>
#include <stdio.h>
#include "munit.h"
#include "cml.h"
#include "cml-internal.h"

static MunitResult
test_init_free_cml_data_no_items () {
	cml_data* data = init_cml_data();
	munit_assert(data != NULL);
	
	error_t err = free_cml_data(NULL);
	munit_assert(err == E_NULL_ARG);
	err = free_cml_data(data);
	munit_assert(err == E_SUCCESS);
	return MUNIT_OK;
}


static MunitResult
test_add_cml_data_double () {
	cml_data* data = init_cml_data();
	munit_assert(data != NULL);

	error_t err;
	for (int i = 0; i < 5; i++) {
		double* dub = malloc(sizeof(double));
		*dub = i;
		err = add_to_cml_data(data, (void*)dub);
		munit_assert(err == E_SUCCESS);
		munit_assert_ptr_equal(dub, data->items[i]);
	}

	free_cml_data(data);
	return MUNIT_OK;
}


static MunitResult
test_add_cml_data_str () {
	cml_data* data = init_cml_data();
	
	const char* sentences[3] = {
		"Hello World",
		"Foo Bar Baz",
		"Quick brown fox jumps over lazy dog",
	};

	error_t err;
	for (int i = 0; i < 3; i++) {
		char* text = malloc(sizeof(char) * 70);
		strcpy(text, sentences[i]);
		err = add_to_cml_data(data, (void*)text);
		munit_assert(err == E_SUCCESS);
		munit_assert_ptr_equal(text, data->items[i]);
	}

	free_cml_data(data);
	return MUNIT_OK;
}


static MunitResult
test_data_set_from_csv () {
	static char* test_data = "src/test/data.csv";
	int line_count = 5;
	FILE* fh = fopen(test_data, "r");

	int line_err;
	data_set* ds = init_data_set();
	
	error_t err = data_set_from_csv (ds, fh, &line_err);
	fprintf(stderr, "Parsed %d lines\n", line_err);
	munit_assert_int(err, ==, E_SUCCESS);
	munit_assert_int(line_count, ==, line_err);
	
	/* Test to ensure data is in the right place */
	munit_assert_int(ds->feature_count, ==, 3);
	
	munit_assert_string_equal("Column 1", ds->feature_names[0]);
	munit_assert_string_equal("Column 3", ds->feature_names[2]);
	
	munit_assert_int(T_STR, ==, ds->feature_types[0]);
	munit_assert_int(T_DOUBLE, ==, ds->feature_types[1]);
	munit_assert_int(T_DOUBLE, ==, ds->feature_types[2]);

	munit_assert_double(*((double*)ds->raw_data[0]->items[1]), ==, 1); 
	munit_assert_double(*((double*)ds->raw_data[1]->items[1]), ==, -7 ); 
	munit_assert_double(*((double*)ds->raw_data[3]->items[1]), ==, -5.7 ); 

	munit_assert_string_equal((char*)ds->raw_data[2]->items[0], "float"); 
	
	free_data_set(ds);
	fclose(fh);	
	return MUNIT_OK;
}


static MunitResult
test_get_feature_names () {
	static char* test_data = "src/test/data.csv";
	FILE* fh = fopen(test_data, "r");
	
	int line_err;
	data_set* ds = init_data_set();
	error_t err = data_set_from_csv(ds, fh, &line_err);
	munit_assert_int(err, ==, E_SUCCESS);

	char** feature = NULL;
	int count = 0;
	err = get_feature_names(ds, &feature, &count);
	
	munit_assert_int(count, ==, 3);
	
	for (int i = 0; i < count; i++) {
		munit_assert_string_equal(ds->feature_names[i], feature[i]);
		free(feature[i]);
	}
	free(feature);
	free_data_set(ds);
	fclose(fh);
	return MUNIT_OK;
}


static MunitResult
test_split_data () {
	static char* test_data = "src/test/data.csv";
	FILE* fh = fopen(test_data, "r");
	
	int line_err;
	data_set* ds = init_data_set();
	error_t err = data_set_from_csv(ds, fh, &line_err);

	char** feature = NULL;
	int count = 0;

	err = split_data(ds, 70);
	munit_assert_int(err, ==, E_NO_INPUT_FEATURES_SPECIFIED);

	err = get_feature_names(ds, &feature, &count);
	munit_assert_int(err, ==, E_SUCCESS);

	char* inputs[2] = { feature[0], feature[2] };
	err = set_input_features (ds, inputs, 2);
	munit_assert_int(err, ==, E_SUCCESS);
	munit_assert_int(ds->features_specified, ==, FEATURES_SPECIFIED);

	fprintf(stderr, "Splitting data...\n");
	err = split_data(ds, 0.7);
	munit_assert_int(err, ==, E_SUCCESS);

	fprintf(stderr, "Freeing dataset\n");
	free_data_set(ds);

	fprintf(stderr, "Freeing feature name array\n");
	for (int i = 0; i < 3; i++)
		free(feature[i]);
	free(feature);
	fclose(fh);
	return MUNIT_OK;
}

/* Set up the test suite */
static MunitTest test_suite_tests[] = {
	{(char*) "init/free data no items", test_init_free_cml_data_no_items, NULL,
		NULL, MUNIT_TEST_OPTION_NONE, NULL},
	{(char*) "test_add_cml_data_double", test_add_cml_data_double, NULL, NULL,
		MUNIT_TEST_OPTION_NONE, NULL},
	{(char*) "test_add_cml_data_str", test_add_cml_data_str, NULL, NULL,
		MUNIT_TEST_OPTION_NONE, NULL},
	{(char*) "test_data_set_from_csv", test_data_set_from_csv, NULL, NULL,
		MUNIT_TEST_OPTION_NONE, NULL},
	{(char*) "test_get_feature_names", test_get_feature_names, NULL, NULL,
		MUNIT_TEST_OPTION_NONE, NULL},
	{(char*) "test_split_data", test_split_data, NULL, NULL, MUNIT_TEST_OPTION_NONE, 
		NULL},
	{NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

/* Declare test suite */
static const MunitSuite test_suite = {
	(char*) "data-builder/",
	test_suite_tests,
	NULL,
	1,
	MUNIT_SUITE_OPTION_NONE
};


int main (int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
	return munit_suite_main(&test_suite, (void*) "munit", argc, argv);
}
