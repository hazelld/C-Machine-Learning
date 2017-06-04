#include <stdlib.h>
#include <stdio.h>
#include "cml.h"
#include "cml-internal.h"

/* Since C garuntees that enums start at specified digit (this case 0), and 
 * count upwards, we can just have each element in struct array correspond with 
 * the proper error code. This is done instead of just a char* array, so that I can
 * remember the corresponding enum value without having to constantly check cml.h.
 */
struct _errorstr {
	error_t err;
	char* message;
} errorstr[] = {
	{ E_SUCCESS, "No error" },
	{ E_FAILURE, "Generic Failure" },
	{ E_NULL_ARG, "Null argument passed to function" },
	{ E_ALLOC_FAILURE, "Failed to allocate memory" },
	{ E_MATRIX_WRONG_DIM, "Matrix is wrong size for operation" },
	{ E_ZERO_DIM_MATRIX, "Matrix has no dimensions. Try init_matrix()" },
	{ E_NOT_VECTOR, "Expected vector and got matrix" },
	{ E_NO_INPUT_LAYER, "No input layer has been provided" },
	{ E_NO_OUTPUT_LAYER, "No output layer has been provided" },
	{ E_TOO_MANY_INPUT_LAYERS, "Provided too many input layers, max of 1 allowed" },
	{ E_TOO_MANY_OUTPUT_LAYERS,"Provided too many output layers, max of 1 allowed"},
	{ E_INVALID_NODE_AMOUNT, "Invalid node amount given, must be positive" },
	{ E_LAYER_ALREADY_IN_NET, "Layer already exists in net" },
	{ E_NET_NOT_CONNECTED, "Net has not been connected with connect_net()" },
	{ E_WRONG_INPUT_SIZE, "Input data does not match input node amount" },
	{ E_WRONG_OUTPUT_SIZE, "Output data does not match output node amount" },
	{ E_NO_CALLBACK_GIVEN, "No callback provided, check function docs" },
};

void print_cml_error (FILE* fh, char* message, error_t err) 
{
	if ( fh == NULL ) return;

	if ( message != NULL ) {
		fprintf(fh, "%s", message);
		fprintf(fh, ": ");
	}
	fprintf(fh, "%s\n", errorstr[err].message);
}
