#ifndef _NET_H_
#define _NET_H_

#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "err.h"
#include "activation.h"

/* enum layer_type
 *
 * 	This enum defines the different type of possible layers the net can have. 
 */
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
	int using_bias;
	double bias;
	matrix_t* input;
	matrix_t* weights;
	matrix_t* output;
	matrix_t* layer_error;
	matrix_t* weight_delta;
	activation_f actf;
} layer;


/*	struct net
 *
 *	This defines the whole net. It has an array of layers, and the count
 *	of layers.
 *
 * 	This is the main data structure that is used throughout the library. This 
 * 	should be created with the 
 */
typedef struct net {
	layer** layers;
	int layer_count;
	int* topology;
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



/*	Public Functions */

/* init_net
 *
 *	This function is used to initialize the neural network datastructure, and
 *	allocate all the memory it needs. If the given pointer points to NULL, the 
 *	memory for the net will be allocated.
 *
 * Arguments:
 *	net** => Pointer to the net. If the pointed to net is not already allocated,
 *	the net will be allocated. 
 *
 * Returns the pointer to the net that was initialized. If there was any problem, 
 * this function returns NULL.
 *
 */
net* init_net (net** n);


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
error_t init_layer (layer* l, layer_type lt, int in_node, int out_node);


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
error_t train (net* n, data_set* data, int epochs);


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
error_t free_net(net* n);


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
error_t free_layer(layer* l);


/* These functions are within net-builder.c but will be exposed through this header. */

/* build_layer
 *
 * 	This function is to build a single layer of the neural network. The struct layer 
 * 	to build must be allocated before calling this function.
 *
 * Arguments:
 * 	layer* -> Pointer to layer 
 * 	layer_type -> Type of layer (input, hidden, output). Note that the network may
 * 		only have a single input and output layer.
 *
 *	bias -> Defines if a bias is needed for this layer or not. Note that by default,
 *			the input layer _will not_ have a bias. This can be over-riden, although
 *			I am not sure why you would want to.
 *
 *	nodes -> The amount of nodes this layer should have.
 *		
 *	af -> Activation function to use
 *	ap -> Derivative of activation function
 *
 * Returns:
 *	E_SUCCESS -> Successfully created layer
 */
error_t build_layer (layer* l, layer_type lt, int bias, int nodes, activation_f actf);


/* add_layer
 *
 *	This function adds a pre-defined layer into the neural network. The order that the 
 *	layers are added in does not matter, as the order is sorted out once the net_build() 
 *	function is called. It will check to make sure that an extra input/output layer
 *	is not added.
 *
 * Arguments: 
 *	net* -> The pointer to the neural network that the layer is added too
 *	layer* -> The layer to add to the net
 *
 * Returns:
 * 	E_SUCCESS -> Added the layer successfully
 *
 * 	E_TOO_MANY_INPUT_LAYERS -> There is already an input layer in the net.
 * 	E_TOO_MANY_OUTPUT_LAYERS-> There is already an output layer in the net.
 *
 * Note: When an error occurs, nothing is added to the neural network, and no memory
 * is deallocated.
 */
error_t add_layer (net* nn, layer* l);


/* connect_net
 *
 * 	This function is used once all the needed layers have been added. This will connect
 * 	each layer together within the neural net. 
 *
 * Arguments:
 * 	net* -> The pointer to the neural network that has all layers added
 *	
 * Returns:
 * 	E_SUCCESS -> The net has been built and is ready to use 
 *
 * Note: If this function returns anything but the E_SUCCESS, it is not ready to use, and 
 * other functions should not be called with it.
 *
 */
error_t connect_net (net* nn);

#endif

