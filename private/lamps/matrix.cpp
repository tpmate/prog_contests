#include <cstdio>
#include <string>
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

using namespace std;

Element& Matrix::element(int column, int row)
{
	return elements[row*width+column];
}

const Element& Matrix::element(int column, int row) const
{
	return elements[row*width+column];
}

void Matrix::calculateBlockStuff()
{
	/* do the horizontal stuff */
	{
		std::vector<std::vector<BlockData> >& blockListList = blockListLists.horizontalBlockListList;
		for (int r = 0; r < height; ++r)
		{
			blockListList.push_back(std::vector<BlockData>());
			std::vector<BlockData>& blockList = blockListList.back();
			int currentBlockIndex = 0;
			int blockStartIdx = -1;
			for (int c = 0; c < width; ++c)
			{
				Element& e = element(c, r);
				if (e.type == ELEMENT_EMPTY)
				{
					if (blockStartIdx == -1)
					{
						blockList.push_back(BlockData());
						blockStartIdx = c;
					}
					e.horizontalBlockIndex = currentBlockIndex;
				} else {
					if (blockStartIdx != -1)
					{
						BlockData& blockData = blockList.back();
						blockData.blockStart = blockStartIdx;
						blockData.blockEnd = c;
						blockStartIdx = -1;
						++currentBlockIndex;
					}
					e.horizontalBlockIndex = -1;
				}
			}
			if (blockStartIdx != -1)
			{
				BlockData& blockData = blockList.back();
				blockData.blockStart = blockStartIdx;
				blockData.blockEnd = width;
			}
		}
	}
	/* do the vertical stuff. Argh, evil copy paste*/
	{
		std::vector<std::vector<BlockData> >& blockListList = blockListLists.verticalBlockListList;
		for (int c = 0; c < width; ++c)
		{
			blockListList.push_back(std::vector<BlockData>());
			std::vector<BlockData>& blockList = blockListList.back();
			int currentBlockIndex = 0;
			int blockStartIdx = -1;
			for (int r = 0; r < height; ++r)
			{
				Element& e = element(c, r);
				if (e.type == ELEMENT_EMPTY)
				{
					if (blockStartIdx == -1)
					{
						blockList.push_back(BlockData());
						blockStartIdx = c;
					}
					e.verticalBlockIndex = currentBlockIndex;
				} else {
					if (blockStartIdx != -1)
					{
						BlockData& blockData = blockList.back();
						blockData.blockStart = blockStartIdx;
						blockData.blockEnd = c;
						blockStartIdx = -1;
						++currentBlockIndex;
					}
					e.verticalBlockIndex = -1;
				}
			}
			if (blockStartIdx != -1)
			{
				BlockData& blockData = blockList.back();
				blockData.blockStart = blockStartIdx;
				blockData.blockEnd = width;
			}
		}
	}
}

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

int Matrix::getLightCount (int column, int row) const
{
	int lightCount = 0;
	if (column > 0        && element(column-1, row).type==ELEMENT_LIGHT) ++lightCount;
	if (column < width-1  && element(column+1, row).type==ELEMENT_LIGHT) ++lightCount;
	if (row > 0           && element(column, row-1).type==ELEMENT_LIGHT) ++lightCount;
	if (row < height-1    && element(column, row+1).type==ELEMENT_LIGHT) ++lightCount;
	return lightCount;
}


int Matrix::getLightCountForType(const ElementType& type) const
{
	switch (type)
	{
		case ELEMENT_WALL0: return 0;
		case ELEMENT_WALL1: return 1;
		case ELEMENT_WALL2: return 2;
		case ELEMENT_WALL3: return 3;
		case ELEMENT_WALL4: return 4;
		default: return -1;
	}
}


bool Matrix::checkLights ()
{
	for (int c = 0; c < width; ++c)
	{
		for (int r = 0; r < height; ++r)
		{
			int currentIndex = r*width+c;
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
				int lightCount = getLightCount(c, r);
				if (lightCount != getLightCountForType(e.type))
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool Matrix::isAllLit ()
{
	for (int c = 0; c < width; ++c)
	{
		for (int r = 0; r < height; ++r)
		{
			int currentIndex = r*width+c;
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

Matrix* Matrix::getDataFromString(const std::string& s, int width, int height)
{
	Matrix* matrix = 0;
	int cc, cl; /* Current column, Current line */
	int newline; /* whether the parser is in newline state */
	int bytesLeft = s.length();

	matrix = new Matrix();

	cc = 0;
	cl = 0;
	newline=0;

	int cursor = 0;
	while (bytesLeft)
	{
		switch (s[cursor])
		{
			case '\r':
			case '\n':
				if (!newline)
				{
					if (cc != width)
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
				if (cc >= width)
				{
					fprintf(stderr, "Line %d seems to be too long.\n", (int)(cl+1));
					goto error;
				}
				{
					Element e;
					switch (s[cursor])
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
				fprintf(stderr, "Invalid character 0x%02x\n", s[cursor]);
				goto error;
		}
		--bytesLeft;
		++cursor;
	}

	if (cc != width)
	{
		fprintf(stderr, "Line %d seems to be too short.\n", (int)(cl+1));
		goto error;
	}

	matrix->width = width;
	matrix->height = cl+1;
	matrix->calculateBlockStuff();
	return matrix;

error:
	if (matrix)
	{
		delete(matrix);
		matrix = 0;
	}
	return 0;
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
	matrix->calculateBlockStuff();
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
	int r, c;

	string elementTypeHeader("matrix:");
	string horizontalBlocksHeader("horizontal blocks:");
	string verticalBlocksHeader("vertical blocks:");

	fprintf (stderr, "%s  %s  %s\n",
			elementTypeHeader.c_str(),
			horizontalBlocksHeader.c_str(),
			verticalBlocksHeader.c_str());

	for (c = 0; c < (width*4)+13; ++c)
	{
		fprintf(stderr, " ");
	}
	for (c = 0; c < width; ++c)
	{
		fprintf(stderr, "% 3d", (int)blockListLists.verticalBlockListList[c].size());
	}
	fprintf (stderr, "\n");

	for (r = 0; r < height; ++r)
	{
		fprintf (stderr, "[");
		for (c = 0; c < width; ++c)
		{
			char outChar;
			switch (element(c, r).type)
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
		fprintf (stderr, "]  % 3d [", (int)blockListLists.horizontalBlockListList[r].size());
		for (c = 0; c < width; ++c)
		{
			fprintf(stderr, "% 3d", element(c, r).horizontalBlockIndex);
		}
		fprintf (stderr, "]  [");
		for (c = 0; c < width; ++c)
		{
			fprintf(stderr, "% 3d", element(c, r).verticalBlockIndex);
		}
		fprintf (stderr, "]\n");
	}
}
