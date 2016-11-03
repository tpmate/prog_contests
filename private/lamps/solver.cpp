#include <vector>

#include "solver.h"

using namespace std;

Solver::BlockLists::BlockLists(const Matrix& matrix)
{
	for (int r = 0; r < matrix.height; ++r)
	{
		horizontalBlocks.push_back(std::vector<bool>());
		std::vector<bool>& blocks = horizontalBlocks[r];
		for (int b = 0; b < matrix.horizontalBlockCounts[r]; ++b)
		{
			blocks.push_back(false);
		}
	}

	for (int c = 0; c < matrix.width; ++c)
	{
		verticalBlocks.push_back(std::vector<bool>());
		std::vector<bool>& blocks = verticalBlocks[c];
		for (int b = 0; b < matrix.verticalBlockCounts[c]; ++b)
		{
			blocks.push_back(false);
		}
	}
}

bool Solver::solve(Matrix& matrix, int index, BlockLists &blocksLists)
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
	int column = index%matrix.width;
	int row    = index/matrix.width;
	if (e.type == ELEMENT_EMPTY)
	{
		if (  !blocksLists.horizontalBlocks[row][e.horizontalBlockIndex]
		    &&!blocksLists.verticalBlocks[column][e.verticalBlockIndex])
		{
			e.type = ELEMENT_LIGHT;
			blocksLists.horizontalBlocks[row][e.horizontalBlockIndex] = true;
			blocksLists.verticalBlocks[column][e.verticalBlockIndex] = true;

			if (solve(matrix, index+1, blocksLists))
			{
				return true;
			}

			e.type = ELEMENT_EMPTY;
			blocksLists.horizontalBlocks[row][e.horizontalBlockIndex] = false;
			blocksLists.verticalBlocks[column][e.verticalBlockIndex] = false;
		}
	}
	if (solve(matrix, index+1, blocksLists))
	{
		return true;
	}
	return false;
}

bool Solver::solve(Matrix& matrix)
{
	BlockLists blocksLists(matrix);
	return solve(matrix, 0, blocksLists);
}
