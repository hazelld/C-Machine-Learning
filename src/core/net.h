#ifndef _NET_H_
#define _NET_H_

/* TODO: See if matrix.h can be removed from here by refactoring predict() */
#include "matrix.h"
#include "err.h"

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
typedef struct layer layer;


/*	struct net
 *
 *	This defines the whole net. It has an array of layers, and the count
 *	of layers.
 *
 * 	This is the main data structure that is used throughout the library. This 
 * 	should be created with the 
 */
typedef struct net net;


/* struct data_set
 *
 *	This is simply an array of data_pairs that has an associated count.
 *	The purpose of this is to allow for building functions that parse 
 *	data sets and return just this struct, instead of an array of data_pairs
 *	and the count.
 *
 */
typedef struct data_set data_set;


/* struct activation_f
 *	
 *	This structure is used to store the activation function, and it's derivative 
 *	function. This should be built with the 
 *
 */
typedef struct activation_f activation_f;


/* activation_functions
 *
 * 	These are the type of activation functions. As more are added, this will 
 * 	change to reflect that. 
 *
 * 	If the CUSTOM is selected, then the user will provide callbacks to their own 
 * 	activation function, and derivative of that function.
 *
 */
typedef enum activation_functions {
	SIGMOID,
	TANH,
	CUSTOM,
} act_func_t;


/*	This defines the signature needed for any custom activation functions or their
 *	derivatives. 
 */
typedef double (*act_func)(double);


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
 *
 *	TODO: move to net-internal.h?
 */
error_t free_layer(layer* l);


/* These functions are within net-builder.c but will be exposed through this header. */

/* build_layer
 *
 *  TODO: FIX THIS HEADER 
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
layer* build_layer (layer** l, layer_type lt, int bias, int nodes, activation_f actf);


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


/* These functions are defined inside activation.c */

/* get_activation_f
 *
 * 	This function is used to create an activation function data structure.
 *	
 * Arguments:
 *	activation_f* -> Location to put everything into.
 *	type -> Type of activation function
 *	af -> Pointer to activation function, only needed when type is CUSTOM
 *	ap -> Pointer to the activation function's derivative, only needed when type is CUSTOM
 *
 * Note i: If type is not CUSTOM, then af and ap are ignored. 
 * Note ii: Generally the structure passed in should be on the stack not the heap. Since 
 * 	the layer structure holds the structure and not the pointer, it works best as:
 *
 * 	activation_f actf;
 * 	get_activation_f(&actf, .....);
 * 	build_layer(...., actf);
 *
 * Returns:
 * 	E_SUCCESS => Built the structure OK, safe to use
 * 	E_NULL_ARG => Null argument given
 * 	E_NO_CALLBACK_GIVEN => Selected CUSTOM type, but no activation function pointers given.
 */
error_t get_activation_f (activation_f* actf, act_func_t type, act_func af, act_func ap );


#endif

