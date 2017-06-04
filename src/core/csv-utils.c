#include "csv-utils.h"

static int is_double (char* feature) 
{
	if (feature == NULL) return -1;
	char c;

	while ((c = *feature) != '\0') {
		if (isdigit(c) || c == '.' || c == '-')
			feature++;
		else 
			return FALSE;
	}
	return TRUE;
}

enum InputType get_type (char* feature) 
{	
	if (is_double(feature) == TRUE)
		return T_DOUBLE;
	else 
		return T_STR;
}


error_t validate_csv_row (data_set* ds, char** row, int row_size) 
{
	if (ds == NULL || row == NULL) return E_NULL_ARG;

	if (row_size != ds->feature_count)
		return E_CSV_INVALID_LINE_LENGTH;

	for (int i = 0; i < ds->feature_count; i++) {

		enum InputType new_type = get_type(row[i]);

		if (ds->feature_types[i] != new_type)
			return E_CSV_INVALID_COLUMN_VALUE;
	}
	return E_SUCCESS;
}

error_t parse_csv_row (FILE* fh, char*** dst, int* items) 
{
	if (dst == NULL || fh == NULL) return E_NULL_ARG;

	char* token;
	char* line = malloc(sizeof(char) * CSVMAXLINELEN);
	int count = 0;

	if (*dst == NULL) 
		*dst = calloc(1, sizeof(char*));

	if (!fgets(line, CSVMAXLINELEN, fh)) {
		free(*dst);
		free(line);

		if (feof(fh))
			return E_NO_MORE_ITEMS;
		else
			return E_CSV_PARSE_ERR;
	}

	/* Remove newline */
	char c = line[strlen(line) - 1];
	if (c == '\n')
		line[strlen(line) - 1] = '\0';

	int counta = 0;
	token = strtok(line, CSV_DELIM);
	while (token != NULL && counta++ < 5) {
		*dst = realloc(*dst, sizeof(char*) * ++count);
		(*dst)[count-1] = malloc(sizeof(char) * CSVMAXLINELEN);
		strcpy((*dst)[count-1], token); 	
		token = strtok(NULL, CSV_DELIM);
	}
	
	free(line);
	*items = count;
	return E_SUCCESS;
}
