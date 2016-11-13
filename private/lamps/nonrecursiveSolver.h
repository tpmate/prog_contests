#ifndef __NONRECURSIVE_SOLVER_H__
#define __NONRECURSIVE_SOLVER_H__

#include <vector>
#include "matrix.h"

class NonrecursiveSolver
{
	bool sanityCheck(const Matrix& matrix) const;
public:
	bool solve(Matrix& matrix);
};

#endif /* __NONRECURSIVE_SOLVER_H__ */
