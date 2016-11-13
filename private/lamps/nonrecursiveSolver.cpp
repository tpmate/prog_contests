#include <vector>
#include <cstdlib>

#include "nonrecursiveSolver.h"

using namespace std;

#define PRINT_AND_EXIT_AT(s, c) \
do \
{ \
	static int fosCounter = 0; \
	++fosCounter; \
	if (fosCounter == (c)) \
	{ \
		fprintf (stderr, "%s was hit for the %dth time. exiting.\n", (s), (int)(c)); \
		exit(1); \
	} \
} while(false)

bool lightBlockedByOtherLightOrWall (const Matrix& matrix, int column, int row)
{
	const Element& e = matrix.elements[matrix.indexOf(column, row)];
	return e.type != ELEMENT_EMPTY ||
		matrix.horizontalBlockListList.list[row][e.horizontalBlockIndex].hasLigth ||
		matrix.verticalBlockListList.list[column][e.verticalBlockIndex].hasLigth;
}

void turnlight(Matrix& matrix, int column, int row, bool on)
{
	Element& e = matrix.element(column, row);
	Matrix::BlockData& horizontalBlockData =
			matrix.horizontalBlockListList.list[row][e.horizontalBlockIndex];
	Matrix::BlockData& verticalBlockData =
			matrix.verticalBlockListList.list[column][e.verticalBlockIndex];

	if (  !horizontalBlockData.hasLigth
		&&!verticalBlockData.hasLigth
		&& e.type == ELEMENT_EMPTY
		&& on)
	{
		e.type = ELEMENT_LIGHT;
		horizontalBlockData.hasLigth = true;
		verticalBlockData.hasLigth = true;
		return;
	}

	if (   horizontalBlockData.hasLigth
		&& verticalBlockData.hasLigth
		&& e.type == ELEMENT_LIGHT
		&& !on)
	{
		e.type = ELEMENT_EMPTY;
		horizontalBlockData.hasLigth = false;
		verticalBlockData.hasLigth = false;
		return;
	}

	fprintf (stderr, "Turnlight internal error\n");
	exit(0);

}

/* we return the index from where the searching have to be continued */
int increase (Matrix& matrix)
{
	for (int i = matrix.elements.size(); i >= 0; --i)
	{
		Element& e = matrix.elements[i];
		if (e.type == ELEMENT_LIGHT)
		{
			int column = i%matrix.width;
			int row    = i/matrix.width;
			turnlight(matrix, column, row, false);
			return i+1;
		}
	}
	return -1;
}

bool backtracToAndTurnOff (Matrix& matrix, int fromIndex, int toIndex)
{
	for (int i = fromIndex; i > toIndex; --i)
	{
		if (matrix.elements[i].type == ELEMENT_LIGHT)
		{
			int column = i%matrix.width;
			int row    = i/matrix.width;
			turnlight(matrix, column, row, false);
		}
	}
	if (matrix.elements[toIndex].type == ELEMENT_LIGHT)
	{
		int column = toIndex%matrix.width;
		int row    = toIndex/matrix.width;
		turnlight(matrix, column, row, false);
		return true;
	}
	return false;
}

bool NonrecursiveSolver::solve(Matrix& matrix)
{
	int currentIndex = 0;

//	static int maxIndex = 0;
	while (true)
	{
//		fprintf (stderr, "Checking matrix (currentIndex = %d):\n", currentIndex);
//		matrix.printColored();

		/* lit the current element if have to */

		if (currentIndex >= matrix.height*matrix.width)
		{
			/* Ok, we placed all the lights in a configuration which seemed valid so far.
			 * but is it really valid?
			 */
			if (matrix.checkLights() && matrix.isAllLit())
			{
				/* Yeah, this seems to be a solution! lets return it quickly */
				return true;
			}
			/* Hmm, the current matrix is not a solution. Try to step...*/
			int nextIndex = increase(matrix);
			if (nextIndex != -1)
			{
				/* ok, there are still possibilities to try, let's backtrack, and continue */
				currentIndex = nextIndex;
				continue;
			} else {
				fprintf (stderr, "We tried all the reasonable configs. unfortunately we have to give up :(\n");
				return false;
			}
		}


		bool backtrackDone = false;
		/* Ok, It seems we are not yet at the end. All we want to do now is increase the
		 * index and put down as much light we can do, in a greedy way.)*/
		for (; currentIndex < matrix.height*matrix.width ; ++currentIndex)
		{
			int column = currentIndex%matrix.width;
			int row    = currentIndex/matrix.width;
			Element& element = matrix.element(column, row);
			/* Do dome basic check... */
			/* This is to be done at a later stage */
			/*
			if (lightBlockedByOtherLightOrWall(matrix, column, row))
			{
				continue;
			}
			 */
			/* Ok. There is no obvious reason why we couldn't put down here a light source.
			 * However we have already have some knowledge of the upper part of the matrix.
			 * So in certain cases we might be able to tell that there is no possible way
			 * to place lights on the fields left free to have a solution, so let's skip a
			 * few steps*/

			/* Implement this later*/

			/* If we are about to place a light source around a restricted wall, we might
			 * be able to exclude a few possibilities */
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
						/* we can't put a light source here. */
						continue;
					}
					if (currentLightCount < validLightCount)
					{
						/* we have to put a light source here. */
						if (element.type != ELEMENT_EMPTY
						    || matrix.horizontalBlockListList.list[row][otherElement.horizontalBlockIndex].hasLigth)
						{
							/* it seems that no light source can be put here. */




							/* Ok, no panic. Does it seems feaseble to add a light somewhere that is
							 * not lit yet?*/





							/* Go back till we find a lightSource, and switch it off.*/



							bool success = false;
							int toIndex;

							/* try with the one right to the wall */
							if (column < matrix.width-1){
								toIndex = currentIndex-matrix.width+1;
								success |= backtracToAndTurnOff(matrix, currentIndex-1, toIndex);
							}

							/* try with the one left to the wall */
							if (!success && column > 0){
								toIndex = currentIndex-matrix.width-1;
								success |= backtracToAndTurnOff(matrix, currentIndex-1, toIndex);
							}

							/* try with the one above the wall */
							if (!success && row > 1){
								toIndex = currentIndex-(2*matrix.width);
								success |= backtracToAndTurnOff(matrix, currentIndex-1, toIndex);
							}

							if (success)
							{
								/* we did the backtrack, continue filling the matrix */
								currentIndex = toIndex+1;
								backtrackDone = true;
								break;
							} else {

							}
						}
						/* above us there is a wall, so only the horizontal block matters */
						else if (matrix.horizontalBlockListList.list[row][otherElement.horizontalBlockIndex].hasLigth)
						{

						}else {

						}

						if (lightBlockedByOtherLightOrWall(matrix, column, row))
						{
							/* uh-oh we cannot do that. we have to backtrack a bit...
							 * the element above us is a restricted wall, below us we didn't put any
							 * lights yet, so there is two possibilities
							 */

						}
						turnlight(matrix, column, row, true);
						continue;
					}
				}
			}
