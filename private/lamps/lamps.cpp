#include <cstdio>
#include <string>


#include "matrix.h"
#include "solver.h"
#include "generator.h"

using namespace std;

int main ()
{
	Matrix* matrix;
	Generator generator;
#if 0
	char* filename = "/home/mate/docs/programming/Peti/puzzle_of_lamps/input_test/t_07.txt";
	matrix = Matrix::getDataFromFile(filename);
#elif 0
	matrix = Matrix::getDataFromStdin();
#elif 1
	int n = 8;
	matrix = Matrix::getDataFromString(generator.getNextMatrix(n, n), n, n);
#else
	matrix = Matrix::getDataFromString(string("   \n#  \n   "), 3, 3);
#endif

	if (matrix)
	{
		fprintf(stderr, "The original matrix is:\n");
		matrix->print();
		fprintf(stderr, "\n");

		Solver solver;

		if (solver.solve(*matrix))
		{
			fprintf(stderr, "A solution is:\n");
			matrix->print();
			fprintf(stderr, "\n");
		} else {
			fprintf(stderr, "There is no solution with this matrix.\n");
		}

		delete matrix;
	} else {
		fprintf(stderr, "Failed to parse matrix\n");
	}

	return 0;
}



