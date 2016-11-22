#include "builder.h"

static int parse_net (net* n, FILE* fh);
static int parse_layer (layer** l, FILE* fh);
static int parse_matrix (matrix_t** m, FILE* fh);

static int chomp (char* str) {
	while (*str != '\n') {
		if (*str == '\0')
			return SUCCESS;
		*str++;
	}	
	*str = '\0';
	return SUCCESS;
}

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


int load_net (net* n, FILE* fh) {
	return parse_net(n, fh);	
}

static int parse_net (net* n, FILE* fh) {
	if (n == NULL || fh == NULL) return FAILURE;
	char* line_buff = malloc(sizeof(char) * MAXLINELEN);
	char *key, *val;
	
	/* Ensure the file begins with BEGIN NET */	
	if (fgets(line_buff, MAXLINELEN, fh) == NULL) return FAILURE;
	chomp(line_buff);
	if (strcmp(line_buff, "BEGIN NET") != 0) return FAILURE;

	/* Get the layer count */
	if (fgets(line_buff, MAXLINELEN, fh) == NULL) return FAILURE;
	chomp(line_buff);
	key = strtok(line_buff, "=");
	val = strtok(NULL, "=");
	sscanf(val, "%d", &(n->layer_count));

	/* Get the topology */
	int* topology = malloc(sizeof(int) * n->layer_count);
	int i = 0;
	if (fgets(line_buff, MAXLINELEN, fh) == NULL) return FAILURE;
	chomp(line_buff);

	key = strtok(line_buff, ",");
	while (key != NULL) {
		sscanf(key, "%d", &(topology[i]));
		key = strtok(NULL, ",");
		i++;
	}
	
	init_net(n, n->layer_count, topology, NULL, NULL, 0.2);

	int done = 0;
	int cur_layer = 0;

	while (!done) {
		if (fgets(line_buff, MAXLINELEN, fh) == NULL) return FAILURE;
		chomp(line_buff);

		if (strcmp(line_buff, "BEGIN LAYER") == 0) {
			
			if (cur_layer >= n->layer_count) {
				fprintf(stderr, "ERROR: Too many layers defined\n");
				return FAILURE;
			}

			if (parse_layer(&(n->layers[cur_layer]), fh) == FAILURE)
				return FAILURE;

			cur_layer++;
			continue;
		}
		
		if (strcmp(line_buff, "END NET") == 0) 
			break;

		fprintf(stderr, "Unexpected input: %s\nExpected BEGIN LAYER or END NET\n", line_buff);
	}

	free(line_buff);
	free(topology);
	return SUCCESS;
}

static int parse_layer (layer** l, FILE* fh) {
	if (l == NULL || fh == NULL) return FAILURE;
	*l = malloc(sizeof(layer));
	int done = 0;

	while (!done) {
		char* line_buff = malloc(sizeof(char) * MAXLINELEN);
		char *key, *val;
		
		if (fgets(line_buff, MAXLINELEN, fh) == NULL) return FAILURE;
		chomp(line_buff);

		if (strcmp(line_buff, "BEGIN MATRIX") == 0) {
			(*l)->weights = malloc(sizeof(matrix_t));
			int ret = parse_matrix(&((*l)->weights), fh);
			if (ret == FAILURE) return FAILURE;
			continue;
		}

		if (strcmp(line_buff, "END LAYER") == 0)
			break;

		key = strtok(line_buff, "=");
		val = strtok(NULL, "=");

		if ( strcmp(key, "type") == 0 ) {
			if (strcmp(val, "input") == 0) (*l)->ltype = input;
			if (strcmp(val, "hidden") == 0) (*l)->ltype = hidden;
			if (strcmp(val, "output") == 0) (*l)->ltype = output;
		} 
		else if ( strcmp(key, "input-nodes") == 0) 
			sscanf(val, "%d", &((*l)->input_nodes));
			//(*l)->input_nodes = strtol(val, NULL, 10);		
		else if ( strcmp(key, "output-nodes") == 0) 
			sscanf(val, "%d", &((*l)->output_nodes));
			//(*l)->output_nodes = strtol(val, NULL, 10);
		else if ( strcmp(key, "bias-nodes") == 0) 
			(*l)->bias = strtod(val, NULL);
		
		free(line_buff);
	}
	return SUCCESS;
}

static int parse_matrix (matrix_t** m, FILE* fh) {
	if (m == NULL || fh == NULL) return FAILURE;

	/* We can't use init_matrix until we know the dimensions, so for now
	 * just keep in a 1d buffer and track the line width */
	double* buff = malloc(sizeof(double));

	*m = malloc(sizeof(matrix_t));
	int done = 0;

	/* Each Line corresponds to the i value, the 
	 * j corresponds to each csv value */
	int j, rows, columns;
	j = rows = 0;
	columns = 1;

	while (!done) {
		rows++;
		char* line_buff = malloc(sizeof(char) * MAXLINELEN);
		char* token;

		if (fgets(line_buff, MAXLINELEN, fh) == NULL) return FAILURE;
		chomp(line_buff);

		if ( strcmp(line_buff, "END MATRIX") == 0 ) {
			done = 1;
		} else {
			token = strtok(line_buff, ",");
			while (token != NULL) {
				j++;
				buff = realloc(buff, sizeof(double) * rows * j);
				buff[(4 * rows-1) + j-1] = strtod(token, NULL);
				token = strtok(NULL, ",");
			}
			
			if (columns == 1 && j > 0) columns = j;
			else if (columns != j) return FAILURE;

		}
		free(line_buff);
	}

	init_matrix(*m, rows, columns);

	for (int i = 0; i < rows; i++) {
		for (int n = 0; n < columns; n++) {
			(*m)->matrix[i][n] = buff[(4 * i) + n];
		}
	}
	free(buff);
	return SUCCESS;
}

int save_net (net* n, FILE* fh) {
	if (n == NULL || fh == NULL) return FAILURE;

	fprintf(fh, "BEGIN NET\n");
	fprintf(fh, "layers=%d\n", n->layer_count);

	for (int i = 0; i < n->layer_count; i++) {
		fprintf(fh, "%d,", n->topology[i]);
	}
	fprintf(fh, "\n");

	for (int i = 0; i < n->layer_count; i++) {
		layer* clayer = n->layers[i];
		fprintf(fh, "BEGIN LAYER\n");
		fprintf(fh, "type=");
		
		if (clayer->ltype == input) fprintf(fh, "input\n");
		if (clayer->ltype == hidden) fprintf(fh, "hidden\n");
		if (clayer->ltype == output) fprintf(fh, "output\n");
		
		fprintf(fh, "input-nodes=%d\n", clayer->input_nodes);
		fprintf(fh, "output-nodes=%d\n", clayer->output_nodes);
		fprintf(fh, "bias=%lf\n", clayer->bias);
		
		fprintf(fh, "BEGIN MATRIX\n");
		
		if (clayer->weights != NULL) {
			matrix_t* buff = clayer->weights;

			for (int n = 0; n < buff->rows; n++) {
				for (int j = 0; j < buff->columns; j++) {
					fprintf(fh, "%lf,", buff->matrix[n][j]);	
				}
				fprintf(fh, "\n");
			}
		}
		fprintf(fh, "END MATRIX\nEND LAYER\n");
	}
	fprintf(fh, "END NET");
	return SUCCESS;
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
