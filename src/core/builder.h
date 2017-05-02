#ifndef _MATRIX_BUILDER_H_
#define _MATRIX_BUILDER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"
#include "cml.h"
#include "net-internal.h"
#include "err.h"

#define MAXLINELEN 256

/*	Below are the functions used to build a neural net (struct net) that is customized
 *	to meet the needs. Each layer must be built seperately and added into the struct net.
 *
 */

/* Not quite sure where the functions below stand. I want to move most of the 
 * data set stuff into python.
 */

/* 
 *
 */
data_set* data_set_from_csv (FILE* fh); 

/*
 *
 */
int free_data_set(data_set* data);

/*
 *
 */
int load_net (net* n, FILE* fh);


/*
 *
 */
error_t save_net (net* n, FILE* fh);


#endif
