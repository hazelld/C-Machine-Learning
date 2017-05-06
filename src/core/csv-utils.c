#include "csv-utils.h"

static int is_int (char* feature) {
	if (feature == NULL) return -1;

	while ((char c = *feature) != '\0') {
		if (isdigit(c) || c == '-')
			feature++;
		else
			return FALSE;
	}
	return TRUE;
}


static int is_double (char* feature) {
	if (feature == NULL) return -1;

	while ((char c = *feature) != '\0') {
		if (isdigit(c) || c == '.' || c == '-')
			feature++;
		else 
			return FALSE;
	}
	return TRUE;
}


error_t parse_csv_row (FILE* fh, char*** dst) {
	if (dst == NULL || fh == NULL) return E_NULL_ARG;

	char* token;
	char* line = malloc(sizeof(char) * CSVMAXLINELEN);
	int count = 1;

	if (*dst == NULL) 
		*dst = calloc(1, sizeof(char*));

	if (!fgets(line, CSVMAXLINELEN, fh)) {
		if (feof(fh))
			return E_SUCCESS;
		else
			return E_CSV_PARSE_ERR;
	}

	token = strtok(line, CSV_DELIM);
	while (token != NULL) {
		*dst = realloc(*dst, sizeof(char*) * ++count);
		(*dst)[count-1] = malloc(sizeof(char) * CSVMAXLINELEN);
		strcpy((*dst)[count-1], token); 	
		token = strtok(line, CSV_DELIM);
	}

	/* Since we aren't passing back the size, set the last entry to NULL.
	 * Note we started count at 1, so there will always be enough room to do this */
	(*dst)[count-1] = NULL;
	return E_SUCCESS;
}
