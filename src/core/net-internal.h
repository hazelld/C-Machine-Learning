#ifndef _NET_INTERNAL_H_
#define _NET_INTERNAL_H_

#include "net.h"
#include "matrix.h"

/*	This header file is to expose the internal data structures used by the library to
 *	all the different modules. This allows for the public header (net.h) to have 
 *	opaque data types for all the things that may change often. Certain data type definitions
 *	are still within the public header, however these should remain static (layer_type for 
 *	example).
 *
 *	It also contains the functions that may be used within the library, but shouldn't be called
 *	externally. For example, init_layer() has useful functionality that the build_layer() 
 *	function can use, but init_layer() shouldn't be publically accessible.
 */

/* Implementation of activation_f */
typedef struct activation_f {
	act_func_t type;
	act_func af;
	act_func ap;
} activation_f;

/* Implementation of layer structure */
typedef struct layer {
	layer_type ltype;
	int input_nodes;
	int output_nodes;
	int using_bias;
	double bias;
	matrix_t* input;
	matrix_t* output;
	matrix_t* weights;
	matrix_t* layer_error;
	matrix_t* weight_delta;
	activation_f actf;
} layer;


/* Implementation of net structure */
typedef struct net {
	layer** layers;
	int layer_count;
	int* topology;
	double learning_rate;
	int connected; // See defines below
} net;

#define NET_CONNECTED 1
#define NET_NOT_CONNECTED -1

/* struct data_pair
 *
 *	This structure contains two matrices, one for the input to the 
 *	net, and the expected output for the input. This allows for the 
 *	data to be passed around easier, and for parsing the data. 
 *
 *	Note this is not exposed at all in the public interface. 
 */
typedef struct data_pair {
	matrix_t* input;
	matrix_t* expected_output;
} data_pair;


/* Implementation of data_set */
typedef struct data_set {

	/* Holds all data */ 
	data_pair** data;
	int count;

	/* Holds the validation sets and training sets, note these are just
	 * pointer arrays that point to elements inside data_set->data */
	data_pair** training_set;
	data_pair** validation_set;
	int training_count;

} data_set;


/* init_layer (net.c)
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



#endif