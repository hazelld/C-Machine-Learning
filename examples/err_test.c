/* This file is a simple test/example of how to use the 
 * print_cml_error() function. 
 */

#include <stdio.h>
#include <stdlib.h>
#include "cml.h"


int main() {
	error_t err = E_SUCCESS;
	int count = E_NO_CALLBACK_GIVEN - E_SUCCESS;

	/* Test with a prepended message, and without on both streams */
	for (int i = err; i < count; i++) {
		print_cml_error (stderr, "ERROR", (error_t)i);
		print_cml_error (stdout, NULL, (error_t)i);
	}
}
