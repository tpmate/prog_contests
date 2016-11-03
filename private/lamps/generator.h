#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <string>

class Generator
{
	/* Probabilities are in 0.1% unit */
	int wallProbability;
	int numberedProbability;

	int GetInt();
public:
	Generator();
	std::string getNextMatrix(int width, int height);
};

#endif /* __GENERATOR_H__ */
