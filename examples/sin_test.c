#include <assert.h>
#include <math.h>
#include "matrix.h"
#include "cml.h"
#include "data-builder.h"

void print_m (matrix_t* m) {
	for (int i = 0; i < m->rows; i++) {
		for (int j = 0; j < m->columns; j++) {
			printf("%lf ", m->matrix[i][j]);
		}
		printf("\n");
	}
}

int main() {
	int topology[4] = { 1, 5, 5, 1 };
	data_set* data;
	FILE* fh = fopen("examples/data/sin.csv", "r");

	if (fh == NULL) {
		printf("Could not open file: data/sin.csv. Ensure you are running from root project directory.\n");
		exit(EXIT_FAILURE);
	}

	activation_f actf;
	get_activation_f(&actf, SIGMOID, NULL, NULL);
	
	int lines = 0;
	data = init_data_set();

	error_t err = data_set_from_csv(data, fh, &lines);
	char* features[1] = { "x" };
	err = set_input_features(data, features, 1);
	err = split_data (data, 0.7);

	printf("Read %d lines\n", lines);


	net* nn = init_net(0.1, QUADRATIC);
	
	/* Build the layers */
	layer* layers[4] = { 
		build_layer(input, 0, 1, actf),
		build_layer(output, 1, 1, actf),
		build_layer(hidden, 1, 5, actf),
		build_layer(hidden, 1, 5, actf)
	};
	
	/* Add layers then connect the net */ 
	for (int i = 0; i < 4; i++) {
		err = add_layer(nn, layers[i]);
		assert(err == E_SUCCESS);
	}
	
	err = connect_net(nn);
	assert(err == E_SUCCESS);
	
	err = train(nn, data, 10);
	print_cml_error(stdout, "Training Error", err);
	assert(err == E_SUCCESS);

	printf("\n\nRESULTS:\n");
	for (int i = 0; i < 5; i++) {

		double div = RAND_MAX / (2 * 2);
		double nrand = -2 + ((double)rand() / div);
		
		printf("sin(%lf) = %lf \t\t", nrand, sin(nrand));
		
		cml_data* in = init_cml_data();
		double* val = malloc(sizeof(double));
		*val = nrand;
		add_to_cml_data(in, val);

		cml_data* res = predict(nn, in);

		/* Transform the value back to regular range,
		 * member the transformation was:
		 * 0.5 * sinx + 0.5 
		 */
		printf("GOT: %lf\n", (2 * get_value_at(res, 0) - 1));
		
		free_cml_data(in);
		free_cml_data(res);
	}
		
	fclose(fh);
	
	free_data_set(data);
	free_net(nn);
	
}
