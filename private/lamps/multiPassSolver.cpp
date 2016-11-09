
#include <vector>

#include "multiPassSolver.h"
#include "matrix.h"

using namespace std;

/* Ok, for this stuff the idea is the following:
 * We think, that finding the solution for a matrix that have no numbered wall in it is very easy.
 * the difficulty is coming in with the numbered walls.
 * So instead trying to solve the whole problem at once, we do it in two passes. First we try to find a solution in the
 * local neighborhood of the numbered walls. We create distinct (!) regions from the neighborhood the following way:
 * have the numbered wall. have the blocks that are going in the four directions (if there is any) and have the blocks
 * being orthogonal for these blocks. The task should be solved for this irregularly shaped field.
 * Please note that it is not a problem if the shape is not convex because the block mechanism prevents us from
 * calculating with the gaps as walls.
 * It is important to handle the areas distinct, so that we solve multiple small problems instead of one larger (even if
 * it is smaller than the original problem. )
 * When the small local problems are solved, we solve the problem for the remaining part, considering the previous solution.
 * Important: this way the original algorithm is executed on all the matrix elements exactly once, as it was earlier,
 * however the difficult stuff is isolated, and solved independently from the other parts. */

#if 0
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
					matrix.verticalBlockListList.list[column][e.verticalBlockIndex-1];

			if (!prevVerticalBlock.hasLigth)
			{
				/* Ok, it seems that this block has no light in it. But it can happen,
				 * that all its crossing horizontal blocks have. Let's check */

				for (int r = prevVerticalBlock.blockStart; r < prevVerticalBlock.blockEnd; ++r)
				{
					Element& currentPrevElement = matrix.element(column, r);
					Matrix::BlockData& CurrentPrevHorizontalBlock =
							matrix.horizontalBlockListList.list[r][currentPrevElement.horizontalBlockIndex];

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
						matrix.horizontalBlockListList.list[row][e.horizontalBlockIndex];
				Matrix::BlockData& verticalBlockData =
						matrix.verticalBlockListList.list[column][e.verticalBlockIndex];

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
#endif

void mapBlocks(const Matrix& matrix,
			   const Matrix::BlockData& primaryBlock,
			   int primaryBlockListIndex,
			   const Matrix::BlockListList& secondaryBlockListList)
{
	for (int secondaryCrossElementIndex = primaryBlock.blockStart;
			secondaryCrossElementIndex < primaryBlock.blockEnd;
			++secondaryCrossElementIndex)
	{
		int crossElementcolumn;
		int crossElementrow;

		if (secondaryBlockListList.isHorizontal)
		{
			crossElementcolumn = primaryBlockListIndex;
			crossElementrow = secondaryCrossElementIndex;
		} else {
			crossElementcolumn = secondaryCrossElementIndex;
			crossElementrow = primaryBlockListIndex;
		}

		const Element& crossElement = matrix.element(crossElementcolumn, crossElementrow);

		int secondaryBlockIndex;

		if (secondaryBlockListList.isHorizontal)
		{
			secondaryBlockIndex = crossElement.horizontalBlockIndex;
		} else {
			secondaryBlockIndex = crossElement.verticalBlockIndex;
		}

		const Matrix::BlockData& secondaryBlock = secondaryBlockListList.list[secondaryCrossElementIndex][secondaryBlockIndex];

		for (int mapElementIndex = secondaryBlock.blockStart;
				mapElementIndex < secondaryBlock.blockEnd;
				++mapElementIndex)
		{
			int mapRow;
			int mapColumn;

			if (secondaryBlockListList.isHorizontal)
			{
				mapColumn = mapElementIndex;
				mapRow = secondaryCrossElementIndex;
			} else {
				mapColumn = secondaryCrossElementIndex;
				mapRow = mapElementIndex;
			}

			fprintf (stderr, "Checking (%d, %d)\n", mapColumn, mapRow);
/*
			int index = matrix.indexOf(bc, br);
			int& mapElement = map[index];
			if (mapElement == -1)
			{
				mapElement = currentListId;
				listUsed = true;
			} else if (mapElement != currentListId)
			{
				listUsed = true;
				lists[mapElement] = currentListId;
			}
*/
		}
	}
}

void mapEnvironment(const Matrix& matrix, int column, int row)
{
	//bool listUsed = false;
	/*set the list value of the map*/
	/*go up*/
	if (row > 0)
	{
		const Element& e1 = matrix.element(column, row-1);
		if (e1.type==ELEMENT_EMPTY)
		{
			const Matrix::BlockData& primaryBlock = matrix.verticalBlockListList.list[column][e1.verticalBlockIndex];
			mapBlocks(matrix, primaryBlock, column, matrix.horizontalBlockListList);
		}
	}
	if (row < matrix.height-1)
	{
		const Element& e1 = matrix.element(column, row+1);
		if (e1.type==ELEMENT_EMPTY)
		{
			const Matrix::BlockData& primaryBlock = matrix.verticalBlockListList.list[column][e1.verticalBlockIndex];
			mapBlocks(matrix, primaryBlock, column, matrix.horizontalBlockListList);
		}
	}
	if (column > 0)
	{
		const Element& e1 = matrix.element(column-1, row);
		if (e1.type==ELEMENT_EMPTY)
		{
			const Matrix::BlockData& primaryBlock = matrix.horizontalBlockListList.list[row][e1.horizontalBlockIndex];
			mapBlocks(matrix, primaryBlock, row, matrix.verticalBlockListList);
		}
	}
	if (column < matrix.width-1)
	{
		const Element& e1 = matrix.element(column+1, row);
		if (e1.type==ELEMENT_EMPTY)
		{
			const Matrix::BlockData& primaryBlock = matrix.horizontalBlockListList.list[row][e1.horizontalBlockIndex];
			mapBlocks(matrix, primaryBlock, row, matrix.verticalBlockListList);
		}
	}
}

void RunPass1 (Matrix& matrix)
{
	/* ---------------------------------------------------------------------- */
	/* first try to create the Map of the matrix to have the regions to solve */
	vector<int> map (matrix.elements.size(), -1);
	/* at each listIndex the value is the index of the parent list.
	 * A list has no parent if it has the value -1*/
	vector<int> lists (matrix.elements.size(), -1);
	/*first look for the numbered walls*/
	//int currentListId = 0;
	for (int r = 0; r < matrix.height; ++r)
	{
		for (int c = 0; c < matrix.width; ++c)
		{
			Element& e = matrix.element(c, r);
			if (e.type & RESTRICTED_WALL)
			{
				mapEnvironment(matrix, c, r);
			}
		}
	}

	/* then create the list of the elements in each region (lists of indexes)*/

	/* then solve each region */

}

bool MutiPassSolver::solve(Matrix& matrix)
{
	RunPass1(matrix);
	return true;
}

