#include <math.h>
#include "matrix.h"
#include "net.h"

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
	
	/* Manually set up the input */
	matrix_t* input1 = malloc(sizeof(matrix_t));
	matrix_t* input2 = malloc(sizeof(matrix_t));
	matrix_t* input3 = malloc(sizeof(matrix_t));
	matrix_t* input4 = malloc(sizeof(matrix_t));
	
	matrix_t* e1 = malloc(sizeof(matrix_t));
	matrix_t* e2 = malloc(sizeof(matrix_t));
	matrix_t* e3 = malloc(sizeof(matrix_t));
	matrix_t* e4 = malloc(sizeof(matrix_t));

	init_matrix(input1, 2, 1);
	init_matrix(input2, 2, 1);
	init_matrix(input3, 2, 1);
	init_matrix(input4, 2, 1);
	init_matrix(e1, 1, 1);
	init_matrix(e2, 1, 1);
	init_matrix(e3, 1, 1);
	init_matrix(e4, 1, 1);

	input1->matrix[0][0] = 0;
	input1->matrix[1][0] = 0;
	e1->matrix[0][0] = 1;

	input2->matrix[0][0] = 1;
	input2->matrix[1][0] = 0;
	e2->matrix[0][0] = 0;
	
	input3->matrix[0][0] = 0;
	input3->matrix[1][0] = 1;
	e3->matrix[0][0] = 0;
	
	input4->matrix[0][0] = 1;
	input4->matrix[1][0] = 1;
	e4->matrix[0][0] = 1;
	
	init_net(nn, 3, topology, sigmoid, sigmoid_prime, 0.3);
	
	matrix_t* inputs[4] = { input1, input2, input3, input4 };
	matrix_t* exp[4] = { e1, e2, e3, e4 };

	train(nn, inputs, exp, 3000, 4);


	printf("\n\nRESULTS:\n");
	for (int i = 0; i < 4; i++) {
		matrix_t* res = predict(nn, inputs[i]);
		printf("%d %d = %lf\n", (int)inputs[i]->matrix[0][0], (int)inputs[i]->matrix[1][0], res->matrix[0][0]);
	}

	free_net(nn);

	free_matrix(input1);
	free_matrix(input2);
	free_matrix(input3);
	free_matrix(input4);

	free_matrix(e1);
	free_matrix(e2);
	free_matrix(e3);
	free_matrix(e4);
}
