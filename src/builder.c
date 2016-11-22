#include "builder.h"

data_set* data_set_from_csv(FILE* fh) {
	int lines, inputs_per_line, outputs_per_line;
	char *token, *inputs, *outputs;

	data_set* data = malloc(sizeof(data_set));
	data->data = malloc(sizeof(data_pair*));
	char* line_buffer = malloc(sizeof(char) * MAXLINELEN);
	lines = inputs_per_line = outputs_per_line = 0;

	const char delim[2] = ",";
	const char split[2] = ";";

	while( fgets(line_buffer, MAXLINELEN, fh) != NULL) {
		int item_count = 0;

		/* Allocate more space */
		data->count = lines++;
		data->data = realloc(data->data, sizeof(data_pair*) * lines);
		data->data[data->count] = malloc(sizeof(data_pair));
		data->data[data->count]->input = malloc(sizeof(matrix_t));
		data->data[data->count]->expected_output = malloc(sizeof(matrix_t));
		
		/* Get first and second half of line */
		double* value_buffer = malloc(sizeof(double));
		inputs = strtok(line_buffer, split);
		outputs = strtok(NULL, split);

		token = strtok(inputs, delim);
		while (token != NULL) {
			item_count++;
			value_buffer = realloc(value_buffer, sizeof(double) * item_count);
			value_buffer[item_count-1] = strtod(token, NULL);
			token = strtok(NULL, delim);
		}
		
		/* If this is first iteration, save the values */
		if (lines == 1) 
			inputs_per_line = item_count;

		if (inputs_per_line != item_count) {
			fprintf(stderr, "Inconsistent amount of inputs on line %d...Dying\n", lines);
			exit(1);
		}

		/* Put into data_pair* */
		init_matrix(data->data[data->count]->input, item_count, 1);
		for (int i = 0; i < item_count; i++)
			data->data[data->count]->input->matrix[i][0] = value_buffer[i];
		
		/* Reset for output */
		item_count = 0;
		double* vals = malloc(sizeof(double));
		
		token = strtok(outputs, delim);
		while (token != NULL) {
			item_count++;
			vals = realloc(vals, sizeof(double) * item_count);
			vals[item_count-1] = strtod(token, NULL);
			token = strtok(NULL, delim);
		}

		if (lines == 1)
			outputs_per_line = item_count;
		
		if (outputs_per_line != item_count) {
			fprintf(stderr, "Inconsistent amount of outputs on line %d...Dying\n", lines);
			exit(1);
		}

		init_matrix(data->data[data->count]->expected_output, item_count, 1);
		for (int i = 0; i < item_count; i++) 
			data->data[data->count]->expected_output->matrix[i][0] = vals[i];
		
		free(value_buffer);
		free(vals);
	}
	data->count++;
	free(line_buffer);	
	return data;
}


int free_data_set (data_set* data) {
	for (int i = 0; i < data->count; i++) {
		free_matrix(data->data[i]->input);
		free_matrix(data->data[i]->expected_output);
		free(data->data[i]);
	}
	free(data->data);
	free(data);
	return SUCCESS;
}
