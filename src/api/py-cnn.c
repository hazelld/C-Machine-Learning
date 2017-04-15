#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include <string.h>

#include "net.h"
#include "builder.h"


/* API for net.h */
static PyObject* CNN_init_net(PyObject* self, PyObject* args);
static PyObject* CNN_test_predict(PyObject* self, PyObject* args);
static PyObject* CNN_test_train(PyObject* self, PyObject* args);
/*
static PyObject* CNN_train(PyObject* self, PyObject* args);
static PyObject* CNN_predict(PyObject* self, PyObject* args);
static PyObject* CNN_free_net(PyObject* self, PyObject* args);
*/

/* API for builder.h */
/*static PyObject* CNN_data_from_csv(PyObject* self, PyObject* args);
static PyObject* CNN_free_data(PyObject* self, PyObject* args);
*/

/* Set up the python stuff */
static PyMethodDef CNNMethods[] = {
	{"init_net", CNN_init_net, METH_VARARGS},
/*	{"train", CNN_train, METH_VARARGS},
	{"predict", CNN_predict, METH_VARARGS},
	{"free_net", CNN_free_net, METH_VARARGS},
	{"data_from_csv", CNN_data_from_csv, METH_VARARGS},
	{"free_data", CNN_free_data, METH_VARARGS},
*/	{"test_train", CNN_test_train, METH_VARARGS},
	{"test_predict", CNN_test_predict, METH_VARARGS},
	{NULL, NULL}
};

static struct PyModuleDef cnnModuleDef = {
	PyModuleDef_HEAD_INIT,
	"cnn",
	NULL,
	-1,
	CNNMethods,
};

/* PyInit_cnn
 *
 * 	This function is called when the Python module is loaded. We 
 * 	want to set up a pointer to the neural network here, so it 
 * 	can be accessed through any other API function.
 *
 */
PyMODINIT_FUNC PyInit_cnn (void) { 
	
	PyObject* module = PyModule_Create(&cnnModuleDef);
	if (module == NULL)
		return NULL;
	
	net* nn = malloc(sizeof(net));
	PyObject* neural_net = PyCapsule_New((void*)nn, "cnn._neuralnet_C_API", NULL);
	PyModule_AddObject(module, "_neuralnet_C_API", neural_net);
	return module;
}


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
	double learning_rate;

	/* Get the arguments from the PyObject */
	PyArg_ParseTuple(args, "Od", &topology_list, &learning_rate);
	
	/* Get the net's pointer */
	net* nn = (net*)PyCapsule_Import("cnn._neuralnet_C_API", 0);

	/*	Extract the topology list */
	PyObject* seq = PySequence_Fast(topology_list, "Expected a Sequence");
	int len = PySequence_Size(topology_list);
	topology = malloc(sizeof(int) * len);
	
	for (int i = 0; i < len; i++) {
		PyObject* item = PySequence_Fast_GET_ITEM(seq, i);
		
		/* Ensure we have an integer */
		int val;
		if (PyLong_Check(item)) {
			val = (int) PyLong_AsSize_t(item);
		} else {
			// TODO: throw exception 
		}
		topology[i] = val;
	}

	/* Initialize the object and return 1 */
	init_net(nn, len, topology, learning_rate);
	free(topology);
	return Py_BuildValue("i", 0);
}


/*	Very basic function to test functionality. In future this needs
 * 	whole overhaul to how the data is loaded. 
 */
PyObject* CNN_test_train (PyObject* self, PyObject* args) {
	char* fname;
	net* nn = (net*)PyCapsule_Import("cnn._neuralnet_C_API", 0);

	/* Get filename as argument */
	PyArg_ParseTuple(args, "s", &fname);

	/* Try to open file */
	FILE* fh = fopen(fname, "r");

	if (fh == NULL) 
		return Py_BuildValue("i", 1);

	data_set* data = data_set_from_csv(fh);
	fclose(fh);

	// Train the network
	train(nn, data, 1);
	free_data_set(data);

	return Py_BuildValue("i", 0);
}

/*
 *
 */
PyObject* CNN_test_predict (PyObject* self, PyObject* args) {
	double input;
	PyArg_ParseTuple(args, "d", &input);
	
	net* nn = (net*)PyCapsule_Import("cnn._neuralnet_C_API", 0);

	matrix_t* in = malloc(sizeof(matrix_t));
	init_matrix(in, 1, 1);
	in->matrix[0][0] = input;
	matrix_t* res = predict(nn, in);

	return Py_BuildValue("d", res->matrix[0][0]);
}
