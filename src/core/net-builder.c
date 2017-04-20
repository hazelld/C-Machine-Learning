#include <stdlib.h>
#include "net.h"
#include "err.h"


/* Local functions */
static error_t find_and_move_layer (net* n, layer_type type, int index); 
static void swap_layers (net* n, int index1, int index2);

/* build_layer() */
error_t build_layer (layer* l, layer_type lt, int bias, int nodes, activation_f actf) {
	if (l == NULL)
		return E_NULL_ARG;
	
	/* If no activation function package is given, default to sigmoid */
	l->actf = actf;

	/* Only set the variables we need */
	l->ltype = lt;
	l->output_nodes = lt;
	l->using_bias = bias;
	
	return E_SUCCESS;
}

/* add_layer */
error_t add_layer (net* nn, layer* l) {
	if (nn == NULL || l == NULL)
		return E_NULL_ARG;

	/* Check to make sure we aren't adding extra input/output layer */
	layer_type added_type = l->ltype;
	for (int i = 0; i < nn->layer_count; i++) {
		if (nn->layers[i]->ltype == added_type && added_type == input)
			return E_TOO_MANY_INPUT_LAYERS;
		
		if (nn->layers[i]->ltype == added_type && added_type == output)
			return E_TOO_MANY_OUTPUT_LAYERS;
	}

	/* Add room for 1 more layer, and put given argument in */
	nn->layers = realloc(nn->layers, sizeof(layer*) * ++nn->layer_count);
	nn->layers[nn->layer_count - 1] = l;
	return E_SUCCESS;
}


/* connect_net */
error_t connect_net (net* n) {
	error_t err;
	if (n == NULL) return E_NULL_ARG;

	/* Put the input layer first */
	err = find_and_move_layer(n, input, 0);
	if (err != E_SUCCESS) return err;

	/* Put the output layer last */
	err = find_and_move_layer(n, output, (n->layer_count - 1));
	if (err != E_SUCCESS) return err;
	
	/* TODO: figure out how to handle hidden layers */
	

	/* Loop backwards through the net, fill out each layer with needed info */
	for (int i = n->layer_count - 1; i > 0; i++) {
		layer* clayer = n->layers[i];
		layer* prev_layer = n->layers[i-1];
		init_layer(clayer, clayer->ltype, prev_layer->output_nodes, clayer->input_nodes);
	}
}


static error_t find_and_move_layer (net* n, layer_type type, int index) {
	
	for (int i = 0; i < n->layer_count; i++) {
		if (n->layers[i]->ltype == type) {
			swap_layers(n, i, index);
			return E_SUCCESS;
		}
	}

	/* If we make it here then we have no layers of the given type. */
	switch(type) {
		case input:
			return E_NO_INPUT_LAYER;

		/* If there is no hidden layer thats ok, but the net may suck */
		case hidden:
			return E_SUCCESS;

		case output:
			return E_NO_OUTPUT_LAYER;
	}

	/* Switch statement is exhaustive, don't need no more code here */
}


static void swap_layers (net* n, int index1, int index2) {
	
	if (index1 > n->layer_count || index2 > n->layer_count)
		return;

	layer* buff = n->layers[index1];
	n->layers[index1] = n->layers[index2];
	n->layers[index2] = buff;
}
