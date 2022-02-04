#include "imdb_frontend.h"

// Macro para automatizar la impresion del sperador en los archivos csv
#define PRINT_SEPARATOR(FILE) fprintf(FILE, SEPARATOR);
// Macro para automatizar la impresion del "\n" en los archivos csv
#define PRINT_ENTER(FILE) fprintf(FILE, "\n");

// readGenresFile es una funcion que lee el archivo con los generos. La funcion retorna TRUE si todo salio bien y FALSE si hubo algun error de alocamiento.
// Parametros de entrada:
// 	genresFile: Puntero a al archivo de generos.
// Parametros de salida:
// 	genres: Estructura que contiene un arreglo con los nombres de los diferentes generos. Tambien guarda la cantidad de generos que hay en total y la longitud de los strings de generos.
static int readGenresFile(FILE * genresFile, allGenres * genres);

// freeAllGenres es una función que libera la estructura que contiene los generos válidos, que se encontraban en el archivo genres.csv
// Parametros de entrada:
//  genres: Puntero a la estructura que contiene los generos válidos
static void freeAllGenres(allGenres * genres);


static int getGenres(char * genresField, genreList * firstGenre);

// readTitlesFile se encarga de leer el archivo imdb.csv
static int readTitlesFile(FILE * titlesFile, queriesADT queries, allGenres * genres);


static int readTitle(char titleData[TITLE_LINE_MAX_CHARS], titleADT title, genreList * firstGenre);


static int getType(char * str);


static int writeData(queriesADT queries, allGenres * genres);


static void closeFileId(FILE * fileId[TOTAL_QUERY_NUMBER]);


static int printYearlyQueries(FILE * query1, FILE * query3, queriesADT queries, allGenres * genres,titleADT title);


static void printQuery1(FILE * query1, queriesADT queries);


static int printQuery2(FILE * query2, queriesADT queries, allGenres * genres, titleADT title);


static int printQuery3(FILE * query3, queriesADT queries, allGenres * genres, titleADT title);


static int printQuery4(FILE * query4, queriesADT queries, titleADT title);


static int printQuery5(FILE * query5, queriesADT queries, allGenres * genres, titleADT title);


static int printTitle(FILE * query,titleADT title);


static void printStartYear(FILE * query, titleADT title);


static void printEndYear(FILE * query, titleADT title);


static void printVotes(FILE * query, titleADT title);


static void printRating(FILE * query, titleADT title);


static void printGenres(FILE * query, titleADT title, allGenres * genres, int printAnimations);


int imdb_frontend_main(char * titlePath, char * genresPath, unsigned int yMin, unsigned int yMax){
	FILE * genresFile = fopen(genresPath, "r");
	if (genresFile == NULL){
		fprintf(stderr, "Error: No se logro abrir el archivo de generos\n");
		return FILE_ERROR;
	}

	FILE * titlesFile = fopen(titlePath, "r");
	if (titlesFile == NULL){	
		fprintf(stderr, "Error: No se logro abrir el archivo de obras\n");
		fclose(genresFile);
		return FILE_ERROR;
	}

	allGenres genres;

	genres.genresName = calloc(1, sizeof(char *) * MAX_GENRES);
	if (genres.genresName == NULL){
		return ALLOC_ERROR;
	}
	genres.nameLengths = calloc(1, sizeof(unsigned int) * MAX_GENRES);
	if (genres.nameLengths == NULL){
		return ALLOC_ERROR;
	}
	genres.dim = 0;

	int check;
	check = readGenresFile(genresFile, &genres);

	if (check == FALSE){
		freeAllGenres(&genres);
		return ALLOC_ERROR;
	}

	fclose(genresFile);

	// Lectura de obras y llamada al backend
	queriesADT queries = newQueries(yMin, yMax);
	if (queries == NULL){
		fclose(titlesFile);
		return ALLOC_ERROR;
	}

	check = readTitlesFile(titlesFile, queries, &genres);
	if (check == FALSE){
		fclose(titlesFile);
		freeQueries(queries);
		return ALLOC_ERROR;
	}

	fclose(titlesFile);
	// Impresion de datos
	
	check = writeData(queries, &genres);

	// Liberar memoria reservada
	freeAllGenres(&genres);
	freeQueries(queries);
	return check;
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
			return FALSE;
		}
	}	
	return TRUE;
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

