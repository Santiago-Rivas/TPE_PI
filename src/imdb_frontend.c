#include <imdb_frontend.h>


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

	gernes.genresName = calloc(1, sizeof(char *) * MAX_GENRES);
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
		return FALSE;
	}

	// Lectura de obras y llamada al backend
	
	// Impresion de datos
	
	free(genres);
	
	
	// Liberar memoria reservada
	freeAllGenres(&genres);

}

static void printGenres(allGenres * genres){
	for (int i = 0; i < genres->dim ; i++){
		printf("%s\n", genres->genresName[i]);
	}

}

static int readGenresFile(FILE * genresFile, allGenres * genres){
	char * genreName = malloc((GEN_LINE_MAX_CHARS + 1) * sizeof(char));
	if (genreName == NULL){
		allocError();
		return FALSE;
	}
	genreName = fgets(genreName, GEN_LINE_MAX_CHARS, genresFile);

	genreName = fgets(genreName, GEN_LINE_MAX_CHARS, genresFile);

	unsigned int nameLen;
	while (genreName != NULL){
		nameLen = strlen(genreName);
		genres->genresName[genres->dim] = malloc(sizeof(char) * nameLen + 1);
		if (genres->genresName[genres->dim] == NULL){
			freeAllGenres(genres);
			allocError();
			return FALSE;
		}

		strcpy(genres->genresName[genres->dim], genreName);
		genres->genresName[genres->dim][--nameLen] = 0;
		
		genres->nameLengths[genres->dim] = nameLen;
		genres->dim += 1;
		genreName = fgets(genreName, GEN_LINE_MAX_CHARS, genresFile);
		}

	char ** genreNameSafe;
	genreNameSafe = genres->genresName;
	genres->genresName = realloc(genres->genresName, sizeof(char *) * (genres->dim));


	if (genres->genresName == NULL){
		freeAllGenres(genres);
		allocError();	
		return FALSE;
	}
	free(genreName);

	return TRUE;
}



static void allocError(){
	fprintf(stderr, "Error de alocamiento");
}



static freeAllGenres(allGenres * genres){
	for (int i = 0 ; i < genres->dim ; i++){
		free(genres->genresName[i]);		
		free(genres->nameLengths[i]);
	}
}
