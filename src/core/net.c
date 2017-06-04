#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "cml.h"
#include "cml-internal.h"
#include "data-builder.h"

/* Local functions */
static error_t feed_forward(net* n, matrix_t* input);
static error_t backprop (net* n, matrix_t* expected); 
static error_t net_error(net* n, matrix_t* expected);
static error_t update_weights(net* n);
static error_t update_bias(net* n);
static error_t calc_test_error(net* n, data_set* ds, double* total_err, double* avg_err);

/* PUBLIC FUNCTIONS */

/* init_net() */
net* init_net (double learning_rate, cost_func_t costf) 
{
	net* n = malloc(sizeof(net));

	if (n == NULL)
		return NULL;
	
	memset(n, 0, sizeof(net));
	n->connected = NET_NOT_CONNECTED;
	n->learning_rate = learning_rate;
	n->costf = costf;

	n->topology = NULL;
	n->layers = NULL;
	return n;
}


/* init_layer() [net-internal.h] */
error_t init_layer (layer* l, layer_type lt, int in_node, int out_node) 
{
	if (l == NULL) return E_NULL_ARG;
	
	l->ltype = lt;
	l->input_nodes = in_node;
	l->output_nodes = out_node;

	/* These matrices are not in use yet */
	l->input = NULL;
	l->output = NULL;
	l->layer_error = NULL;
	l->weight_delta = NULL;
	
	/* Output layer has no weights or bias */
	if (lt == input) {
		l->weights = NULL;
		l->bias = 0;
	} else {
		/* Weight matrix has output_nodes columns and input_nodes rows */
		double interval = 0.5;
		l->weights = random_matrix(out_node, in_node, interval);
		l->bias = -interval + (rand() / (RAND_MAX / interval * 2)); 
	}

	return E_SUCCESS;
}


/* TODO:
 * -> Add verbose mode
 * -> Return error on unconnected net 
 */
error_t train (net* n, data_set* data, int epochs) 
{
	double total_err = 0.0;
	double avg_err = 0.0;
	
	if (n == NULL || data == NULL) 
		return E_NULL_ARG;

	if (n->connected != NET_CONNECTED)
		return E_NET_NOT_CONNECTED;

	for (int j = 0; j < epochs; j++) {
		fprintf(stderr, "Training epoch: %d\t", j);
		for (int i = 0; i < data->count; i++) {
			error_t e;
			matrix_t* input = NULL;
			matrix_t* expected_output = NULL;

			e = cml_data_to_matrix(data->data[i]->input, &input);
			if (e != E_SUCCESS) return e;

			e = cml_data_to_matrix(data->data[i]->expected_output, &expected_output);
			if (e != E_SUCCESS) return e;

			e = feed_forward(n, input);
			if (e != E_SUCCESS) return e;

			e = backprop(n, expected_output);
			if (e != E_SUCCESS) return e;
			
			free_matrix(input);
			free_matrix(expected_output);
		}

		/* Test against the test data if the user wants to */
		if (data->test_count > 0) {
			error_t err = calc_test_error(n, data, &total_err, &avg_err);
			if (err != E_SUCCESS) return err;

			fprintf(stderr, "Total error: %lf\tAverage error: %lf\n", total_err, avg_err);
			total_err = 0.0;
		} else {
			fprintf(stderr, "\n");
		}
	}

	return E_SUCCESS;
}


/* TODO: Fix function to better handle errors */
cml_data* predict (net* n, cml_data* input) 
{
	int last_layer = n->layer_count - 1;

	matrix_t* input_matrix = NULL;
	cml_data_to_matrix(input, &input_matrix);	
	error_t e = feed_forward(n, input_matrix);

	if (e != E_SUCCESS) {
		// HANDLE ERR
		printf("Error %d in predict()\n", (int)e);
	}
	
	cml_data* data = NULL;
	matrix_to_cml_data(n->layers[last_layer]->output, &data);
	free_matrix(input_matrix);
	return data;
}


