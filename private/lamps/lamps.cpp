#include <cstdio>
#include <cstdlib>
#include <string>


#include "matrix.h"
#include "solver.h"
#include "generator.h"
#include "multiPassSolver.h"
#include "nonrecursiveSolver.h"

using namespace std;

#ifdef __CYGWIN__
#define EXECUTABLE_NAME "lamps.exe"
#else
#define EXECUTABLE_NAME "lamps"
#endif

#define MODE_GEN "gen"
#define MODE_BUILTIN "builtin"
#define MODE_STDIN "stdin"
#define MODE_FILE "file"
#define MODE_HELP "help"

void printHelp()
{
	printf ("Usage:\n");
	printf ("\t%s -%s <width> <height> <# prob> <[0-4] prob>\n", EXECUTABLE_NAME, MODE_GEN);
	printf ("\t%s -%s\n", EXECUTABLE_NAME, MODE_BUILTIN);
	printf ("\t%s -%s\n", EXECUTABLE_NAME, MODE_STDIN);
	printf ("\t%s -%s filename\n", EXECUTABLE_NAME, MODE_FILE);
	printf ("\t%s -%s\n\n", EXECUTABLE_NAME, MODE_HELP);
	printf ("Probabilities are integers, and in the unit of 0.1%%\n\n");
}

int main (int argc, char** argv)
{
	Matrix* matrix = 0;
	Generator generator;

	if (argc < 2)
	{
		printHelp();
		return 1;
	}

	if (argv[1][0] != '-')
	{
		printf ("Invalid mode, use '%s -%s'\n", EXECUTABLE_NAME, MODE_HELP);
		return 1;
	}

	string mode (argv[1]+1);

	if (mode==MODE_HELP)
	{
		printHelp();
		return 0;
	} else if (mode == MODE_GEN)
	{
		if (argc != 6)
		{
			printf ("Invalid number of parameters, use '%s -%s'\n", EXECUTABLE_NAME, MODE_HELP);
			return 1;
		}
		int width = atoi(argv[2]);
		int height = atoi(argv[3]);
		int wallProb = atoi(argv[4]);
		int numberedWallProb = atoi(argv[5]);
		if (width<0||width>=256||height<0||height>256)
		{
			printf ("Invalid with(%d) or height(%d). Valid values are in the interval [0, 256)\n", width, height);
			return 1;
		}
		if (wallProb<0||wallProb>1000||
			numberedWallProb<0||numberedWallProb>1000||
			wallProb+numberedWallProb>1000)
		{
			printf ("Invalid probabilities (%d, %d). Valid values are in the interval [0, 1000],\n"
					"and the sum of the probabilities cannot exceed 1000.\n", wallProb, numberedWallProb);
			return 1;
		}
		matrix = Matrix::getDataFromString(generator.getNextMatrix(width, height, wallProb, numberedWallProb), width, height);
	} else if (mode == MODE_BUILTIN)
	{
/*		matrix = Matrix::getDataFromString(string(
				"      # \n      # \n  #     \n    ##  \n   #    \n #   #  \n  #    0\n   #  # ")
				, 8, 8);
*/
		matrix = Matrix::getDataFromString(string(
		" # 2\n    \n   #\n    ")
				, 4, 4);


	} else if (mode==MODE_FILE)
	{
		if (argc != 3)
		{
			printf ("Invalid number of parameters, use '%s -%s'\n", EXECUTABLE_NAME, MODE_HELP);
			return 1;
		}
		matrix = Matrix::getDataFromFile(argv[2]);
	} else if (mode==MODE_STDIN)
	{
		matrix = Matrix::getDataFromStdin();
	} else {
		printf ("Invalid mode, use '%s -%s'\n", EXECUTABLE_NAME, MODE_HELP);
		return 1;
	}

	if (matrix)
	{
		fprintf(stderr, "The original matrix is:\n");
		matrix->printColored();
		fprintf(stderr, "\n");

		Solver solver;
//		MutiPassSolver solver;
//		NonrecursiveSolver solver;

		if (solver.solve(*matrix))
		{
			fprintf(stderr, "A solution is:\n");
			matrix->printColored();
			fprintf(stderr, "\n");
			matrix->print(stdout);
			fprintf(stdout, "\n");
		} else {
			fprintf(stdout, "There is no solution with this matrix.\n");
		}

		delete matrix;
	} else {
		fprintf(stderr, "Failed to parse/generate matrix\n");
		return 1;
	}

	return 0;
}



