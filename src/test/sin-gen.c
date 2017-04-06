#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>


int main() {
	
	srand(time(NULL));
	// Random numbers from [-10, 10]
	double interval = 2;
	double div = RAND_MAX / (interval * 2);
	
	for (int i = 0; i < 150; i++) {
		
		double rnum = -interval + ((double)rand() / div);
		double sin_val = sin(rnum);
		double transformed = 0.5 * (sin_val + 1);

		printf("%lf;%lf\n", rnum, transformed);
	}
}
