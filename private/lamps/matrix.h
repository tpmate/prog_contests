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


struct Element
{
	ElementType type;
	int index; /* index in the matrix vector */
	int horizontalBlockIndex;
	int verticalBlockIndex;
};

struct Matrix
{
private:
	void calculateBlockStuff();
	Matrix(): elements(), horizontalBlockListList(true), verticalBlockListList(false), width(0), height(0){}
public:
	struct BlockData {
		int hasLigth;
		int blockStart; /* This index is in the block. */
		int blockEnd; /* This index is NOT in the block. */
		BlockData() :hasLigth(false), blockStart(-1), blockEnd(-1) {}
	};
	struct BlockListList
	{
		bool isHorizontal;
		std::vector<std::vector<BlockData> > list;
		BlockListList(bool isHorizontal);
	};

	std::vector<Element> elements;
	BlockListList horizontalBlockListList;
	BlockListList verticalBlockListList;
	int width;
	int height;

	static Matrix* getData(FILE* inputFile);
	static Matrix* getDataFromStdin ();
	static Matrix* getDataFromFile(const char *inputFileName);
	static Matrix* getDataFromString(const std::string& s, int width, int height);

	void print();
	int getLightCount (int column, int row) const;
	int getLightCountForType(const ElementType& type) const;
	bool isNextnonEmptyALight(int currentIndex, int difference, int count);
	bool checkLights ();
	bool isAllLit ();
	Element& element(int column, int row);
	const Element& element(int column, int row) const;
	int indexOf(int column, int row) const;

};

#endif /* __MATRIX_H__ */
