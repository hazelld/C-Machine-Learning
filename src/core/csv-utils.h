#ifndef _CSV_UTILS_H_
#define _CSV_UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "cml.h"
#include "cml-internal.h"
#include "data-builder.h"

/* Allow for user to override if they so choose */
#ifndef CSV_DELIM
#	define CSV_DELIM ","
#endif

#ifndef CSVMAXLINELEN
#	define CSVMAXLINELEN 256
#endif

#define TRUE 0
#define FALSE 1

enum InputType get_feature_type (char* feature);
error_t check_input_type (enum InputType type, char* feature);
error_t parse_csv_row (FILE* fh, char*** dst, int* items);
enum InputType get_type (char* feature);
error_t validate_csv_row (data_set* ds, char** row, int row_size);


#endif
