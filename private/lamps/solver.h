#ifndef __SOLVER_H__
#define __SOLVER_H__

#include <vector>
#include "matrix.h"

class Solver
{
	bool solve(Matrix& matrix, int index);
	bool sanityCheck(const Matrix& matrix) const;
public:
	bool solve(Matrix& matrix);
};

#endif /* __SOLVER_H__ */
