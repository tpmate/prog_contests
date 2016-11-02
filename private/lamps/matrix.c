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

static void getData(FILE* inputFile, Element** matrix, size_t* width, size_t* height)
{
	size_t i;
	char buf [1024];
	size_t bytesRead;
	size_t l_width = 0; /* width of the matrix */
	size_t l_height = 0; /* height of the matrix */
	Element* l_matrix = 0;
	size_t cc, cl; /* Current column, Current line */
	int newline; /* whether the parser is in newline state */

	*matrix = 0;
	*width = 0;
	*height = 0;

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
			l_height = i;
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

	/* First assume a square area of matrix. */
	l_matrix  = (Element*)malloc(l_width*l_height*sizeof(Element));
	if (!l_matrix)
	{
		fprintf(stderr, "Out of memory\n");
		goto error;
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
					if (cl >= l_height)
					{
						/*uhh, the matrix is too high, do some realloc... */
						size_t newSize;
						Element* newMatrix;

						l_height*=2;
						newSize = l_width*l_height*sizeof(Element);
						newMatrix = (Element*)realloc(l_matrix, newSize);
						if (!newMatrix)
						{
							fprintf(stderr, "Out of memory\n");
							goto error;
						}
						l_matrix = newMatrix;
					}

					{
						ElementType e;
						switch (buf[cursor])
						{
							case EMPTY:
								e = ELEMENT_EMPTY; break;
							case LIGHT:
								e = ELEMENT_LIGHT; break;
							case WALL:
								e = ELEMENT_WALL; break;
							case WALL0:
								e = ELEMENT_WALL0; break;
							case WALL1:
								e = ELEMENT_WALL1; break;
							case WALL2:
								e = ELEMENT_WALL2; break;
							case WALL3:
								e = ELEMENT_WALL3; break;
							case WALL4:
								e = ELEMENT_WALL4; break;
							default:
								fprintf(stderr, "Invalid element, Internal error...\n");
								goto error;
						}
						/* fprintf(stderr, "setting column %d row %d -> index %d to '%c'\n", (int)cc, (int)cl, (int)INDEX_OF(cc, cl, l_width), buf[cursor]); */
						l_matrix[INDEX_OF(cc, cl, l_width)].type = e;
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

	*matrix = l_matrix;
	*width = l_width;
	*height = cl+1;
	return;

error:
	if (l_matrix)
	{
		free (l_matrix);
		l_matrix = 0;
	}
}

void getDataFromStdin (Element** matrix, size_t* width, size_t* height)
{
	fprintf(stderr, "Reading from standard input...\n");
	getData (stdin, matrix, width, height);
	fprintf(stderr, "Done.\n");
}

void getDataFromFile(const char *inputFileName, Element** matrix, size_t* width, size_t* height)
{
	FILE* inputFile;

	*matrix = 0;
	*width = 0;
	*height = 0;

	inputFile = fopen(inputFileName, "rb");

	if (!inputFile)
	{
		fprintf(stderr, "Failed to open %s\n", inputFileName);
		return;
	}

	fprintf(stderr, "Reading from %s...\n", inputFileName);
	getData(inputFile, matrix, width, height);
	fprintf(stderr, "Done.\n");

	fclose(inputFile);
}


void printMatrix (Element* matrix, size_t width, size_t height)
{
	size_t r, c;

	for (r = 0; r < height; ++r)
	{
		for (c = 0; c < width; ++c)
		{
			char outChar;
			switch (matrix[INDEX_OF(c, r, width)].type)
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
