#include <assert.h>
#include <math.h>
#include "matrix.h"
#include "cml.h"
#include "builder.h"

void print_m (matrix_t* m) {
	for (int i = 0; i < m->rows; i++) {
		for (int j = 0; j < m->columns; j++) {
			printf("%lf ", m->matrix[i][j]);
		}
		printf("\n");
	}
}

/*	Sigmoid function and its derivative
 */
double sigmoid (double val) {
	double ret = 1 / (1 + exp(-val));
	return ret;
}

double sigmoid_prime (double val) {
	return val * (1 - val);
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
	data = data_set_from_csv(fh);
	net* nn = init_net(0.1);
	
	/* Build the layers */
	layer* layers[4] = { 
		build_layer(input, 0, 1, actf),
		build_layer(output, 1, 1, actf),
		build_layer(hidden, 1, 5, actf),
		build_layer(hidden, 1, 5, actf)
	};
	
	/* Add layers then connect the net */ 
	error_t err;
	for (int i = 0; i < 4; i++) {
		err = add_layer(nn, layers[i]);
		assert(err == E_SUCCESS);
	}
	
	err = connect_net(nn);
	assert(err == E_SUCCESS);
	
	train(nn, data, 15);

	printf("\n\nRESULTS:\n");
	for (int i = 0; i < 5; i++) {

		double div = RAND_MAX / (2 * 2);
		double nrand = -2 + ((double)rand() / div);
		
		printf("sin(%lf) = %lf \t\t", nrand, sin(nrand));
	
		matrix_t* in = malloc(sizeof(matrix_t));
		init_matrix(in, 1, 1);
		in->matrix[0][0] = nrand;
		matrix_t* res = predict(nn, in);

		/* Transform the value back to regular range,
		 * member the transformation was:
		 * 0.5 * sinx + 0.5 
		 */
		printf("GOT: %lf\n", (2 * res->matrix[0][0]) - 1);
	
		free_matrix(in);
	}
		
	fclose(fh);
	
	free_data_set(data);
	free_net(nn);
	
}
