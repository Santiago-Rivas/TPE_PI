#include "imdb_frontend.h"

static void printGenres(allGenres * genres);
static int readGenresFile(FILE * genresFile, allGenres * genres);
static void allocError();
static void freeAllGenres(allGenres * genres);

int imdb_frontend_main(char * titlePath, char * genresPath, unsigned int yMin, unsigned int yMax){
	FILE * genresFile = fopen(genresPath, "r");
	if (genresFile == NULL){
		fprintf(stderr, "Error: No se logro abrir el archivo de generos\n");
		return EXIT_FAILURE;
	}

	FILE * titleFile = fopen(titlePath, "r");
	if (titleFile == NULL){	
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
	queriesADT queries = newQueries();
	if (queries == NULL){
		allocError();
		return FALSE;
	}

	check = readTitlesFile(titlesFile, queries, &genres, yMin, yMax);
	if (check == FALSE){
		freeQueries(queries);
		return FALSE;
	}
	// Impresion de datos
	printGenres(&genres);

	// Liberar memoria reservada
	freeAllGenres(&genres);
	freeQueries(queries);
	return TRUE;
}

static void printGenres(allGenres * genres){
	for (int i = 0; i < genres->dim ; i++){
		printf("%s\n", genres->genresName[i]);
	}

}

static int readGenresFile(FILE * genresFile, allGenres * genres){
	char * returnGenreName;
	char genreName[GEN_LINE_MAX_CHARS];

	returnGenreName = fgets(genreName, GEN_LINE_MAX_CHARS, genresFile);

	returnGenreName = fgets(genreName, GEN_LINE_MAX_CHARS, genresFile);

	unsigned int nameLen;
	while (returnGenreName != NULL){
		nameLen = strlen(genreName);
		genres->genresName[genres->dim] = malloc(sizeof(char) * nameLen + 1);
		if (genres->genresName[genres->dim] == NULL){
			allocError();
			return FALSE;
		}

		strcpy(genres->genresName[genres->dim], genreName);
		genres->genresName[genres->dim][--nameLen] = 0;

		genres->nameLengths[genres->dim] = nameLen;
		genres->dim += 1;
		returnGenreName = fgets(genreName, GEN_LINE_MAX_CHARS, genresFile);
	}


	if (genres->dim != 0){
		genres->genresName = realloc(genres->genresName, sizeof(char *) * (genres->dim));

		if (genres->genresName == NULL){
			allocError();	
			return FALSE;
		}
	}	
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

static int readTitlesFile(FILE * titlesFile, queriesADT queries, allGenres * genres, unsigned int yMin, unsigned int yMax){

	char titleData[TITLE_LINE_MAX_CHARS];
	char * reutrnTitleData;
	int check;

	returnTitleData = fgets(titleData, TITLE_LINE_MAX_CHARS, titlesFile);

	returnTitleData = fgets(titleData, TITLE_LINE_MAX_CHARS, titlesFile);

	titleADT title = newTitlte();
	if (title == NULL){
		allocError();
		return FALSE;
	}
	while (returnTitleData != NULL){
		check = readTitle(titleData, title);

		check = processData(queries, genres, title, yMin, yMax);

		returnTitleData = fgets(titleData, TITLE_LINE_MAX_CHARS, titlesFile);
	}

	freeTitle(title);
	return TRUE;
}

static int readTitle(char titleData[TITLE_LINE_MAX_CHARS], titleADT title){
	int check;
	unsigned char fieldNum = 0;
	char * field;
	char genresField[CSV_LINE_MAX_CHARS];
	field = strtok(titleData, ";");	
	while (field != NULL){		
		switch (fieldNum){
			case TITLE_TYPE:	
				setTitleType(title, getType(field));
				break;
			case PRIMARY_TITLE:
				check = setTitleName(title, field)
					if(check == FALSE){
						allocError();
						return FALSE;
					}
				break;
			case START_YEAR:		
				if ((!strcmp(field, NO_ELEM_STR1)) || (!strcmp(field, NO_ELEM_STR2))){
					setStartYear(title, NO_YEAR);
				}
				else{
					setStartYear(title, atoi(field));
				}
				break;
			case END_YEAR:		
				if ((!strcmp(field, NO_ELEM_STR1)) || (!strcmp(field, NO_ELEM_STR2))){
					setEndYear(title, NO_YEAR);
				}
				else{
					setEndYear(title, atoi(field));
				}
				break;
			case GENRES:	
				strcpy(genresFiled, field);	
				break;		
			case AVERAGE_RATING:
				setRanking(title, atof(field));
				break;
			case NUM_VOTES:			
				setVotes(title, atoi(field));
				break;
			case ID: case RUNTIME_MINUTES: default:	
				break;
		}
		field++;
		field = strtok(NULL, ";");
	}
	check = getGenres(genresField, title);


}

static int getGenres(char * gernesField, titleADT title){
	char * genreStr;
	genreStr = strtok(genresField, ",");
	while(genreStr != NULL){
		check = addGenre(title, genreStr);
		if (check == FALSE){
			return FALSE;
		}
		genreStr = strtok(NULL, ",");
	}
	return TRUE;
}


static int getType(char * str){
	for (int i = 0 ; str[i] != 0 ; i++){
		str[i] = tolower(str[i]);
	}

	if (strcmp(str, "movie") == 0){
		return MOVIE;			
	}
	else if (strcmp(typeString, "short") == 0){
		return SHORT;
	}
	else if (strcmp(typeString, "tvseries") == 0){
		return TV_SERIES;
	}
	else if (strcmp(typeString, "tvminiseries") == 0){
		return TV_MINI_SERIES;
	}
	else{
		return -1;							//Si la obra no coincide con ningun tipo, se devuelve -1
	}
}