/* free_net() */
error_t free_net (net* n) 
{
	if (n == NULL)
		return E_NULL_ARG;

	for (int i = 0; i < n->layer_count; i++) 
		free_layer(n->layers[i]);
	free(n->layers);
	free(n->topology);
	free(n);
	return E_SUCCESS;
}


/* NOTE:
 *
 * layer->input is never free'd because it is never allocated. It only
 * ever points the previous layer's output which will be free'd by 
 * layer->output. Or, it is the input for the whole neural net so it 
 * will be the responsibility of the creator of the net to dealloc.
 *
 */
error_t free_layer (layer* l) 
{
	if (l == NULL)
		return E_NULL_ARG;

	free_matrix(l->output);
	free_matrix(l->weights);
	free_matrix(l->layer_error);
	free(l);
	return E_SUCCESS;
}


/* feed_forward
 *
 *	This function is used to feed an input through the net. It is 
 *	important that the defined input topology (n->topology[0]) has the
 *	same amount of rows that the input vector has. 
 *
 *	Arguments:
 *	net => Neural network
 *	input => Input matrix to feed through the net
 *
 */
static error_t feed_forward (net* n, matrix_t* input) 
{
	layer* clayer;
	
	if (n == NULL || input == NULL)
		return E_NULL_ARG;

	if (n->topology[0] != input->rows)
		return E_WRONG_INPUT_SIZE;

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

		clayer->output = malloc(sizeof(matrix_t));

		error_t err = matrix_vector_mult(clayer->weights, clayer->input, &clayer->output); 
		if (err != E_SUCCESS) return err;

		/* Check if we have bias to add */
		if (clayer->using_bias)
			vector_scalar_addition(clayer->output, clayer->bias);
		
		map_vector(clayer->output, clayer->actf.af);
		
		if (clayer->ltype != output) 
			n->layers[i+1]->input = clayer->output;
	}

	return E_SUCCESS;
}


/* backprop
 *
 * 	This function is used to provide a single interface 
 * 	to the steps needed to back propogate the network. 
 *
 * 	Arguments:
 * 	n => Neural Network
 * 	expected => Expected value after the feed forward
 *
 * 	Returns:
 * 	SUCCESS => Successfully backproped the network
 * 	FAILURE => Could not backprop the network
 */
static error_t backprop (net* n, matrix_t* expected) 
{
	error_t e;

	/* Check for NULL args */
	if (n == NULL || expected == NULL)
		return E_NULL_ARG;
	
	/* Make sure the expected vector has same dim as output */
	if (n->topology[n->layer_count - 1] != expected->rows)
		return E_WRONG_OUTPUT_SIZE;

	if ((e = net_error(n, expected)) != E_SUCCESS) return e;
	if ((e = update_weights(n)) != E_SUCCESS) return e;
	if ((e = update_bias(n)) != E_SUCCESS) return e;
	
	return E_SUCCESS;
}


/* net_error
 *	
 *	This function calculates the weight delta matrix based
 *	off the error of each layer.
 *
 * 	Arguments:
 * 	n => Neural Network
 * 	expected => The matrix with the expected result 
 *
 * 	Returns:
 * 	SUCCESS => Calculated error successfully
 * 	FAILURE => Could not calculate error 
 */
