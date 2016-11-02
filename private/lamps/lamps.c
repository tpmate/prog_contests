#include <stdio.h>
#include <malloc.h>


#include "matrix.h"

int main ()
{
	Element* matrix;
	size_t width, height;
#if 0
	char* filename = "/home/mate/docs/programming/Peti/puzzle_of_lamps/input_test/t_07.txt";
	getDataFromFile(filename, &matrix, &width, &height);
#else
	getDataFromStdin(&matrix, &width, &height);
#endif

	if (matrix)
	{
		printMatrix(matrix, width, height);
		free(matrix);
	}

	return 0;
}



