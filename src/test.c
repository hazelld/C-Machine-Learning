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
	printf("\n");
}

double learning_weight(double val) {
	return val * 0.5;
}

double sigmoid (double val) {
	double ret = 1 / (1 + exp(-val));
	return ret;
}

double sigmoid_prime (double val) {
	return val * (1 - val);
}

int main() {
	int topology[3] = { 2, 3, 2 };
	net* nn = malloc(sizeof(net));
	init_net(nn, 3, topology, sigmoid, sigmoid_prime);
	
	printf("Weight matrices:\n");
	print_m(nn->layers[1]->weights);
	print_m(nn->layers[2]->weights);

	printf("Biases:\n");
	printf("%lf\n%lf\n\n", nn->layers[1]->bias, nn->layers[2]->bias);
	
	matrix_t* input = random_matrix (2, 1, 1);
	printf("Inputs:\n");
	print_m(input);
	feed_forward(nn, input);
	
	printf("Resulting feedforward: %lf\n\n", nn->layers[2]->input->matrix[0][0]);
	
	matrix_t* expected = random_matrix(2,1,1);
	printf("Expected Output:\n");
	print_m(expected);

	net_error(nn, expected);
	
	printf("Error of output layer:\n");
	print_m(nn->layers[2]->layer_error);
	
	printf("Error of hidden layer:\n");
	print_m(nn->layers[1]->layer_error);
	
	update_weights(nn);
	printf("New Weights:\n");
	print_m(nn->layers[1]->weights);
	print_m(nn->layers[2]->weights);
	

	free_net(nn);
	free_matrix(input);
}
