#include "matrix-builder.h"

/* TODO: 
 * - Add support for other delims and line endings
 */
matrix_t** from_csv (FILE* f, int* result) {
	int lines = 0;
	int items_per_line = 0;
	matrix_t** input = malloc(sizeof(matrix_t*));
	char* linebuff = malloc(sizeof(char) * MAXLINELEN);
	char* token;
	double* values;

	while (fgets(linebuff, MAXLINELEN, f) != NULL) {
		int current_items = 0;
		values = malloc(sizeof(double));
		token = strtok(linebuff, ",");

		/* Get all values from line */
		while (token != NULL) {
			current_items++;
			values = realloc(values, sizeof(double) * current_items);
			values[current_items-1] = strtod(token, NULL);
		}

		if (items_per_line == 0 && current_items > 0) 
			items_per_line = current_items;

		/* Mismatched amount of items */
		if (current_items != items_per_line) 
			goto die;
	
		/* Re-alloc more space, and add new matrix in */
		lines++;
		input = realloc(input, sizeof(matrix_t*) * lines);
		input[lines-1] = malloc(sizeof(matrix_t));
		init_matrix(input[lines-1], items_per_line, 1);

		for (int i = 0; i < items_per_line; i++)
			input[lines-1]->matrix[i][0] = values[i];
		free(values);
	}


	/* If we have to abort the function early, free all memory allocated */
die:
	free(values);
	free(linebuff);
	
	for(int i = 0; i < lines; i++) 
		free_matrix(input[i]);
	free(input);

	*result = 1;
	return NULL;
}
