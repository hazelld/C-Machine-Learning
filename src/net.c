#include <linux/random.h>
#include "net.h"

net* init_net (int lc, int* topology_arr, activation act, activation_prime actp) {
	net* nn = malloc(sizeof(net));

	nn->af = act;
	nn->ap = actp;
	nn->layer_count = lc;
	
	nn->topology = malloc(sizeof(int) * lc);
	nn->layers = malloc(sizeof(layer) * lc);
	
	for(int i = 0; i < lc; i++) {
		
		/* Determine Layer type */
		layer_type lt;

		if (i == 0) { lt = input; }
		else if (i == lc - 1) { lt = output; }
		else { lt = hidden; }
		
		/* The input layer has no inputs, it simply has outputs 
		 * (which are technically the inputs to the NN) */
		if (lc == input) { 
			nn->layers[i] = init_layer(input, 0, topology_arr[i]);
			continue;
		}

		nn->topology[i] = topology_arr[i];
		nn->layers[i] = init_layer(lt, topology_arr[i-1], topology_arr[i]);
	}
	return nn;
}

layer init_layer (layer_type lt, int in_node, int out_node) {
	layer l;

	l.ltype = lt;
	l.input_nodes = in_node;
	l.output_nodes = out_node;

	/* These matrices are not in use yet */
	l.input = NULL;
	l.output = NULL;
	l.layer_error = NULL;

	/* Output layer has no weights or bias */
	if (lt == input) {
		l.weights = NULL;
		l.bias = 0;
		return l;
	}

	/* Weight matrix has output_nodes columns and input_nodes rows */
	double interval = 1.0;
	l.weights = random_matrix(out_node, in_node, interval);
	l.bias = -interval + (rand() / (RAND_MAX / (interval * 2))); 
	return l;
}

int feed_forward (net* n, matrix_t* input) {
	//assert that n->topology[0] == input->columns
	layer* clayer;
	n->layers[1].input = input;
	
	for (int i = 1; i < n->layer_count; i++) {
		clayer = &n->layers[i];
		
		/* Free up old memory */
		if (clayer->output) { free_matrix(clayer->output); }
		if (clayer->layer_error) { free_matrix(clayer->layer_error); }

		clayer->output = matrix_vector_dot(clayer->weights, clayer->input);
		vector_scalar_addition(clayer->output, clayer->bias);
		function_on_vector(clayer->output, n->af);
		
		if (clayer->ltype != output) 
			n->layers[i+1].input = clayer->output;
	}
	
	return 0;
}

int free_net (net* n) {
	for (int i = 0; i < n->layer_count; i++) 
		free_layer(&n->layers[i]);
	free(n->topology);
	free(n);
	return 0;
}


int free_layer (layer* l) {
	free_matrix(l->input);
	free_matrix(l->output);
	free_matrix(l->weights);
	free_matrix(l->layer_error);
	free(l);
	return 0;
}