static int readTitlesFile(FILE * titlesFile, queriesADT queries, allGenres * genres){

	char titleData[TITLE_LINE_MAX_CHARS];
	char * returnTitleData;
	int check;

	returnTitleData = fgets(titleData, TITLE_LINE_MAX_CHARS, titlesFile);

	returnTitleData = fgets(titleData, TITLE_LINE_MAX_CHARS, titlesFile);

	titleADT title = newTitle();
	if (title == NULL){
		return FALSE;
	}
	while (returnTitleData != NULL){
		genreList titleGenres =NULL;
		check = readTitle(titleData, title,&titleGenres);
		if (check == FALSE){
			freeGenreList(titleGenres);
			freeTitle(title);				
			return FALSE;
		}
		check = processData(queries, title, titleGenres, genres);
		if (check == FALSE){
			freeGenreList(titleGenres);
			freeTitle(title);				
			return FALSE;
		}
		freeGenreList(titleGenres);
		returnTitleData = fgets(titleData, TITLE_LINE_MAX_CHARS, titlesFile);
	}
	freeTitle(title);
	return TRUE;
}

static int readTitle(char titleData[TITLE_LINE_MAX_CHARS], titleADT title, genreList * firstGenre){
	int check;
	unsigned char fieldNum = 0;
	char * field;
	char genresField[TITLE_LINE_MAX_CHARS];
	field = strtok(titleData, SEPARATOR);	
	while (field != NULL){		
		switch (fieldNum){
			case TITLE_TYPE:	
				setTitleType(title, getType(field));
				break;
			case PRIMARY_TITLE:
				check = setTitleName(title, field);
					if(check == FALSE){
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
				strcpy(genresField, field);	
				break;		
			case AVERAGE_RATING:
				setRating(title, atof(field));
				break;
			case NUM_VOTES:			
				setVotes(title, atoi(field));
				break;
			case ID: case RUNTIME_MINUTES: default:	
				break;
		}
		fieldNum++;
		field = strtok(NULL, SEPARATOR);
	}
	setFalseAnimation(title);
	check = getGenres(genresField,firstGenre);

	return check;
	

}

static int getGenres(char * genresField, genreList * firstGenre){
	char * genreStr;
	int check= TRUE;
	genreStr = strtok(genresField, ",");
	while(genreStr != NULL){
		(*firstGenre) = addGenres(*firstGenre, genreStr, &check);
		if (check == FALSE){
			return FALSE;
		}
		genreStr = strtok(NULL, ",");
	}
	return TRUE;
}

static int getType(char * str){
	
	if (stringCompare(str, "movie") == 0){
		return MOVIE;			
	}
	else if (stringCompare(str, "short") == 0){
		return SHORT;
	}
	else if (stringCompare(str, "tvseries") == 0){
		return TV_SERIES;
	}
	else if (stringCompare(str, "tvminiseries") == 0){
		return TV_MINI_SERIES;
	}
	else{
		return NO_VALID_TYPE;							//Si la obra no coincide con ningun tipo, se devuelve -1
	}
}

static int writeData(queriesADT queries, allGenres * genres){
	FILE * fileId[TOTAL_QUERY_NUMBER] = {NULL};
	titleADT title = newTitle();
	if (title==NULL)
	{
		return ALLOC_ERROR;
	}
	
	char * returnFileNames[TOTAL_QUERY_NUMBER] = RETURN_FILE_NAMES;
	char * returnFileHeaders[TOTAL_QUERY_NUMBER] = RETURN_FILE_HEADERS;
	int fileCheck = EXIT_SUCCESS;

	for (int i = 0 ; i < TOTAL_QUERY_NUMBER ; i++){
		fileId[i] = fopen(returnFileNames[i], "wt");
		if(fileId[i] != NULL){
			fprintf(fileId[i],"%s\n",returnFileHeaders[i]);
		}
		else{
			fileCheck = FILE_ERROR;
		}
	}
	
	int check = TRUE;
	
	check = printYearlyQueries(fileId[Q1], fileId[Q3], queries, genres,title);
	if (check == FALSE)
	{
		closeFileId(fileId);
		return ALLOC_ERROR;
	}
	check = printQuery2(fileId[Q2] ,queries, genres,title);
	if (check == FALSE)
	{
		closeFileId(fileId);
		return ALLOC_ERROR;
	}
	check = printQuery4(fileId[Q4], queries,title);
	if (check == FALSE)
	{
		closeFileId(fileId);
		return ALLOC_ERROR;
	}
	check = printQuery5(fileId[Q5], queries, genres, title);
	if (check == FALSE)
	{
		closeFileId(fileId);
		return ALLOC_ERROR;
	}

	freeTitle(title);
	closeFileId(fileId);
	return fileCheck;
}

static void closeFileId(FILE * fileId[TOTAL_QUERY_NUMBER]){
	for (int i = 0 ; i < TOTAL_QUERY_NUMBER ; i++){
		if (fileId[i] != NULL){
			fclose(fileId[i]);
		}
	}
}

static int printYearlyQueries(FILE * query1, FILE * query3, queriesADT queries, allGenres * genres,titleADT title)
{
	if(query1==NULL && query3==NULL)
	{
		return TRUE;
	}
	toBeginYears(queries);
	int hasYear=hasNextYear(queries);
	int check;
	while(hasYear){
		if(query1 != NULL)
		{
			printQuery1(query1, queries);
		}
		if(query3 != NULL)
		{
			check=printQuery3(query3, queries, genres, title);
			if(check == FALSE){
				return FALSE;
			}
		}
		hasYear=nextYear(queries);
	}

	return TRUE;
}

static void printQuery1(FILE * query1, queriesADT queries)
{
	unsigned int year,nFilms,nSeries,nShorts;
	returnCurrentYearQ1(queries,&year,&nFilms,&nSeries,&nShorts);
	fprintf(query1,"%d;%d;%d;%d\n",year,nFilms,nSeries,nShorts);
}

static int printQuery2(FILE * query2, queriesADT queries, allGenres * genres, titleADT title)
{
	toBeginTopAnimatedFilms(queries);
	int errorFlag=1;
	int hasNext=hasNextTopAnimatedFilms(queries);
	while(hasNext)
	{
		nextTopAnimatedFilms(queries,title,&errorFlag);
		if(errorFlag == FALSE)
		{
			return FALSE;
		}
		printStartYear(query2, title);
		PRINT_SEPARATOR(query2)
		errorFlag = printTitle(query2,title);
		if(errorFlag == FALSE)
		{
			return FALSE;
		}
		PRINT_SEPARATOR(query2)
		printVotes(query2, title);
		PRINT_SEPARATOR(query2)
		printRating(query2, title);
		PRINT_SEPARATOR(query2)
		printGenres(query2, title, genres, FALSE);
		PRINT_ENTER(query2)	
		hasNext = hasNextTopAnimatedFilms(queries);
	}
	return TRUE;
}

static int printQuery3(FILE * query3, queriesADT queries, allGenres * genres, titleADT title)
{
	toBeginYearRankings(queries);
	int errorFlag=1;
	int hasNext=hasNextYearRanking(queries);
	while(hasNext)
	{
		nextYearRanking(queries,title,&errorFlag);
		if(errorFlag == FALSE)
		{
			return FALSE;
		}
		printStartYear(query3, title);
		PRINT_SEPARATOR(query3)
		errorFlag = printTitle(query3,title);
		if(errorFlag == FALSE)
		{
			return FALSE;
		}
		PRINT_SEPARATOR(query3)
		printVotes(query3, title);
		PRINT_SEPARATOR(query3)
		printRating(query3, title);
		PRINT_SEPARATOR(query3)
		printGenres(query3, title, genres, TRUE);
		PRINT_ENTER(query3)
		hasNext = hasNextYearRanking(queries);
	}
	return TRUE;
}

static int printQuery4(FILE * query4, queriesADT queries, titleADT title)
{
	toBeginTopSeries(queries);
	int errorFlag=1;
	int hasNext=hasNextTopSeries(queries);
	while(hasNext)
	{
		hasNext=nextTopSeries(queries,title,&errorFlag);
		if(errorFlag == FALSE)
		{
			return FALSE;
		}
		printStartYear(query4, title);
		PRINT_SEPARATOR(query4)
		printEndYear(query4, title);
		PRINT_SEPARATOR(query4)
		errorFlag = printTitle(query4,title);
		if(errorFlag == FALSE)
		{
			return FALSE;
		}
		PRINT_SEPARATOR(query4)
		printVotes(query4, title);
		PRINT_SEPARATOR(query4)
		printRating(query4, title);
		PRINT_ENTER(query4)
		hasNext = hasNextTopSeries(queries);
	}
	return TRUE;
}

static int printQuery5(FILE * query5, queriesADT queries,allGenres * genres, titleADT title) 
{
	toBeginWorstSeries(queries);
	int errorFlag=1;
	int hasNext=hasNextWorstSeries(queries);
	while(hasNext)
	{
		hasNext=nextWorstSeries(queries,title,&errorFlag);
		if(errorFlag == FALSE)
		{
			return FALSE;
		}
		printStartYear(query5, title);
		PRINT_SEPARATOR(query5)
		printEndYear(query5, title);
		PRINT_SEPARATOR(query5)
		errorFlag = printTitle(query5, title);
		if(errorFlag == FALSE)
		{
			return FALSE;
		}
		PRINT_SEPARATOR(query5)
		printVotes(query5, title);
		PRINT_SEPARATOR(query5)
		printRating(query5, title);
		PRINT_SEPARATOR(query5);
		printGenres(query5, title, genres, TRUE);
		PRINT_ENTER(query5)
		hasNext = hasNextWorstSeries(queries);
	}
	return TRUE;
}

static int printTitle(FILE * query,titleADT title)
{
	char * str = NULL;
	int check;
	check = returnTitleName(title,&str);
	if(check == FALSE)
	{
		return FALSE;
	}
	fprintf(query,"%s",str);
	free(str);
	return TRUE;
}

static void printStartYear(FILE * query, titleADT title){
	fprintf(query, "%d", returnStartYear(title));
}

static void printEndYear(FILE * query, titleADT title){
	int year = returnEndYear(title);
	if (year <= NO_YEAR){
		fprintf(query, NO_ELEM_STR2);
	} else{
		fprintf(query, "%d", year);
	}
}

static void printVotes(FILE * query, titleADT title){
	fprintf(query, "%d", returnVotes(title));
}

static void printRating(FILE * query, titleADT title){
	fprintf(query, "%0.1f", returnRating(title));
}

static void printGenres(FILE * query, titleADT title, allGenres * genres, int printAnimations){
	int dim = returnGenCount(title);
	int i;
	int addCounter = 0;
	int genreNum;
	for (i = 0 ; i < dim ; i++){
		genreNum = returnGenre(title, i);
		if (!((printAnimations == FALSE) && (stringCompare(genres->genresName[genreNum], Q2_GENRE_NAME) == 0))){
			if (i != 0){
				fprintf(query, ",");
			}
			
			fprintf(query, "%s", genres->genresName[genreNum]);
			addCounter++;
		}
	}
	if (addCounter == 0){	
		fprintf(query, NO_ELEM_STR2);
	}
}
