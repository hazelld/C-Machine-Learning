#ifndef _NET_H_
#define _NET_H_

#include <stdlib.h>
#include <math.h>
#include "matrix.h"

typedef double (*act_f)(double);
typedef double (*act_prime_f)(double);

typedef enum layer_type {
	input,
	hidden,
	output,
} layer_type;

/*	struct layer
 *
 *	The layer defines the nodes at the current layer, and the weights 
 *	from the previous layer. That is, if we have the layers ijk, the 
 *	layer k defines the weights wjk. 
 *
 * 	The input matrix just points to the previous layer's output matrix,
 * 	to make it more accessible. That way when we only need the one layer
 * 	to do all the needed operations on.
 *
 */
typedef struct layer {
	layer_type ltype;
	int input_nodes;
	int output_nodes;
	double bias;
	matrix_t* input;
	matrix_t* weights;
	matrix_t* output;
	matrix_t* layer_error;
	matrix_t* weight_delta;
} layer;

/*	struct net
 *
 *	This defines the whole net. It has an array of layers, and the count
 *	of layers. The activation function and activation derivative function
 *	are pointers to functions.
 *
 *	The topology array is to keep track of the nodes per layer. If you want
 *	a 2-5-1 net, this array would be:
 *	{ 2, 5, 1 }
 *
 *	It is important the size of the topology_arr is equal to layer_count
 *
 */
typedef struct net {
	layer** layers;
	int layer_count;
	int* topology;
	act_f af;
	act_prime_f ap;
} net;


/* struct data_pair
 *
 *	This structure contains two matrices, one for the input to the 
 *	net, and the expected output for the input. This allows for the 
 *	data to be passed around easier, and for parsing the data. 
 */
typedef struct data_pair {
	matrix_t* input;
	matrix_t* expected_output;
} data_pair;


/* struct data_set
 *
 *	This is simply an array of data_pairs that has an associated count.
 *	The purpose of this is to allow for building functions that parse 
 *	data sets and return just this struct, instead of an array of data_pairs
 *	and the count.
 *
 */
typedef struct data_set {
	data_pair** data;
	int count;
} data_set;

/*	Public Functions	*/
/* init_net
 *
 * 	This function is used to initialize an allocated struct net with the 
 * 	appropriate values given. 
 *
 * 	Arguments:
 * 		n => Pointer to an already allocated struct net
 * 		lc => The amount of layers your net will have
 * 		topology_arr => Array with the amount of nodes per layer
 * 		act => Pointer to an activation function
 *		actp => Pointer to the activation function's derivative
 *		lr => Learning rate you would like the net to use
 *
 *	Returns:
 *		0 => Net was successfully initialized
 *		1 => Error initializing net
 *
 *	Memory Allocated:
 *		net->topology
 *		net->layers
 *		net->layers[i]
 *	
 */
int init_net (net* n,int lc, int* topology_arr, act_f act, act_prime_f actp, double lr);


/* init_layer
 *
 * 	This function is used to initialize an allocated struct layer with the
 * 	given values.
 *
 * 	Arguments:
 * 		l => Pointer to allocated struct layer
 * 		lt => The layer type as defined by enum layer_type
 * 		in_node => The amount of nodes in the previous layer
 * 		out_node => The amount of nodes in the current layer
 *
 *	Returns:
 *		0 => Layer successfully initialized 
 *		1 => Error initializing layer
 *
 *	Memory Allocated:
 *		l->weights (Through random_matrix())
 *
 */
int init_layer (layer* l, layer_type lt, int in_node, int out_node);


/* train
 *	
 *	This function is used to train a neural network with the given inputs, 
 *	for the designated amount of epochs. 
 *
 *	Arguments:
 *		n => Neural Network to train
 *		inputs => Array of matrix_t* that are the inputs
 *		epochs => How many epochs the net should train for
 *		input_count => Count of how many inputs are given
 *
 *	Returns:
 *		0 => Training was successful 
 *		1 => Error occured during training
 *
 *	Memory Allocated:
 *		NONE
 */
int train (net* n, data_set* data, int epochs);


/* predict
 *
 *	This function is used to predict a given value once the network has
 *	been trained. It will return the result of the forward propogation.
 *	
 *	Arguments:
 *		n => struct net that has been trained using train()
 *		input => The input to predict the result of
 *	
 *	Returns:
 *		Pointer to the output matrix. Note this pointer will only exist
 *		until free_net() is called. Once free_net() is called this pointer
 *		will not be valid.
 *
 * 	Memory Allocated:
 * 		NONE
 */
matrix_t* predict (net* n, matrix_t* input);


/* free_net
 *
 *	This function frees all memory that the struct net 
 *	contains.
 *
 *	Arguments: 
 *		n => struct net to free
 *
 *	Returns:
 *		0 => All memory successfully free'd
 *		1 => Invalid pointer passed to function
 *
 *	Memory Allocated:
 *		NONE
 */
int free_net(net* n);


/* free_layer
 * 	
 * 	This function frees all memory that the struct layer
 * 	contains.
 *
 *	Arguments: 
 *		l => struct layer to free
 *
 *	Returns:
 *		0 => All memory successfully free'd
 *		1 => Invalid pointer passed to function
 *	
 *	Memory Allocated:
 *		NONE
 */
int free_layer(layer* l);

#endif

