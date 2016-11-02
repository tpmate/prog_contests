#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "matrix.h"

class Solver
{
	bool solve(Matrix& matrix, int index);
public:
	bool solve(Matrix& matrix);
};

#endif /* __SOLVER_H__ */
