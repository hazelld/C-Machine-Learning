#ifndef _NET_INTERNAL_H_
#define _NET_INTERNAL_H_

#include "cml.h"
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


enum InputType {
	T_INT,
	T_DOUBLE,
	T_STR,
};

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
	cost_func_t costf; // Type of cost function 
} net;

#define NET_CONNECTED 1
#define NET_NOT_CONNECTED -1


/* Implementation of cml_data struct */
typedef struct cml_data {
	double* items;
	unsigned int count;
	unsigned int pos;
} cml_data;


/* struct data_pair
 *
 *	This structure contains two matrices, one for the input to the 
 *	net, and the expected output for the input. This allows for the 
 *	data to be passed around easier, and for parsing the data. 
 *
 *	Note this is not exposed at all in the public interface. 
 */
typedef struct data_pair {
	cml_data* input;
	cml_data* expected_output;
} data_pair;


/* Implementation of data_set */
typedef struct data_set {

	/* Holds all feature names */
	char** feature_names;
	enum InputType feature_types;
	int feature_count;
	
	/* Data */
	data_pair** data;
	int count;

	/* Holds the validation, test, and training sets, note these are just
	 * pointer arrays that point to elements inside data_set->data */
	data_pair** training_set;
	data_pair** validation_set;
	data_pair** test_set;
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


/* calculate_cost_func (cost.c)
 *
 *	This function is used to calculate the result of the cost function after 
 *	a feed forward pass. It is used to evaluate how good the network is performing.
 *	Currently the only cost functions available are those provided by the library, 
 *	and unlike the activation functions, these may not get custom cost functions 
 *	with callbacks. 
 *
 * Arguments:
 * 	net => Current neural network that was just fed forward
 * 	expected => Expected result of the net
 */
double calculate_cost_func(net* n, matrix_t* expected);


/* calculate_cost_gradient (cost.c)
 *
 *	This function is used to calculate the gradient of the cost function after 
 *	a feed forward pass. This is the error vector of the output layer.
 *
 * Arguments:
 * 	net => Current neural network that was just fed forward
 * 	expected => Expected output of neural network
 * 	result => Pointer to the matrix_t pointer that the error vector is put in
 */
error_t calculate_cost_gradient(net* n, matrix_t* expected, matrix_t** result);


/* data-builder.c
 *
 * Most of these functions are defined here as the user will generally build the data
 * set from the main building functions:
 * - data_set_from_csv()
 * - data_set_from_mysql()
 * - data_set_from_postgres()
 *
 * Should more control be needed, or a custom data building algorithm needed, they can 
 * simply include this file as well. 
 *
 */

/* TODO: docs */
error_t cml_data_to_matrix(cml_data* data, matrix_t** m);
error_t matrx_to_cml_data(matrix_t* m, cml_data** data);
data_pair* init_data_pair(cml_data* input, cml_data* output);
error_t add_data_pair (data_set* set, data_pair* pair);
error_t free_data_pair(data_pair* pair);

#endif
