#ifndef _MATRIX_BUILDER_H_
#define _MATRIX_BUILDER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"
#include "net.h"
#include "err.h"

#define MAXLINELEN 256

/*	Below are the functions used to build a neural net (struct net) that is customized
 *	to meet the needs. Each layer must be built seperately and added into the struct net.
 *
 */


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
error_t build_layer (layer* l, layer_type lt, int bias, int nodes, act_f af, act_prime_f ap);


/* add_layer
 *
 *	This function adds a pre-defined layer into the neural network. The order that the 
 *	layers are added in does not matter, as the order is sorted out once the net_sort() 
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



/* Not quite sure where the functions below stand. I want to move most of the 
 * data set stuff into python.
 */


/* 
 *
 */
data_set* data_set_from_csv (FILE* fh); 


/*
 *
 */
int free_data_set(data_set* data);

/*
 *
 */
int load_net (net* n, FILE* fh);


/*
 *
 */
int save_net (net* n, FILE* fh);


#endif
