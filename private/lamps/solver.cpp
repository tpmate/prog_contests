#include <vector>

#include "solver.h"

using namespace std;

bool Solver::solve(Matrix& matrix, int index)
{
	while (index < matrix.height*matrix.width)
	{
		Element& e = matrix.elements[index];
		int column = index%matrix.width;
		int row    = index/matrix.width;

		/*
		fprintf(stderr, "Try to solve matrix (row=%d, column=%d):\n", row, column);
		matrix.print();
		//*/

		/* We go through the elements line by line. If we are not in the first block, and
		 * the previous vertical block is not lit, then there is no point in continuing the
		 * search, the resulting matrix will not be valid.*/
		if (e.verticalBlockIndex>0 && (matrix.element(column, row-1).type&ANY_WALL))
		{
			//fprintf (stderr, "Just started a new vertical block with index %d\n", index);
			Matrix::BlockData& prevVerticalBlock =
					matrix.blockListLists.verticalBlockListList[column][e.verticalBlockIndex-1];

			if (!prevVerticalBlock.hasLigth)
			{
				/* Ok, it seems that this block has no light in it. But it can happen,
				 * that all its crossing horizontal blocks have. Let's check */

				for (int r = prevVerticalBlock.blockStart; r < prevVerticalBlock.blockEnd; ++r)
				{
					Element& currentPrevElement = matrix.element(column, r);
					Matrix::BlockData& CurrentPrevHorizontalBlock =
							matrix.blockListLists.horizontalBlockListList[r][currentPrevElement.horizontalBlockIndex];

					if (!CurrentPrevHorizontalBlock.hasLigth)
					{
						fprintf (stderr, "Giving up at index %d\n", index);
						return false;
					}
				}
			}
		}

		/* The matrix seems to be valid so far, let's continue the search for a solution. */
		if (e.type == ELEMENT_EMPTY)
		{
			/* we are about to try to put down a lightsource here. But can we?
			 * There might be restrictions here by numbered walls. Let's check*/

			bool tryWithLamp = true;
			bool tryWithoutLamp = true;


			/* First check the one above. */
			if (row > 0)
			{
				Element& otherElement = matrix.element(column, row-1);
				if (otherElement.type&RESTRICTED_WALL)
				{
					int currentLightCount = matrix.getLightCount(column, row-1);
					int validLightCount = matrix.getLightCountForType(otherElement.type);
					if (currentLightCount+1 > validLightCount)
					{
						tryWithLamp = false;
					}
					if (currentLightCount < validLightCount){
						tryWithoutLamp = false;
					}
				}
			}

			/* then the one to the left. */
			if (column > 0)
			{
				Element& otherElement = matrix.element(column-1, row);
				if (otherElement.type&RESTRICTED_WALL)
				{
					int currentLightCount = matrix.getLightCount(column-1, row);
					int validLightCount = matrix.getLightCountForType(otherElement.type);
					if (currentLightCount+1 > validLightCount)
					{
						tryWithLamp = false;
					}
					if (currentLightCount+1 < validLightCount)
					{
						tryWithoutLamp = false;
					}
				}
			}

			/* then the one to the right. */
			if (column < matrix.width-1)
			{
				Element& otherElement = matrix.element(column+1, row);
				if (otherElement.type&RESTRICTED_WALL)
				{
					int currentLightCount = matrix.getLightCount(column+1, row);
					int validLightCount = matrix.getLightCountForType(otherElement.type);
					if (currentLightCount+1 > validLightCount)
					{
						tryWithLamp = false;
					}
					if (currentLightCount+2 < validLightCount)
					{
						tryWithoutLamp = false;
					}
				}
			}

			/* then the one below. */
			if (row < matrix.height-1)
			{
				Element& otherElement = matrix.element(column, row+1);
				if (otherElement.type&RESTRICTED_WALL)
				{
					int currentLightCount = matrix.getLightCount(column, row+1);
					int validLightCount = matrix.getLightCountForType(otherElement.type);
					if (currentLightCount+1 > validLightCount)
					{
						tryWithLamp = false;
					}
					if (currentLightCount+3 < validLightCount)
					{
						tryWithoutLamp = false;
					}
				}
			}

			if (tryWithLamp)
			{
				Matrix::BlockData& horizontalBlockData =
						matrix.blockListLists.horizontalBlockListList[row][e.horizontalBlockIndex];
				Matrix::BlockData& verticalBlockData =
						matrix.blockListLists.verticalBlockListList[column][e.verticalBlockIndex];

				if (  !horizontalBlockData.hasLigth
					&&!verticalBlockData.hasLigth)
				{
					e.type = ELEMENT_LIGHT;
					horizontalBlockData.hasLigth = true;
					verticalBlockData.hasLigth = true;

					if (solve(matrix, index+1))
					{
						return true;
					}

					e.type = ELEMENT_EMPTY;
					horizontalBlockData.hasLigth = false;
					verticalBlockData.hasLigth = false;
				}
			}
			if (!tryWithoutLamp)
			{
				return false;
			}
		}
		++index;
	}
	if (index >= matrix.height*matrix.width)
	{
		/*
		fprintf(stderr, "Checking validity of matrix:\n");
		matrix.print();
		//*/
		return matrix.checkLights() && matrix.isAllLit();
	}
	return false;
}

bool Solver::solve(Matrix& matrix)
{
	return solve(matrix, 0);
}
