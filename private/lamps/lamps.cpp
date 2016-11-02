#include <stdio.h>
#include <malloc.h>


#include "matrix.h"

int main ()
{
	Matrix* matrix;
#if 0
	char* filename = "/home/mate/docs/programming/Peti/puzzle_of_lamps/input_test/t_07.txt";
	matrix = Matrix::getDataFromFile(filename);
#else
	matrix = Matrix::getDataFromStdin();
#endif

	if (matrix)
	{
		matrix->print();
		if (matrix->checkLights())
		{
			fprintf(stderr, "lights: OK\n");
		} else {
			fprintf(stderr, "lights: NOT OK\n");
		}
		if (matrix->isAllLit())
		{
			fprintf(stderr, "lit   : OK\n");
		} else {
			fprintf(stderr, "lit   : NOT OK\n");
		}
		delete matrix;
	}

	return 0;
}



