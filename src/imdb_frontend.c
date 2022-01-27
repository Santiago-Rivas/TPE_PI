#include "imdb_frontend.h"

static void printGenres(allGenres * genres);
static int readGenresFile(FILE * genresFile, allGenres * genres);
static void allocError();
static void freeAllGenres(allGenres * genres);

int imdb_frontend_main(char * moviePath, char * genresPath, int yMin, int yMax){
	FILE * genresFile = fopen(genresPath, "r");
	if (genresFile == NULL){
		fprintf(stderr, "Error: No se logro abrir el archivo de generos\n");
		return EXIT_FAILURE;
	}

	FILE * movieFile = fopen(moviePath, "r");
	if (movieFile == NULL){	
		fprintf(stderr, "Error: No se logro abrir el archivo de obras\n");
		fclose(genresFile);
		return EXIT_FAILURE;
	}

	allGenres genres;

	genres.genresName = calloc(1, sizeof(char *) * MAX_GENRES);
	if (genres.genresName == NULL){
		allocError();
		return FALSE;
	}
	genres.nameLengths = calloc(1, sizeof(unsigned int) * MAX_GENRES);
	if (genres.nameLengths == NULL){
		allocError();
		return FALSE;
	}
	genres.dim = 0;

	int check;
	check = readGenresFile(genresFile, &genres);

	if (check == FALSE){
		freeAllGenres(&genres);
		return FALSE;
	}

	// Lectura de obras y llamada al backend
	
	// Impresion de datos
	
	printGenres(&genres);
	
	// Liberar memoria reservada
	freeAllGenres(&genres);
	return TRUE;
}

static void printGenres(allGenres * genres){
	for (int i = 0; i < genres->dim ; i++){
		printf("%s\n", genres->genresName[i]);
	}

}

static int readGenresFile(FILE * genresFile, allGenres * genres){
	char * genreName = malloc((GEN_LINE_MAX_CHARS + 1) * sizeof(char));
	char * safeGenreName;
	if (genreName == NULL){
		allocError();
		return FALSE;
	}
	safeGenreName=genreName;
	genreName = fgets(genreName, GEN_LINE_MAX_CHARS, genresFile);

	genreName = fgets(genreName, GEN_LINE_MAX_CHARS, genresFile);

	unsigned int nameLen;
	while (genreName != NULL){
		nameLen = strlen(genreName);
		genres->genresName[genres->dim] = malloc(sizeof(char) * nameLen + 1);
		if (genres->genresName[genres->dim] == NULL){
			allocError();
			free(safeGenreName);
			return FALSE;
		}

		strcpy(genres->genresName[genres->dim], genreName);
		genres->genresName[genres->dim][--nameLen] = 0;
		
		genres->nameLengths[genres->dim] = nameLen;
		genres->dim += 1;
		genreName = fgets(genreName, GEN_LINE_MAX_CHARS, genresFile);
	}

	genres->genresName = realloc(genres->genresName, sizeof(char *) * (genres->dim));

	if (genres->genresName == NULL){
		allocError();	
		free(safeGenreName);
		return FALSE;
	}
	
	free(safeGenreName);
	return TRUE;
}

static void allocError(){
	fprintf(stderr, "Error de alocamiento\n");
}

static void freeGenreNames(char ** nameVec,unsigned int dim){
	for (int i = 0 ; i < dim ; i++){
		free(nameVec[i]);		
	}
}

static void freeAllGenres(allGenres * genres){
	if(genres->genresName != NULL){
		freeGenreNames(genres->genresName,genres->dim);
		free(genres->genresName);
	}
	if (genres->nameLengths != NULL)
	{
		free(genres->nameLengths);
	}
	
}
