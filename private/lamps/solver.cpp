#include <vector>

#include "solver.h"

using namespace std;

bool Solver::solve(Matrix& matrix, int index)
{
	if (index >= matrix.height*matrix.width)
	{
		return matrix.checkLights() && matrix.isAllLit();
	}

	Element& e = matrix.elements[index];
	int row = index/matrix.width;
	int column = index%matrix.width;
	if (e.type & ANY_WALL)
	{
		vector<Element*> neighbours;
		{
			int i = INDEX_OF(column-1, row, matrix.width);
			if (column>0 && matrix.elements[i].type==ELEMENT_EMPTY) neighbours.push_back(&matrix.elements[i]);
		}
		{
			int i = INDEX_OF(column+1, row, matrix.width);
			if (column<matrix.width-1 && matrix.elements[i].type==ELEMENT_EMPTY) neighbours.push_back(&matrix.elements[i]);
		}
		{
			int i = INDEX_OF(column, row-1, matrix.width);
			if (row>0 && matrix.elements[i].type==ELEMENT_EMPTY) neighbours.push_back(&matrix.elements[i]);
		}
		{
			int i = INDEX_OF(column, row+1, matrix.width);
			if (row<matrix.height-1 && matrix.elements[i].type==ELEMENT_EMPTY) neighbours.push_back(&matrix.elements[i]);
		}

		int patternEnd = 1 << neighbours.size();

		for (int pattern = 0; pattern < patternEnd; ++pattern)
		{
			for (int i = 0; i < (int)neighbours.size(); ++i)
			{
				if (pattern & (1<<i))
				{
					neighbours[i]->type=ELEMENT_LIGHT;
				}
				else
				{
					neighbours[i]->type=ELEMENT_EMPTY;
				}

				if (solve(matrix, index+1))
				{
					return true;
				}
			}
		}
		for (int i = 0; i < (int)neighbours.size(); ++i)
		{
			neighbours[i]->type=ELEMENT_EMPTY;
		}
	}
	return false;
}

bool Solver::solve(Matrix& matrix)
{
	return solve(matrix, 0);
}
