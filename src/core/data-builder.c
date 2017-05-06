#include <stdlib.h>
#include "cml.h"
#include "net-internal.h"
#include "matrix.h"
#include "csv-utils.h"

/* Static funcs */
static error_t extract_feature_names_csv (data_set* ds, FILE* fh);
static error_t add_feature_name (data_set* ds, char* name);

/* init_cml_data() */
cml_data* init_cml_data () {
	cml_data* new_data = calloc(1, sizeof(cml_data));
	return new_data;
}


/* add_to_cml_data() */
error_t add_to_cml_data (cml_data* data, double value) {
	if (data == NULL) return E_NULL_ARG;

	unsigned int oldnum = ++data->count;
	data->items = realloc(data->items, sizeof(double) * data->count);
	data->items[oldnum] = value;
	return E_SUCCESS;
}


/* copy_cml_data() */
error_t copy_cml_data (cml_data* src, cml_data** dst) {
	
	if (src == NULL || dst == NULL)
		return E_NULL_ARG;

	if (*dst == NULL)
		*dst = init_cml_data();

	(*dst)->count = src->count;
	(*dst)->items = calloc(src->count, sizeof(double));

	for (int i = 0; i < src->count; i++)
		(*dst)->items[i] = src->items[i];

	return E_SUCCESS;
}


/* cml_data_iter() */
error_t cml_data_iter (cml_data* data, double* value) {
	if (data == NULL || value == NULL)
		return E_NULL_ARG;

	/* Reset the position and return that there are no more items */
	if (data->pos + 1 >= data->count) {
		data->pos = 0;
		return E_NO_MORE_ITEMS;
	}

	*value = data->items[data->pos];
	data->pos++;
	return E_SUCCESS;
}


/* cml_data_to_matrix() */
error_t cml_data_to_matrix (cml_data* data, matrix_t** m) {
	if (data == NULL || m == NULL)
		return E_NULL_ARG;

	if (*m == NULL)
		*m = malloc(sizeof(matrix_t));

	error_t err = init_matrix(*m, data->count, 1);
	if (err != E_SUCCESS) return err;

	for (int i = 0; i < data->count; i++) 
		(*m)->matrix[i][0] = data->items[i];

	return E_SUCCESS;
}


/* matrix_to_cml_data() */
error_t matrix_to_cml_data (matrix_t* m, cml_data** data) {
	if (m == NULL || data == NULL)
		return E_NULL_ARG;

	if (*data == NULL)
		*data = init_cml_data();

	if (*data == NULL)
		return E_FAILURE;

	for (int i = 0; i < m->rows; i++) 
		(*data)->items[i] = m->matrix[i][0];

	return E_SUCCESS;
}

/* free_cml_data() */
error_t free_cml_data (cml_data*  data) {
	if (data)
		free(data->items);
	free(data);
	return E_SUCCESS;
}


/* init_data_pair() */
data_pair* init_data_pair (cml_data* input, cml_data* output) {
	if (input == NULL || output == NULL)
		return E_NULL_ARG;

	error_t err;
	data_pair = calloc(1, sizeof(data_pair));

	err = copy_cml_data(input, &data_pair->input);
	if (err != E_SUCCESS) goto exit;

	err = copy_cml_data(output, &data_pair->output);
	if (err != E_SUCCESS) goto exit;

	return data_pair;

	/* On failure free all memory allocated */
exit:
	free(data_pair);
	return NULL;
}

/* free_data_pair() */
error_t free_data_pair (data_pair* pair) {
	if (pair == NULL) return E_NULL_ARG;

	free_data_pair(pair->input);
	free_data_pair(pair->output);
	free(pair);
	return E_SUCCESS;
}

/* add_data_pair() */
error_t add_data_pair (data_set* set, data_pair* pair) {
	
	if (set == NULL || pair == NULL)
		return E_NULL_ARG;

	/* Shallow copy here should be ok, as user's never directly deal with
	 * the data_pair unless they implement their own functions to create a data_set */
	set->data = realloc(set->data, sizeof(data_pair*) * ++set->count);
	set->data[set->count - 1] = pair;

	return E_SUCCESS;
}


/* init_data_set() */
data_set* init_data_set () {
	data_set* ds = calloc(1, sizeof(data_set));
	return ds;
}

/* free_data_set() */
error_t free_data_set(data_set* ds) {
	if (ds == NULL) return E_NULL_ARG;

	for (int i = 0; i < ds->count; i++) {
		error_t err = free_data_pair(ds->data[i]);
		if (err != E_SUCCESS) return err;
	}
	free(ds);
}


/* data_set_from_csv() */
error_t data_set_from_csv (data_set* ds, FILE* fh) {

}

static error_t add_feature_name (data_set* ds, char* name);
