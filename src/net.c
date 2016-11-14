#include <linux/random.h>
#include "net.h"

int init_net (net* nn, int lc, int* topology_arr, activation act, activation_prime actp) {
	if (nn == NULL) return 1;
	nn->af = act;
	nn->ap = actp;
	nn->layer_count = lc;
	
	nn->topology = malloc(sizeof(int) * lc);
	nn->layers = malloc(sizeof(layer) * lc);
	
	for(int i = 0; i < lc; i++) {
		
		/* Allocate space for layer */
		nn->layers[i] = malloc(sizeof(layer));

		/* Determine Layer type */
		layer_type lt;

		if (i == 0) { lt = input; }
		else if (i == lc - 1) { lt = output; }
		else { lt = hidden; }
		
		/* The input layer has no inputs, it simply has outputs 
		 * (which are technically the inputs to the NN) */
		if (lt == input) { 
			init_layer(nn->layers[i], input, 0, topology_arr[i]);
			continue;
		}

		nn->topology[i] = topology_arr[i];
		init_layer(nn->layers[i], lt, topology_arr[i-1], topology_arr[i]);
	}
	return 0;
}

int init_layer (layer* l, layer_type lt, int in_node, int out_node) {
	if (l == NULL) return 1;
	
	l->ltype = lt;
	l->input_nodes = in_node;
	l->output_nodes = out_node;

	/* These matrices are not in use yet */
	l->input = NULL;
	l->output = NULL;
	l->layer_error = NULL;

	/* Output layer has no weights or bias */
	if (lt == input) {
		l->weights = NULL;
		l->bias = 0;
		return 0;
	}

	/* Weight matrix has output_nodes columns and input_nodes rows */
	double interval = 1.0;
	l->weights = random_matrix(out_node, in_node, interval);
	l->bias = -interval + (rand() / (RAND_MAX / interval * 2)); 
	return 0;
}

int feed_forward (net* n, matrix_t* input) {
	//assert that n->topology[0] == input->columns
	layer* clayer;
	n->layers[1]->input = input;
	
	for (int i = 1; i < n->layer_count; i++) {
		clayer = n->layers[i];
		
		/* Free up old memory */
		if (clayer->output) { 
			free_matrix(clayer->output); 
			clayer->output = NULL;
		}

		if (clayer->layer_error) { 
			free_matrix(clayer->layer_error); 
			clayer->layer_error = NULL;
		}

		clayer->output = matrix_vector_dot(clayer->weights, clayer->input);
		vector_scalar_addition(clayer->output, clayer->bias);
		function_on_vector(clayer->output, n->af);
		
		if (clayer->ltype != output) 
			n->layers[i+1]->input = clayer->output;
	}
	
	return 0;
}

int free_net (net* n) {
	for (int i = 0; i < n->layer_count; i++) 
		free_layer(n->layers[i]);
	free(n->layers);
	free(n->topology);
	free(n);
	return 0;
}


/* NOTE:
 *
 * layer->input is never free'd because it is never allocated. It only
 * ever points the previous layer's output which will be free'd by 
 * layer->output. Or, it is the input for the whole neural net so it 
 * will be the responsibility of the creator of the net to dealloc.
 *
 */
int free_layer (layer* l) {
	if (l == NULL)
		return 1;

	free_matrix(l->output);
	free_matrix(l->weights);
	free_matrix(l->layer_error);
	free(l);
	return 0;
}
