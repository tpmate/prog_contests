#include <cstdlib>
#include <ctime>
#include <sys/time.h>

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
{
	timeval tv;
	gettimeofday(&tv,NULL);
	std::srand(tv.tv_usec); // use current time as seed for random generator
}

std::string Generator::getNextMatrix(int width, int height, int wallProbability, int numberedProbability)
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
				/* make sure that there is no problem at the corners or at the walls */
				if ((r==0)||(r==height-1))
				{
					if (( (c==0) || (c==width-1) ) && type>2)
					{
						type=2;
					} else {
						if (type>3)
						{
							type=3;
						}
					}
				}
				if (( (c==0) || (c==width-1) ) && type>3)
				{
					type=3;
				}

				result += '0' + type;

				/* And numbered walls should not be restricted by other walls(at least from top and left, right now),
				 * just to be sure */
				if (r > 0)
				{
					//result[result.length()-1-width] =' ';
					//result[result.length()-1-1] =' ';
				}

			} else {
				result += ' ';
			}
		}
		result += '\n';
	}
	return result;
}
