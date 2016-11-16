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
	layer** layers;
	int layer_count;
	int* topology;
	activation af;
	activation_prime ap;
} net;

int init_net (net* n,int lc, int* topology_arr, activation act, activation_prime actp);
int init_layer (layer* l, layer_type lt, int in_node, int out_node);
int feed_forward (net* n, matrix_t* input);
int net_error(net* n, matrix_t* expected);
int update_weights(net* n);
int update_bias(net* n);
int free_net(net* n);
int free_layer(layer* l);

#endif

