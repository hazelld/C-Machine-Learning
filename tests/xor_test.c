#include <math.h>
#include "matrix.h"
#include "net.h"
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
	int topology[3] = { 2, 8, 1 };
	net* nn = malloc(sizeof(net));
	data_set* data;
	FILE* fh = fopen("tests/xor.csv", "r");

	data = data_set_from_csv(fh);
	init_net(nn, 3, topology, sigmoid, sigmoid_prime, 0.3);
	train(nn, data, 3000);

	printf("\n\nRESULTS:\n");
	for (int i = 0; i < 4; i++) {
		matrix_t* res = predict(nn, data->data[i]->input);
		printf("%d %d = %lf\n", (int)data->data[i]->input->matrix[0][0], (int)data->data[i]->input->matrix[1][0], res->matrix[0][0]);
	}
	
	fclose(fh);
	free_data_set(data);
	free_net(nn);
}