static error_t net_error (net* n, matrix_t* expected) 
{

	/* Backprop has already checked, but lets check again! */
	if (n == NULL || expected == NULL)
		return E_NULL_ARG;

	/* Calculate the error of the other layers, excluding the dummy 
	 * input layer */
	for (int i = n->layer_count-1; i > 0; i--) {
		layer* clayer = n->layers[i];
		matrix_t* buff_err = malloc(sizeof(matrix_t));
		matrix_t* tweights = NULL;
		error_t err = E_SUCCESS;

		if (clayer->ltype != output) {
			layer* nlayer = n->layers[i+1];
			tweights = transpose_r(nlayer->weights);
			err = matrix_vector_mult(tweights, nlayer->layer_error, &buff_err);
		} else {
			err = calculate_cost_gradient(n, expected, &buff_err);
		}

		if (err != E_SUCCESS) return err;
		/* g'(z) */	
		map_vector(clayer->output, clayer->actf.ap);
		
		/* S * g'(z) */
		clayer->layer_error = malloc(sizeof(matrix_t));
		err = multiply_vector(buff_err, clayer->output, &clayer->layer_error);
		if (err != E_SUCCESS) return err;
		matrix_t* transposed_input = transpose_r(clayer->input);
		
		/* Get weight delta matrix */
		clayer->weight_delta = malloc(sizeof(matrix_t));
		err = kronecker_vectors(clayer->layer_error, transposed_input, &clayer->weight_delta);
		if (err != E_SUCCESS) return err;

		free_matrix(transposed_input);
		free_matrix(tweights);
		free_matrix(buff_err);
	}

	return E_SUCCESS;
}


/* update_weights
 *
 * 	This function is used to update the weights by the values of their 
 * 	weight delta matrix. It should be called after the net_error() function.
 *
 * 	Returns:
 * 	SUCCESS => Updated weights successfully 
 * 	FAILURE => Could not update weights
 */
static error_t update_weights (net* n) 
{
	for (int i = 1; i < n->layer_count; i++) {
		layer* clayer = n->layers[i];
		matrix_t* f_weights = malloc(sizeof(matrix_t));
	
		error_t err;
		err = matrix_scalar_mult(clayer->weight_delta, n->learning_rate);
		if (err != E_SUCCESS) return err;

		err = matrix_subtraction(clayer->weights, clayer->weight_delta, &f_weights);
		if (err != E_SUCCESS) return err;

		/* Free old memory */
		free_matrix(clayer->weights);
		free_matrix(clayer->weight_delta);
		
		/* Set new weights */
		clayer->weights = f_weights;
	}	
	return E_SUCCESS;
}


/* update_bias
 *
 * 	This function is used to update the network biases. It 
 * 	should be called after the net_error() function.
 *
 * 	Returns:
 * 	SUCCESS => Updated the biases
 * 	FAILURE => Could not update the weights
 */
static error_t update_bias (net* n) 
{
	if (n == NULL)
		return E_NULL_ARG;

	for (int i = 1; i < n->layer_count; i++) {
		double error_sum = 0;
		layer* clayer = n->layers[i];
		
		/* If the layer doesn't use bias, move to the next */
		if (!clayer->using_bias)
			continue;

		for (int j = 0; j < clayer->layer_error->rows; j++) 
			error_sum += clayer->layer_error->matrix[j][0];
		
		clayer->bias -= error_sum;
	}
	return E_SUCCESS;
}


/*
 *
 *
 */
static error_t calc_test_error(net* n, data_set* ds, double* total_err, double* avg_err) 
{
	if (n == NULL || ds == NULL || total_err == NULL || avg_err == NULL)
		return E_NULL_ARG;

	*total_err = 0;

	for (int i = 0; i < ds->test_count; i++) {
		matrix_t* input = NULL;
		matrix_t* expected_output = NULL;
		
		error_t e = cml_data_to_matrix(ds->data[i]->input, &input);
		if (e != E_SUCCESS) return e;

		e = cml_data_to_matrix(ds->data[i]->expected_output, &expected_output);
		if (e != E_SUCCESS) return e;

		e = feed_forward(n, input);
		if (e != E_SUCCESS) return e;
		
		*total_err += calculate_cost_func(n, expected_output);
		free_matrix(input);
		free_matrix(expected_output);
	}

	if (ds->test_count < 1)
		return E_FAILURE;

	*avg_err = *total_err / (double)ds->test_count;
	return E_SUCCESS;
}
