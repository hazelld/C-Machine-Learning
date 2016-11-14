#ifndef _NET_H_
#define _NET_H_

#include <stdlib.h>
#include "matrix.h"

typedef double (*activation)(double);
typedef double (*activation_prime)(double);

typedef enum layer_type {
	input,
	hidden,
	output,
} layer_type;

typedef struct layer {
	layer_type ltype;
	int input_nodes;
	int output_nodes;
	double bias;
	matrix_t* input;
	matrix_t* weights;
	matrix_t* output;
	matrix_t* layer_error;
} layer;

typedef struct net {
	layer* layers;
	int layer_count;
	int* topology;
	activation af;
	activation_prime ap;
} net;

net* init_net (int lc, int* topology_arr, activation act, activation_prime actp);
layer init_layer (layer_type lt, int in_node, int out_node);
int feed_forward (net* n, matrix_t* input);
int free_net(net* n);
int free_layer(layer* l);

#endif
