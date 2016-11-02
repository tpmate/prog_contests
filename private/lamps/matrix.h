#ifndef __MATRIX_H__
#define __MATRIX_H__

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

#define ANY_WALL (     \
		ELEMENT_WALL | \
		ELEMENT_WALL0| \
		ELEMENT_WALL1| \
		ELEMENT_WALL2| \
		ELEMENT_WALL3| \
		ELEMENT_WALL4  )

#define INDEX_OF(column, row, width) ((row)*(width)+(column))

typedef struct
{
	ElementType type;
} Element;

void getDataFromStdin (Element** matrix, size_t* width, size_t* height);
void getDataFromFile(const char *inputFileName, Element** matrix, size_t* width, size_t* height);

void printMatrix (Element* matrix, size_t width, size_t height);

#endif /* __MATRIX_H__ */
