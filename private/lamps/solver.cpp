#include <vector>

#include "solver.h"

using namespace std;

bool Solver::solve(Matrix& matrix, int index)
{
	if (index >= matrix.height*matrix.width)
	{
		/*
		fprintf(stderr, "Checking validity of matrix:\n");
		matrix.print();
		//*/
		return matrix.checkLights() && matrix.isAllLit();
	}

	Element& e = matrix.elements[index];
	if (e.type == ELEMENT_EMPTY)
	{
		e.type = ELEMENT_LIGHT;
		if (solve(matrix, index+1))
		{
			return true;
		}
		e.type = ELEMENT_EMPTY;
	}
	if (solve(matrix, index+1))
	{
		return true;
	}
	return false;
}

bool Solver::solve(Matrix& matrix)
{
	return solve(matrix, 0);
}
