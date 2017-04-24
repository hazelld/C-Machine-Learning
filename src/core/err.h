#ifndef _CNN_ERR_H_
#define _CNN_ERR_H_

/*	This header defines some common errors that may occur throughout the cnn library.
 *	It should be included into all source files.
 *
 */

enum _cnn_error {

	/* Generic Success/Failure. The failure is only used in weird conditions when the 
	 * other error codes do not suffice. 
	 */
	E_SUCCESS,
	E_FAILURE,

	/* Should never have NULL arguments */
	E_NULL_ARG,

	/*	These are the error codes that may occur within the matrix module. The handling
	 *	of the errors is done by the matrix module, as generally they should not occur. If 
	 *	an error does occur in this module, there is no forseeable way to continue, so just 
	 *	print the error message.
	 * */
	E_MATRIX_WRONG_DIM,
	E_ZERO_DIM_MATRIX,
	E_NOT_VECTOR,

	/*	These are the error codes that may occur in the builder module when creating 
	 *	a custom neural network.
	 */
	E_NO_INPUT_LAYER,
	E_NO_OUTPUT_LAYER,
	E_TOO_MANY_INPUT_LAYERS,
	E_TOO_MANY_OUTPUT_LAYERS,
	E_INVALID_NODE_AMOUNT,
	E_INVALID_LAYER_TYPE,
	E_LAYER_ALREADY_IN_NET,
	E_NET_NOT_CONNECTED, 

	/* Error codes for issues between data and neural net */
	E_WRONG_INPUT_SIZE,
	E_WRONG_OUTPUT_SIZE,

	/* Activation function issues */
	E_NO_CALLBACK_GIVEN,

};

typedef enum _cnn_error error_t;


#endif
