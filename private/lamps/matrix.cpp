#include <stdio.h>
#include <malloc.h>

#include "matrix.h"

#define INPUT_BUF_SIZE 256

#define EMPTY ' '
#define LIGHT 'o'
#define WALL  '#'
#define WALL0 '0'
#define WALL1 '1'
#define WALL2 '2'
#define WALL3 '3'
#define WALL4 '4'

bool Matrix::isNextnonEmptyALight(int currentIndex, int difference, int count)
{
	int toCheck = currentIndex + difference;
	for (int i = 0; i < count; ++i)
	{
		Element& e = elements[toCheck];
		if (e.type == ELEMENT_EMPTY)
		{
			toCheck += difference;
			continue;
		}
		if (e.type == ELEMENT_LIGHT)
		{
			return true;
		} else
		{
			return false;
		}
	}
	return false;
}

bool Matrix::checkLights ()
{
	for (size_t c = 0; c < width; ++c)
	{
		for (size_t r = 0; r < height; ++r)
		{
			int currentIndex = INDEX_OF(c, r, width);
			Element& e = elements[currentIndex];
			if (e.type == ELEMENT_LIGHT)
			{
				bool isOK =
						!isNextnonEmptyALight(currentIndex, -1,     c            ) && /* left  */
						!isNextnonEmptyALight(currentIndex,  1,     width - c - 1) && /* right */
						!isNextnonEmptyALight(currentIndex, -width, r            ) && /* up    */
						!isNextnonEmptyALight(currentIndex,  width, height -r -1 );   /* down  */
				if (!isOK)
				{
					return false;
				}
			} else if ((e.type & RESTRICTED_WALL) != 0){
				int lightCount = 0;
				if (c > 0        && elements[INDEX_OF(c-1, r, width)].type==ELEMENT_LIGHT) ++lightCount;
				if (c < width-1  && elements[INDEX_OF(c+1, r, width)].type==ELEMENT_LIGHT) ++lightCount;
				if (r > 0        && elements[INDEX_OF(c, r-1, width)].type==ELEMENT_LIGHT) ++lightCount;
				if (r < height-1 && elements[INDEX_OF(c, r+1, width)].type==ELEMENT_LIGHT) ++lightCount;

				if (e.type == ELEMENT_WALL0 && lightCount != 0) return false;
				else if (e.type == ELEMENT_WALL1 && lightCount != 1) return false;
				else if (e.type == ELEMENT_WALL2 && lightCount != 2) return false;
				else if (e.type == ELEMENT_WALL3 && lightCount != 3) return false;
				else if (e.type == ELEMENT_WALL4 && lightCount != 4) return false;

			}

		}
	}
	return true;
}

bool Matrix::isAllLit ()
{
	for (size_t c = 0; c < width; ++c)
	{
		for (size_t r = 0; r < height; ++r)
		{
			int currentIndex = INDEX_OF(c, r, width);
			Element& e = elements[currentIndex];
			if (e.type == ELEMENT_EMPTY)
			{
				bool isLit =
						isNextnonEmptyALight(currentIndex, -1,     c            ) || /* left  */
						isNextnonEmptyALight(currentIndex,  1,     width - c - 1) || /* right */
						isNextnonEmptyALight(currentIndex, -width, r            ) || /* up    */
						isNextnonEmptyALight(currentIndex,  width, height -r -1 );   /* down  */
				if (!isLit)
				{
					return false;
				}
			}
		}
	}
	return true;
}

