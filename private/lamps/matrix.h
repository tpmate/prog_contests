#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <vector>
#include <cstdio>
#include <string>


typedef enum {
	ELEMENT_EMPTY = 1u << 0,
	ELEMENT_LIGHT = 1u << 1,
	ELEMENT_WALL  = 1u << 2,
	ELEMENT_WALL0 = 1u << 3,
	ELEMENT_WALL1 = 1u << 4,
	ELEMENT_WALL2 = 1u << 5,
	ELEMENT_WALL3 = 1u << 6,
	ELEMENT_WALL4 = 1u << 7
} ElementType;

#define RESTRICTED_WALL ( \
		ELEMENT_WALL0|    \
		ELEMENT_WALL1|    \
		ELEMENT_WALL2|    \
		ELEMENT_WALL3|    \
		ELEMENT_WALL4     )

#define ANY_WALL (      \
		ELEMENT_WALL |  \
		RESTRICTED_WALL )


#define INDEX_OF(column, row, width) ((row)*(width)+(column))

struct Element
{
	ElementType type;
	int horizontalBlockIndex;
	int verticalBlockIndex;
};

struct Matrix
{
private:
	void calculateBlockStuff();
	Matrix(): elements(), width(0), height(0) {}
public:
	std::vector<Element> elements;
	std::vector<int> horizontalBlockCounts;
	std::vector<int> verticalBlockCounts;
	int width;
	int height;

	static Matrix* getData(FILE* inputFile);
	static Matrix* getDataFromStdin ();
	static Matrix* getDataFromFile(const char *inputFileName);
	static Matrix* getDataFromString(const std::string& s, int width, int height);

	void print();
	bool isNextnonEmptyALight(int currentIndex, int difference, int count);
	bool checkLights ();
	bool isAllLit ();
};

#endif /* __MATRIX_H__ */
