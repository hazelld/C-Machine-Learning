#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include <string.h>

#include "net.h"
#include "builder.h"


/* API for net.h */
static PyObject* CNN_init_net(PyObject* self, PyObject* args);
static PyObject* CNN_train(PyObject* self, PyObject* args);
static PyObject* CNN_predict(PyObject* self, PyObject* args);
static PyObject* CNN_free_net(PyObject* self, PyObject* args);


/* API for builder.h */
static PyObject* CNN_data_from_csv(PyObject* self, PyObject* args);
static PyObject* CNN_free_data(PyObject* self, PyObject* args);


/* Set up the python stuff */
static PyMethodDef CNNMethods[] = {
	{"init_net", CNN_init_net, METH_VARARGS},
	{"train", CNN_train, METH_VARARGS},
	{"predict", CNN_predict, METH_VARAGS},
	{"free_net", CNN_free_net, METH_VARARGS},
	{"data_from_csv", CNN_data_from_csv, METH_VARARGS},
	{"free_data", CNN_free_data, METH_VARAGRS},
	{NULL, NULL}
};

static struct PyModuleDef cnnModuleDef = {
	PyModuleDef_HEAD_INIT,
	"CNN",
	NULL,
	-1,
	CNNMethods,
};

PyMODINIT_FUNC PyInit_CNN (void) { return PyModule_Create(&cnnModuleDef); }


/* CNN_init_net
 *	
 *	This function is used to initialize a net datastructure within the CNN 
 *	library. 
 *
 * Arguments:
 *	- Python list that holds the topology of the net
 *	- Name of the activation function, currently supported functions:
 *		-> sigmoid	
 *	- Learning Rate
 *
 * Returns:
 * 	- Error Code
 *
 */
PyObject* CNN_init_net (PyObject* self, PyObject* args) {
	int* topology;
	PyObject* topology_list;
	net* nn;
	char* function_name;
	double learning_rate;

	PyArg_ParseTuple(args, "Osd",  &topology_list, function_name, learning_rate);
	nn = malloc(sizeof(net));

	/* Check function name here, set to proper value */
	
	/* Convert topology list to an int array */
	if (!PyList_Check(topology_list)) {
		// Return Error
	} else {
		len = PyList_Size(topology_list);
	}

	topology = malloc(sizeof(int) * len);
	for (int i = 0; i < len; i++) {
		PyObject* item = PyList_GetItem(topology_list, i);
		
		/* Check that it is the right type */
		if (!PyInt_Check(item)) {
			fprintf(stderr, "Item sent to init_net isn't an int");
			exit(EXIT_FAILURE);
			// Throw error
		}
		/* I have to assume you will never use layers with a size larger than
		 * what an int can hold. */
		topology[i] = (int)PyInt_AsLong(item);
	}

	/* Initialize the net */
	int err;
	err = init_net(nn, len, topology, NULL, NULL, learning_rate);  
	/* Handle the error */

	free(topology);
	
	/* Return the reference to the pointer */
}