Matrix* Matrix::getData(FILE* inputFile)
{
	size_t i;
	char buf [1024];
	size_t bytesRead;
	size_t l_width = 0; /* width of the matrix */
	Matrix* matrix = 0;
	size_t cc, cl; /* Current column, Current line */
	int newline; /* whether the parser is in newline state */

	matrix = new Matrix();

	bytesRead = fread(buf, 1, INPUT_BUF_SIZE, inputFile);
	if (!bytesRead)
	{
		fprintf(stderr, "Failed to read.\n");
		goto error;
	}

	/* read through to the first \n or \r to find out the length of the lines */

	for (i = 0; i < bytesRead; ++i)
	{
		if (buf[i] == '\r' || buf[i] == '\n')
		{
			l_width = i;
			break;
		}
	}

	if (!l_width)
	{
		/* There is maybe a single line?? */
		if (bytesRead < INPUT_BUF_SIZE)
		{
			l_width = bytesRead;
		} else {
			fprintf(stderr, "Uhh, it seems that the line is longer than INPUT_BUF_SIZE\n");
			goto error;
		}
	}

	cc = 0;
	cl = 0;
	newline=0;

	do
	{
		size_t cursor = 0;
		while (bytesRead)
		{
			switch (buf[cursor])
			{
				case '\r':
				case '\n':
					if (!newline)
					{
						if (cc != l_width)
						{
							fprintf(stderr, "Line %d seems to be too short.\n", (int)(cl+1));
							goto error;
						}
						newline=1;
					}
					break;
				case EMPTY:
				case LIGHT:
				case WALL:
				case WALL0:
				case WALL1:
				case WALL2:
				case WALL3:
				case WALL4:
					if (newline)
					{
						newline = 0;
						++cl;
						cc=0;
					}
					if (cc >= l_width)
					{
						fprintf(stderr, "Line %d seems to be too long.\n", (int)(cl+1));
						goto error;
					}
					{
						Element e;
						switch (buf[cursor])
						{
							case EMPTY:
								e.type = ELEMENT_EMPTY; break;
							case LIGHT:
								e.type = ELEMENT_LIGHT; break;
							case WALL:
								e.type = ELEMENT_WALL; break;
							case WALL0:
								e.type = ELEMENT_WALL0; break;
							case WALL1:
								e.type = ELEMENT_WALL1; break;
							case WALL2:
								e.type = ELEMENT_WALL2; break;
							case WALL3:
								e.type = ELEMENT_WALL3; break;
							case WALL4:
								e.type = ELEMENT_WALL4; break;
							default:
								fprintf(stderr, "Invalid element, Internal error...\n");
								goto error;
						}
						/* fprintf(stderr, "setting column %d row %d -> index %d to '%c'\n", (int)cc, (int)cl, (int)INDEX_OF(cc, cl, l_width), buf[cursor]); */
						matrix->elements.push_back(e);
					}
					++cc;
					break;
				default:
					fprintf(stderr, "Invalid character 0x%02x\n", buf[cursor]);
					goto error;
			}
			--bytesRead;
			++cursor;
		}
		bytesRead = fread(buf, 1, INPUT_BUF_SIZE, inputFile);
	} while (bytesRead);

	if (cc != l_width)
	{
		fprintf(stderr, "Line %d seems to be too short.\n", (int)(cl+1));
		goto error;
	}

	matrix->width = l_width;
	matrix->height = cl+1;
	return matrix;

error:
	if (matrix)
	{
		delete(matrix);
		matrix = 0;
	}
	return 0;
}

Matrix* Matrix::getDataFromStdin ()
{
	Matrix* matrix;
	fprintf(stderr, "Reading from standard input...\n");
	matrix = Matrix::getData (stdin);
	fprintf(stderr, "Done.\n");
	return matrix;
}

Matrix* getDataFromFile(const char *inputFileName)
{
	FILE* inputFile;

	Matrix* matrix = 0;

	inputFile = fopen(inputFileName, "rb");

	if (!inputFile)
	{
		fprintf(stderr, "Failed to open %s\n", inputFileName);
		return 0;
	}

	fprintf(stderr, "Reading from %s...\n", inputFileName);
	matrix = Matrix::getData(inputFile);
	fprintf(stderr, "Done.\n");

	fclose(inputFile);

	return matrix;
}


void Matrix::print ()
{
	size_t r, c;

	for (r = 0; r < height; ++r)
	{
		for (c = 0; c < width; ++c)
		{
			char outChar;
			switch (elements[INDEX_OF(c, r, width)].type)
			{
				case ELEMENT_EMPTY:
					outChar = EMPTY; break;
				case ELEMENT_LIGHT:
					outChar = LIGHT; break;
				case ELEMENT_WALL:
					outChar = WALL; break;
				case ELEMENT_WALL0:
					outChar = WALL0; break;
				case ELEMENT_WALL1:
					outChar = WALL1; break;
				case ELEMENT_WALL2:
					outChar = WALL2; break;
				case ELEMENT_WALL3:
					outChar = WALL3; break;
				case ELEMENT_WALL4:
					outChar = WALL4; break;
				default:
					outChar = '?'; break;
			}
			fprintf(stderr, "%c", outChar);
		}
		fprintf (stderr, "\n");
	}
}
