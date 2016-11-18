#include <math.h>
#include "matrix.h"
#include "net.h"
#include "matrix-builder.h"

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
	int topology[3] = { 1, 3, 1 };
	net* nn = malloc(sizeof(net));
	data_set* data;
	FILE* fh = fopen("tests/sin.csv", "r");

	data = parse_from_csv(fh);
	init_net(nn, 3, topology, sigmoid, sigmoid_prime, 0.3);
	train(nn, data, 5000);

	printf("\n\nRESULTS:\n");
	for (int i = 0; i < 5; i++) {
		double div = RAND_MAX / 4;
		double nrand = -2 + ((double)rand() / div);
		printf("sin(%lf) = %lf \t\t", nrand, sin(nrand));
		matrix_t* in = malloc(sizeof(matrix_t));
		init_matrix(in, 1, 1);
		in->matrix[0][0] = nrand;
		matrix_t* res = predict(nn, in);
		printf("GOT: %lf\n", res->matrix[0][0]);
	}
	
	fclose(fh);
	free_data_set(data);
	free_net(nn);
}