#if 0
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
						fprintf (stderr, "3 try...Lamp = false \n");
						tryWithLamp = false;
					}
					if (currentLightCount+1 < validLightCount)
					{
						PRINT_AND_EXIT_AT("CheckLeft - withLamp fail", 1);
						tryWithoutLamp = false;
					}
				}
			}


#endif


			turnlight(matrix, column, row, true);
		}
		if (backtrackDone)
		{
			continue;
		}

	}


	/* The following code is from the previous solver, remove if not necessary anymore.*/
#if 0
		Element& e = matrix.elements[index];
		int column = index%matrix.width;
		int row    = index/matrix.width;

		bool maxReached = index > maxIndex;
		if (maxReached)
		{
			maxIndex = index;
			fprintf (stderr, "\e[HReached (%d, %d)\n", column, row);
		} else {
			fprintf (stderr, "\e[H  \n");
		}
		fprintf (stderr, "current: (%d, %d)\n", column, row);
		matrix.printColored();


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
						fprintf (stderr, "Giving up at index %d, the %dth block in column %d is not lit.\n",
								index, e.verticalBlockIndex-1, column);
						fprintf (stderr, "Also the %dth row of this block is not lit. returning false\n", r);
						exit(0);
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
						fprintf (stderr, "1 try...Lamp = false \n");
						tryWithLamp = false;
					}
					if (currentLightCount < validLightCount){
						fprintf (stderr, "2 try...Lamp = false \n");
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
						fprintf (stderr, "3 try...Lamp = false \n");
						tryWithLamp = false;
					}
					if (currentLightCount+1 < validLightCount)
					{
						PRINT_AND_EXIT_AT("CheckLeft - withLamp fail", 1);
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
						fprintf (stderr, "5 try...Lamp = false \n");
						tryWithLamp = false;
					}
					if (currentLightCount+2 < validLightCount)
					{
						fprintf (stderr, "6 try...Lamp = false \n");
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
						fprintf (stderr, "7 try...Lamp = false \n");
						tryWithLamp = false;
					}
					if (currentLightCount+3 < validLightCount)
					{
						fprintf (stderr, "8 try...Lamp = false \n");
						tryWithoutLamp = false;
					}
				}
			}

			if (!tryWithLamp && !tryWithoutLamp)
			{
				fprintf (stderr, "This is totally bad matrix. stop.\n");
				exit(0);
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
						fprintf (stderr, "returning 4 \n");
						exit(1);
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
		fprintf (stderr, "returning 1 \n");
		exit(1);
		return matrix.checkLights() && matrix.isAllLit();
	}
	fprintf (stderr, "returning 2 \n");
	exit(1);
	return false;
#endif
}

bool NonrecursiveSolver::sanityCheck(const Matrix& matrix) const
{
	for (int r = 0; r < matrix.height; ++r)
	{
		for (int c = 0; c < matrix.width; ++c)
		{
			const Element& e = matrix.element(c, r);
			if (e.type&RESTRICTED_WALL)
			{
				int validLightCount = matrix.getLightCountForType(e.type);
				int emptyAround = 0;

				if (c > 0 && matrix.element(c-1, r).type == ELEMENT_EMPTY) ++emptyAround;
				if (c < matrix.width-1 && matrix.element(c+1, r).type == ELEMENT_EMPTY) ++emptyAround;
				if (r > 0 && matrix.element(c, r-1).type == ELEMENT_EMPTY) ++emptyAround;
				if (r < matrix.height-1 && matrix.element(c, r+1).type == ELEMENT_EMPTY) ++emptyAround;

				if (emptyAround < validLightCount)
				{
					return false;
				}
			}
		}
	}
	return true;
}
