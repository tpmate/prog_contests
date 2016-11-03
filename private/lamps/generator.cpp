#include <cstdlib>
#include <ctime>

#include "generator.h"

using namespace std;

int Generator::GetInt ()
{
	int random_variable;
	do
	{
		random_variable = std::rand();
	} while (random_variable >= 1000000000);
	return random_variable;
}

Generator::Generator ()
: wallProbability(100)
, numberedProbability(20)
{
	std::srand(std::time(0)); // use current time as seed for random generator
}

std::string Generator::getNextMatrix(int width, int height)
{
	string result;

	for (int r = 0; r < height; ++r)
	{
		for (int c = 0; c < width; ++c)
		{
			int random_variable = GetInt();
			if (random_variable < wallProbability*1000000)
			{
				result += '#';
			} else if (random_variable < (wallProbability+numberedProbability)*1000000)
			{
				int type = GetInt()%5;
				result += '0' + type;
			} else {
				result += ' ';
			}
		}
		result += '\n';
	}
	return result;
}
