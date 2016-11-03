#ifndef __SOLVER_H__
#define __SOLVER_H__

#include <vector>
#include "matrix.h"

class Solver
{
	class BlockLists
	{
	public:
		std::vector<std::vector<bool> > horizontalBlocks;
		std::vector<std::vector<bool> > verticalBlocks;
		BlockLists(const Matrix& matrix);
	};

	bool solve(Matrix& matrix, int index, BlockLists &blocksLists);
public:
	bool solve(Matrix& matrix);
};

#endif /* __SOLVER_H__ */
