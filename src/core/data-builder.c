#include <stdlib.h>
#include "cml.h"
#include "net-internal.h"
#include "matrix.h"
#include "csv-utils.h"

/* Static funcs */
static error_t add_feature_name (data_set* ds, char* name);
static error_t str_to_cml_data(data_set* ds, cml_data* data, char** str, int count);
static error_t set_feature_types(data_set* ds, char** features, int size);


/* init_cml_data() */
cml_data* init_cml_data () {
	cml_data* new_data = calloc(1, sizeof(cml_data));
	return new_data;
}


/* add_to_cml_data() */
error_t add_to_cml_data (cml_data* data, void* value) {
	if (data == NULL || value == NULL) return E_NULL_ARG;

	unsigned int oldnum = data->count++;
	data->items = realloc(data->items, sizeof(void*) * data->count);
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



/* cml_data_to_matrix() */
/* error_t cml_data_to_matrix (cml_data* data, matrix_t** m) {
	if (data == NULL || m == NULL)
		return E_NULL_ARG;

	if (*m == NULL)
		*m = malloc(sizeof(matrix_t));

	error_t err = init_matrix(*m, data->count, 1);
	if (err != E_SUCCESS) return err;

	for (int i = 0; i < data->count; i++) 
		(*m)->matrix[i][0] = data->items[i];

	return E_SUCCESS;
} */


/* matrix_to_cml_data() */
/* error_t matrix_to_cml_data (matrix_t* m, cml_data** data) {
	if (m == NULL || data == NULL)
		return E_NULL_ARG;

	if (*data == NULL)
		*data = init_cml_data();

	if (*data == NULL)
		return E_FAILURE;

	for (int i = 0; i < m->rows; i++) 
		(*data)->items[i] = m->matrix[i][0];

	return E_SUCCESS;
} */

/* free_cml_data() */
error_t free_cml_data (cml_data*  data) {
	if (data == NULL) return E_NULL_ARG;

	for (int i = 0; i < data->count; i++)
		free(data->items[i]);
	free(data->items);
	free(data);
	return E_SUCCESS;
}


/* init_data_pair() */
data_pair* init_data_pair (cml_data* input, cml_data* output) {
	if (input == NULL || output == NULL)
		return NULL;

	error_t err;
	data_pair* pair = calloc(1, sizeof(data_pair));
	pair->input = input;
	pair->expected_output = output;
	return pair;
}

/* free_data_pair() */
error_t free_data_pair (data_pair* pair) {
	if (pair == NULL) return E_NULL_ARG;

	free_cml_data(pair->input);
	free_cml_data(pair->expected_output);
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

/* add_cml_data() */
error_t add_cml_data (data_set* ds, cml_data* data) {
	if (ds == NULL || data == NULL) return E_NULL_ARG;

	ds->raw_data = realloc(ds->raw_data, sizeof(cml_data*) * ++ds->raw_count);
	ds->raw_data[ds->raw_count - 1] = data;
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
	error_t err;

	for (int i = 0; i < ds->feature_count; i++) 
		free(ds->feature_names[i]);
	free(ds->feature_names);
	free(ds->feature_types);

	for (int i = 0; i < ds->raw_count; i++) {
		free_cml_data(ds->raw_data[i]);
		if (err != E_SUCCESS) return err;
	}
	free(ds->raw_data);

	for (int i = 0; i < ds->count; i++) {
		err = free_data_pair(ds->data[i]);
		if (err != E_SUCCESS) return err;
	}
	free(ds);
}


/* data_set_from_csv() */
error_t data_set_from_csv (data_set* ds, FILE* fh, int* lineno) {
	error_t err;
	int line = 1;
	int count;

	/* First row of CSV file must be names */
	char** features = NULL;
	err = parse_csv_row(fh, &features, &count);
	if (err != E_SUCCESS) goto error;
	
	for (int i = 0; i < count; i++) {
		err = add_feature_name(ds, features[i]);
		if (err != E_SUCCESS) goto error;
		free(features[i]);
	}
	free(features);

	/* Now loop over entire file until the end */
	char** strline = NULL;
	while ((err = parse_csv_row(fh, &strline, &count)) == E_SUCCESS) {
		line++;

		if (line == 2) 
			err = set_feature_types(ds, strline, count);
		else 
			err = validate_csv_row(ds, strline, count);
		
		if (err != E_SUCCESS) goto error;

		cml_data* new_data = init_cml_data();
		err = str_to_cml_data(ds, new_data, strline, count);
		if (err != E_SUCCESS) goto error;
		
		err = add_cml_data (ds, new_data);
		if (err != E_SUCCESS) goto error;

		/* Since we used strcpy, we can free resources up */
		for (int i = 0; i < count; i++)
			free(strline[i]);
		free(strline);
		strline = NULL;
	}

error:
	*lineno = line;
	/* If the error code was EOF, want to return E_SUCCESS */
	if (err == E_NO_MORE_ITEMS)
		return E_SUCCESS;
	return err;
}


/* str_to_cml_data */
static error_t str_to_cml_data(data_set* ds, cml_data* data, char** str, int count) {
	error_t err;

	if (ds->feature_count != count)
		return E_CSV_INVALID_LINE_LENGTH;
	
	for (int i = 0; i < ds->feature_count; i++) {
		switch (ds->feature_types[i]) {
			
			case T_DOUBLE: ;
				double* new_dub = malloc(sizeof(double));
				*new_dub = strtod(str[i], (char**)NULL);
				err = add_to_cml_data(data, (void*)new_dub);
				break;

			case T_STR: ;
				char* new_str = malloc(sizeof(char) * (strlen(str[i]) + 1));
				strcpy(new_str, str[i]);
				err = add_to_cml_data(data, (void*)new_str);
				break;
		}
	}
	return E_SUCCESS;
}

/* add_feature_name */
static error_t add_feature_name (data_set* ds, char* name) {
	if (ds == NULL || name == NULL) return E_NULL_ARG;

	ds->feature_names = realloc(ds->feature_names, sizeof(char*) * ++ds->feature_count);
	ds->feature_names[ds->feature_count - 1] = malloc(sizeof(char) * CSVMAXLINELEN);
	strcpy(ds->feature_names[ds->feature_count - 1], name);
	return E_SUCCESS;
}

/* set_feature_types */
static error_t set_feature_types (data_set* ds, char** features, int size) {
	int i = 0;
	
	ds->feature_types = malloc(sizeof(enum InputType) * ds->feature_count);
	
	for (int i = 0; i < size; i++) {

		/* Ensure we don't go out of ds->feature_type's bounds */
		if (i > ds->feature_count)
			return E_CSV_INVALID_LINE_LENGTH;

		enum InputType type = get_type(features[i]);
		ds->feature_types[i] = type;
	}

	return E_SUCCESS;
}
