#include "imdb_frontend.h"

#define MAX_ARG 4
#define MIN_ARG 2


int main( int argc, char *argv[] )  {
	if( argc < MIN_ARG + 1 ) {
		fprintf(stderr, "Error: Muy pocos argumentos\n");
		return EXIT_FAILURE;
	}
	else if( argc  > MAX_ARG + 1 ) {
		fprintf(stderr, "Error: Demasiados argumentos\n");
		return EXIT_FAILURE;
	}
	int check, yMin, yMax;
	int yearCheck = 1;
	if (argc == MIN_ARG + 1){
		yMin = 0;
		yMax = 0;
	}
	if (argc > MIN_ARG + 1){
		if ((yMin = atoi(argv[3])) <= 0){
			yearCheck = 0;
		}

	}
	if (argc == MAX_ARG + 1){
		yMax = atoi(argv[4]);
		if ((yMax = atoi(argv[4])) <= 0){
			yearCheck = 0;
		}
	}

	if (yearCheck == 0){
		fprintf(stderr, "Error: Los años deben ser numeros naturales\n");
		return EXIT_FAILURE;
	}

//	check = imdb_frontend_main(argv[1], argv[2], yMin, yMax);

}
