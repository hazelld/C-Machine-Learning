#include "matrix.h"

void print_m (matrix_t* m) {
	for (int i = 0; i < m->rows; i++) {
		for (int j = 0; j < m->columns; j++) {
			printf("%lf ", m->matrix[i][j]);
		}
		printf("\n");
	}
}

int main() {

	matrix_t* w = init_matrix(2,2);
	
	w->matrix[0][0] = 0.15;
	w->matrix[0][1] = 0.20;
	w->matrix[1][0] = 0.25;
	w->matrix[1][1] = 0.30;
	
	print_m(w);
	printf("\n");

	matrix_t* v = init_matrix(2, 1);
	v->matrix[0][0] = 0.05;
	v->matrix[1][0] = 0.1;

	print_m(v);
	printf("\n");
	
	matrix_t* res = matrix_vector_product(w, v);
	print_m(res);
	printf("\n");

	double b = 0.35;
	vector_scalar_addition(res, b);
	print_m(res);
}

