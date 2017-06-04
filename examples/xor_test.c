#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "cml.h"
#include "matrix.h"
#include "data-builder.h"

int main() {
	data_set* data;
	FILE* fh = fopen("examples/data/xor.csv", "r");
	
	if (fh == NULL) {
		printf("Could not open file data/xor.csv Ensure you are running from the root project directory.\n");
		exit(EXIT_FAILURE);
	}
	
	/* 
	 * Set the activation function, note since we aren't using custom callbacks,
	 * send the last 2 args as NULL 
	 */
	activation_f actf;
	get_activation_f(&actf, SIGMOID, NULL, NULL);
	
	int lines = 0;

	/* Set up the data set */
	data = init_data_set();
	error_t err = data_set_from_csv(data, fh, &lines);
	printf("Read %d lines\n", lines);

	/* Set the features we want */
	char* features[2] = { "input1", "input2" };
	err = set_input_features(data, features, 2);

	/* Don't need a testing set, so split data as 100% training */
	err = split_data(data, 1);
	
	data->test_set = data->training_set;
	data->test_count = data->training_count;

	/* Initialize the net with the learning rate and the type of cost func */
	net* nn = init_net(0.1, 0.9, CROSS_ENTROPY);

	/* Set up the layers */
	layer* layers[3] = {
		build_layer(input, 0, 2, actf),
		build_layer(hidden, 1, 5, actf),
		build_layer(output, 0, 1, actf),
	};

	for (int i = 0; i < 3; i++) {
		err = add_layer(nn, layers[i]);
	}
	
	for (int i = 0; i < data->count; i++) {
		printf("DATA PAIR %d:\n", i);
		
		for (int j = 0; j < data->data[i]->input->count; j++) 
			printf("\tINPUT %d = %lf\n", j, *((double*)data->data[i]->input->items[j]));
		
		for (int j = 0; j < data->data[i]->expected_output->count; j++) 
			printf("\tOUTPUT %d = %lf\n", j, *((double*)data->data[i]->expected_output->items[j]));
	}
	err = connect_net(nn);
	assert(err == E_SUCCESS);

	err = train(nn, data, 5000);
	assert(err == E_SUCCESS);

	printf("\n\nRESULTS:\n");

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			cml_data* in = init_cml_data();

			/* 
			 * Note that we are mallocing inside the loop, because once these 
			 * pointers are given to cml_data struct through add_to_cml_data(), 
			 * the struct "owns" them. We can't free or modify this data anymore.
			 *
			 * This occurs because cml_data holds the data as a void*, as we can 
			 * load categorical variables (char*). Since it doesn't know what it is
			 * pointing too, it only makes a shallow copy.
			 *
			 * In the future this should change, as it is kind of sloppy and may
			 * cause really hard to find bugs. It may either include a size param,
			 * or a copy callback.
			 *
			 */
			double* val1 = malloc(sizeof(double));
			double* val2 = malloc(sizeof(double));
			
			*val1 = (double)i;
			*val2 = (double)j;
			
			add_to_cml_data(in, val1);
			add_to_cml_data(in, val2);
			
			cml_data* ret = predict(nn, in);

			printf("XOR(%d, %d)\tGOT: %lf\n", i, j, get_value_at(ret, 0));

			free_cml_data(in);
			free_cml_data(ret);
		}
	}
	
	data->test_set = NULL;
	fclose(fh);
	free_data_set(data);
	free_net(nn);
}
