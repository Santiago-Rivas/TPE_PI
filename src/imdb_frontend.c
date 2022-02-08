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

// getGenres se encarga de obtener los géneros asociados a cada obra y armar una lista con ellos
// Parametros de Entrada:
// genresField: es un vector donde se guardan todos los géneros juntos
// firstGenre: es un puntero a la lista donde se guardarán los géneros
// Parametros de Salida:
// Retorna 1 si no hubo errores, 0 si hubo algún error
static int getGenres(char * genresField, genreList * firstGenre);

// readTitlesFile se encarga de leer el archivo imdb.csv
// Parametros de entrada:
// titlesFile: es un puntero al archivo de obras (imdb.csv)
// queries: es un puntero a la estructura donde se almacena lo necesario para cada query
// genres: es la estructura con la lista de generos válidos
// Parametros de Salida:
// Retorna 1 si no hubo errores, 0 si hubo algún error
static int readTitlesFile(FILE * titlesFile, queriesADT queries, allGenres * genres);

// readTitle se encarga de leer el archivo imdb.csv
// Parametros de entrada:
// titleData: vector donde se guarda cada línea del archivo
// title: puntero a la estructura en donde se guardan los datos de cada obra
// firstGenre: puntero a la lista ed generos de una obra
// Parametros de Salida:
// Retorna 1 si no hubo errores, 0 si hubo algún error
static int readTitle(char titleData[TITLE_LINE_MAX_CHARS], titleADT title, genreList * firstGenre);

// getType obtiene el tipo de obra
// Parametros de Entrada:
// str: string con el tipo de obra
// Parametros de Salida:
// Retorna un entero que hace referencia a algun tipo de obra según un enum
static int getType(char * str);

// writeData se encarga de crear los archivos de respuesta y llenarlos con los datos de cada query
// Parametros de Entrada:
// queries: puntero a la estructura donde se almacena lo necesario para cada query
// genres: puntero a la estructura con la lista de generos válidos
// Parametros de Salida:
// Retorna un entero según el tipo de error, o EXIT_SUCCESS si no hubo errores
static int writeData(queriesADT queries, allGenres * genres);

// closeFileId se encarga de cerrar los archivos de respuesta
// Parametros de Entrada:
// fileID: es un vector de punteros a los archivos de respuesta
static void closeFileId(FILE * fileId[TOTAL_QUERY_NUMBER]);

// printYearlyQueries se encarga de imprimir aquellos archivos de respuesta que son iterados por año (query 1 y query 3)
// Parametros de Entrada:
// query1: puntero al archivo de respuesta del query1
// query3: puntero al archivo de respuesta del query3
// queries: puntero a la estructura donde se almacena lo necesario para cada query
// genres: puntero a la estructura con la lista de generos válidos
// title: puntero a la estructura en donde se guardan los datos de cada obra
static int printYearlyQueries(FILE * query1, FILE * query3, queriesADT queries, allGenres * genres,titleADT title);

// printQuery1 se encarga de imprimir en el archivo de respuesta los datos de la query1
// Parametros de Entrada:
// query1: puntero al archivo de respuesta del query1
// queries: puntero a la estructura donde se almacena lo necesario para cada query
static void printQuery1(FILE * query1, queriesADT queries);

// printQuery2 se encarga de imprimir en el archivo de respuesta los datos de la query2
// Parametros de Entrada:
// query2: puntero al archivo de respuesta del query2
// queries: puntero a la estructura donde se almacena lo necesario para cada query
static int printQuery2(FILE * query2, queriesADT queries, allGenres * genres, titleADT title);

// printQuery3 se encarga de imprimir en el archivo de respuesta los datos de la query3
// Parametros de Entrada:
// query3: puntero al archivo de respuesta del query3
// queries: puntero a la estructura donde se almacena lo necesario para cada query
static int printQuery3(FILE * query3, queriesADT queries, allGenres * genres, titleADT title);

// printQuery4 se encarga de imprimir en el archivo de respuesta los datos de la query4
// Parametros de Entrada:
// query4: puntero al archivo de respuesta del query4
// queries: puntero a la estructura donde se almacena lo necesario para cada query
static int printQuery4(FILE * query4, queriesADT queries, titleADT title);

// printQuery5 se encarga de imprimir en el archivo de respuesta los datos de la query5
// Parametros de Entrada:
// query5: puntero al archivo de respuesta del query5
// queries: puntero a la estructura donde se almacena lo necesario para cada query
static int printQuery5(FILE * query5, queriesADT queries, allGenres * genres, titleADT title);

// printQueries se encarga de imprimir en los archivos de respuesta, los resultados de las queries 2 a 5
// Parametros de entrada:
// query: puntero al archivo de respuesta
// queries: puntero a la estructura donde se almacena lo necesario para cada query
// genres: puntero a la estructura con el arreglo de generos válidos. Si no se deben imprimir los generos en algún query, el puntero vale NULL
// title: puntero a la estructura en donde se guardan los datos de cada obra
// isSeries: vale TRUE si se trata de series (como el caso de las queries 4 y 5), o FALSE si se trata de peliculas (como el caso de las queries 2 y 3)
// showAnimation: vale TRUE si se debe mostrar el genero "Animation", o FALSE si debe omitirse (como es el caso de la query 2)
// hasNextFunction: es un puntero a función, dependiendo de la query a mostrar se le pasa la funcion correspondiente
// nextFunction: es un puntero a función, dependiendo de la query a mostrar se le pasa la funcion correspondiente
// Parametros de Salida:
// Retorna un TRUE si no hubo errores, FALSE si se produjo algún error de alocamiento
static int printQueries(FILE * query, queriesADT queries,allGenres * genres, titleADT title, int isSeries, int showAnimation, int (*hasNextFunction) (queriesADT queries), int (*nextFunction)(queriesADT queries,titleADT title, int *flag));

// printTitle se encarga de imprimir en los archivos de respuesta el titulo de la obra
// Parametros de Entrada:
// query: puntero al archivo de respuesta
// title: puntero a la estructura en donde se guardan los datos de cada obra
static int printTitle(FILE * query,titleADT title);

// printStartYear se encarga de imprimir en los archivos de respuesta el año de comienzo de la obra
// Parametros de Entrada:
// query: puntero al archivo de respuesta
// title: puntero a la estructura en donde se guardan los datos de cada obra
static void printStartYear(FILE * query, titleADT title);

// printEndYear se encarga de imprimir en los archivos de respuesta el año de finalización de la obra
// Parametros de Entrada:
// query: puntero al archivo de respuesta
// title: puntero a la estructura en donde se guardan los datos de cada obra
static void printEndYear(FILE * query, titleADT title);

// printVotes se encarga de imprimir en los archivos de respuesta la cantidad de votos de la obra
// Parametros de Entrada:
// query: puntero al archivo de respuesta
// title: puntero a la estructura en donde se guardan los datos de cada obra
static void printVotes(FILE * query, titleADT title);

// printRating se encarga de imprimir en los archivos de respuesta el rating de la obra
// Parametros de Entrada:
// query: puntero al archivo de respuesta
// title: puntero a la estructura en donde se guardan los datos de cada obra
static void printRating(FILE * query, titleADT title);

// printGenres se encarga de imprimir los generos asociados a cada obra
// Parametros de Entrada:
// query: puntero al archivo de respuesta
// title: puntero a la estructura en donde se guardan los datos de cada obra
// genres: puntero a la estructura con la lista de generos válidos
// printAnimations: entero que se encarga de controlar si la pelicula es o no animada
static void printGenres(FILE * query, titleADT title, allGenres * genres, int printAnimations);


///////////////////////////////////////////////////// Funciones /////////////////////////////////////////////////////


int imdb_frontend_main(char * titlePath, char * genresPath, unsigned int yMin, unsigned int yMax){
	FILE * genresFile = fopen(genresPath, "r");												// Se abre el archivo de generos
	if (genresFile == NULL){																// Si no se pudo abrir
		return GENRES_FILE_ERROR;															// Retorna que hubo un error en los archivos
	}

	FILE * titlesFile = fopen(titlePath, "r");												// Se abre el archivo de obras
	if (titlesFile == NULL){																// Si no se pudo abrir
		fclose(genresFile);																	// Se cierra el archivo de generos que quedó abierto
		return TITLE_FILE_ERROR;															// Retorna que hubo un error en los archivos
	}

	allGenres genres;																		// Se crea una estructura para guardar los géneros válidos

	genres.genresName = calloc(1, sizeof(char *) * MAX_GENRES);								// Se crea espacio para guardar la cantidad máxima de géneros
	if (genres.genresName == NULL){															// Si no se pudo reservar memoria
		return ALLOC_ERROR;																	// Retorna que hubo un error de alocamiento de memoria
	}
	genres.nameLengths = calloc(1, sizeof(unsigned int) * MAX_GENRES);						// Se crea espacio para guardar la longitud de cada género
	if (genres.nameLengths == NULL){														// Si no se pudo reservar memoria
		return ALLOC_ERROR;																	// Retorna que hubo un error de alocamiento de memoria
	}
	genres.dim = 0;																			// Inicializa la dimensión de los vectores en 0

	int check;
	check = readGenresFile(genresFile, &genres);											// Lee el archivo de generos

	if (check == FALSE){																	// Si hubo un error
		freeAllGenres(&genres);																// Libera la estructura que utilizó para guardar los generos
		return ALLOC_ERROR;																	// Retorna que hubo un error de alocamiento de memoria
	}

	fclose(genresFile);																		// Cierra el archivo de géneros

	// Lectura de obras y llamada al backend
	queriesADT queries = newQueries(yMin, yMax);											// Crea una estructura donde se almacena todo lo necesario para cada query
	if (queries == NULL){																	// Si hubo un error
		fclose(titlesFile);																	// Cierra el archivo de obras
		return ALLOC_ERROR;																	// Retorna que hubo un error de alocamiento de memoria
	}

	check = readTitlesFile(titlesFile, queries, &genres);									// Lee el archivo de obras
	if (check == FALSE){																	// Si hubo un error
		fclose(titlesFile);																	// Cierra el archivo de obras
		freeAllGenres(&genres);																// Libera la estructura utilizada para guardar los géneros válidos
		freeQueries(queries);																// Libera la estructura donde se almacena todo lo necesario para cada query
		return ALLOC_ERROR;																	// Retorna que hubo un error de alocamiento de memoria
	}

	fclose(titlesFile);																		// Cierra el archivo de obras
	// Impresion de datos
	
	check = writeData(queries, &genres);													// Crea e imprime en los archivos de respuestas, los resultados de cada query

	// Liberar memoria reservada
	freeAllGenres(&genres);																	// Libera la estructura utilizada para guardar los géneros válidos
	freeQueries(queries);																	// Libera la estructura donde se almacena todo lo necesario para cada query
	return check;																			// Retorna si hubo un erro o no
}

static int readGenresFile(FILE * genresFile, allGenres * genres){
	char * returnGenreName;
	char genreName[GEN_LINE_MAX_CHARS];														//Se crea un vector para guardar cada línea del archivo de generos

	returnGenreName = fgets(genreName, GEN_LINE_MAX_CHARS, genresFile);						// Se lee el encabezado del archivo	

	returnGenreName = fgets(genreName, GEN_LINE_MAX_CHARS, genresFile);						// Se lee la 1° línea del archivo

	unsigned int nameLen;																	// Indica la longitud del género
	while (returnGenreName != NULL && genres->dim < MAX_GENRES){														// Si el archivo no esta vacío
		nameLen = strlen(genreName);														// Se guarda la longitud del género
		genres->genresName[genres->dim] = malloc(sizeof(char) * nameLen + 1);				// Se reserva espacio para guardar el género
		if (genres->genresName[genres->dim] == NULL){										// Si no de pudo reservar memoria
			return FALSE;																	// Retorna 0
		}

		strcpy(genres->genresName[genres->dim], genreName);									// Se guarda el género en la estructura
		genres->genresName[genres->dim][--nameLen] = 0;										// Se coloca el 0 final (esto se hace para que no quede guardado con el "\n" y así evitar problemas a la hora de compararlos con los géneros de cada obra)

		genres->nameLengths[genres->dim] = nameLen;											// Se guarda la longitud de ese género


		genres->dim += 1;																	// Se incrementa en 1 la dimensión de los vectores
		returnGenreName = fgets(genreName, GEN_LINE_MAX_CHARS, genresFile);					// Se lee la siguiente línea del archivo
	}


	if (genres->dim != 0){																	// Si la dimensión no es 0
		genres->genresName = realloc(genres->genresName, sizeof(char *) * (genres->dim));	// Se agranda el vector

		if (genres->genresName == NULL){													// Si hubo un error
			return FALSE;																	// Retorna 0
		}
	}	
	return TRUE;																			// Si no hubo error retorna 1
}

static void freeGenreNames(char ** nameVec,unsigned int dim){
	for (int i = 0 ; i < dim ; i++){       													// Va desde 0 hasta la cantidad de generos de la obra
		free(nameVec[i]);																	// Libera cada string que contiene el nombre de cada genero
	}
}

static void freeAllGenres(allGenres * genres){
	if(genres->genresName != NULL){															// Si el vector de generos no esta vacío
		freeGenreNames(genres->genresName,genres->dim);										// Libera todos los strings con los nombres de cada género que tenía la obra
		free(genres->genresName);															// Libera el vector
	}								
	if (genres->nameLengths != NULL)														// Si el vector que guarda la longitud de cada género no esta vacío					
	{								
		free(genres->nameLengths);															// Se libera el vector
	}

}

static int readTitlesFile(FILE * titlesFile, queriesADT queries, allGenres * genres){

	char titleData[TITLE_LINE_MAX_CHARS];													// Se crea un vector para guardar una línea del archivo
	char * returnTitleData;
	int check;

	returnTitleData = fgets(titleData, TITLE_LINE_MAX_CHARS, titlesFile);					// Se lee el encabezado del archivo

	returnTitleData = fgets(titleData, TITLE_LINE_MAX_CHARS, titlesFile);					// Se lee la 1° línea con los datos de la obra

	titleADT title = newTitle();															// Se crea una estructura para guardar los datos de la obra
	if (title == NULL){																		// Si hubo un error
		return FALSE;																		// Retorna 0
	}
	while (returnTitleData != NULL){														// Mientras haya líneas para leer
		genreList titleGenres =NULL;														// Se cre una lista para guardar los géneros de cada obra
		check = readTitle(titleData, title,&titleGenres);									// Lee una línea del archivo y pone en la estructura todos los datos de la obra
		if (check == FALSE){																// Si hubo un error
			freeGenreList(titleGenres);														// Libera la lista de generos
			freeTitle(title);																// Libera la estructura donde se guardan los datos de la obra
			return FALSE;																	// Retorna 0
		}
		check = processData(queries, title, titleGenres, genres);							// Procesa la línea leída
		if (check == FALSE){																// Si hubo un error
			freeGenreList(titleGenres);														// Libera la lista de géneros
			freeTitle(title);																// Libera la estructura donde se guardan los datos de la obra
			return FALSE;																	// Retorna 0
		}
		freeGenreList(titleGenres);															// Libera la estructura donde se guardan los datos de la obra
		returnTitleData = fgets(titleData, TITLE_LINE_MAX_CHARS, titlesFile);				// Lee la siguiente línea del archivo
	}
	freeTitle(title);																		// Cuando terminó de leer el archivo, libera la estructura que utilizó para ir guardando los datos de cada obra, e ir procesando cada linea
	return TRUE;																			// Retorna 1
}

static int readTitle(char titleData[TITLE_LINE_MAX_CHARS], titleADT title, genreList * firstGenre){
	int check;
	unsigned char fieldNum = 0;																// Indica el número de campo de la estructura que contiene los datos de la obra
	char * field;																			// String donde se guarda cada campo de la estructura
	char genresField[TITLE_LINE_MAX_CHARS];													// Vector para guardar los géneros de la obra
	field = strtok(titleData, SEPARATOR);													// Se obtiene el primer campo
	while (field != NULL){																	// Si no estaba vacío
		switch (fieldNum){
			case TITLE_TYPE:																// Si es el  tipo de obra
				setTitleType(title, getType(field));										// Se agrega el tipo de obra a la estructura
				break;
			case PRIMARY_TITLE:																// Si es el titulo
				check = setTitleName(title, field);											// Se agrega el titulo a la estructura
					if(check == FALSE){														// Si hubo un error
						return FALSE;														// Retorna 0
					}
				break;
			case START_YEAR:																// Si es el año de inicio de la obra
				if ((!strcmp(field, NO_ELEM_STR1)) || (!strcmp(field, NO_ELEM_STR2))){		// En caso de que el campo no sea un año, si no que hay un indicador de que no hay año
					setStartYear(title, NO_YEAR);											// En el campo del año de la estructura se coloca un 0, indicando que no hay año de inicio
				}
				else{
					setStartYear(title, atoi(field));										// Si era un año, se agrega a la estructura
				}
				break;
			case END_YEAR:																	// Si es el año de inicio de la obra
				if ((!strcmp(field, NO_ELEM_STR1)) || (!strcmp(field, NO_ELEM_STR2))){		// En caso de que el campo no sea un año, si no que hay un indicador de que no hay año
					setEndYear(title, NO_YEAR);												// En el campo del año de la estructura se coloca un 0, indicando que no hay año de finalización
				}
				else{
					setEndYear(title, atoi(field));											// Si era un año, se agrega a la estructura
				}
				break;
			case GENRES:																	// Si se trata de los géneros de la obra
				strcpy(genresField, field);													// Se copian todos juntos al string genresField
				break;		
			case AVERAGE_RATING:															// Si es la puntuación
				setRating(title, atof(field));												// Se agrega la puntuación a la estructura
				break;
			case NUM_VOTES:																	// Si es la cantidad de votos
				setVotes(title, atoi(field));												// Se agrega la cantidad de votos a la estructura
				break;
			case ID: case RUNTIME_MINUTES: default:											// Si se trata del id o de la duración de la obra no se hace nada pues para este caso son irrelevantes dichos datos
				break;
		}
		fieldNum++;																			// Se incrementa el numero de campo a analizar
		field = strtok(NULL, SEPARATOR);													// Se obtiene el siguiente campo
	}
	setFalseAnimation(title);																// Cambia un valor en la estructura para no imprimir el genero animación
	check = getGenres(genresField,firstGenre);												// Se obtienen los generos de la obra y arma una lista con ellos

	return check;																			// Retorna check que indica si hubo un error
	

}

static int getGenres(char * genresField, genreList * firstGenre){
	char * genreStr;																		// Se crea un string, donde se guarda cada género de la obra											
	int check= TRUE;					
	genreStr = strtok(genresField, ",");													// Se obtiene un genero de la obra
	while(genreStr != NULL){																// Mientras haya un género
		(*firstGenre) = addGenres(*firstGenre, genreStr, &check);							// Se agrega a la lista de generos de la obra
		if (check == FALSE){																// Si hubo un error
			return FALSE;																	// Retorna 0
		}					
		genreStr = strtok(NULL, ",");														// Se obtiene el próximo género
	}					
	return TRUE;																			// Si no hubo errores, devuelve 1
}

static int getType(char * str){
	
	if (stringCompare(str, "movie") == 0){													// Compara el tipo de obra para ver si es un película
		return MOVIE;																		// Retorna un valor que indica que es una película
	}								
	else if (stringCompare(str, "short") == 0){												// Compara el tipo de obra para ver si es un corto
		return SHORT;																		// Retorna un valor que indica que es una corto
	}								
	else if (stringCompare(str, "tvseries") == 0){											// Compara el tipo de obra para ver si es un serie
		return TV_SERIES;																	// Retorna un valor que indica que es una serie
	}								
	else if (stringCompare(str, "tvminiseries") == 0){										// Compara el tipo de obra para ver si es un mini serie
		return TV_MINI_SERIES;																// Retorna un valor que indica que es una mini serie
	}								
	else{								
		return NO_VALID_TYPE;																//Si la obra no coincide con ningun tipo, se devuelve -1
	}
}

static int writeData(queriesADT queries, allGenres * genres){
	FILE * fileId[TOTAL_QUERY_NUMBER] = {NULL};												// Se inicializa el vector de archivos de respuesta con todos los archivos en NULL
	titleADT title = newTitle();															// Se crea una estructura, donde se almacenan los datos de la obra
	if (title==NULL)																		// Si no se pudo crear
	{				
		return ALLOC_ERROR;																	// Retorna que hubo un error de alocamiento de memoria
	}
	
	char * returnFileNames[TOTAL_QUERY_NUMBER] = RETURN_FILE_NAMES;
	char * returnFileHeaders[TOTAL_QUERY_NUMBER] = RETURN_FILE_HEADERS;
	int fileCheck = EXIT_SUCCESS;

	for (int i = 0 ; i < TOTAL_QUERY_NUMBER ; i++){											// Va desde 0 hasta la cantidad de archivos de respuesta
		fileId[i] = fopen(returnFileNames[i], "wt");										// Se crean los archivos de respuesta
		if(fileId[i] != NULL){																// Si se pudo crear
			fprintf(fileId[i],"%s\n",returnFileHeaders[i]);									// Se imprimie el encabezado correspondiente
		}
		else{
			fileCheck = RETURN_FILE_ERROR;													// Si no se pudo crear, en fileCheck se guarda que hubo un error de alocamiento
		}
	}
	
	int check = TRUE;
	
	check = printYearlyQueries(fileId[Q1], fileId[Q3], queries, genres,title);  			// Se imprimien en los archivos de respuesta, las queries que iteran por año juntas, para no recorres 2 veces la lista de años
	if (check == FALSE)																		// Si hubo un error
	{			
		closeFileId(fileId);																// Se cierran los archivos abiertos
		return ALLOC_ERROR;																	// Se retorna que hubo un error de alocamiento
	}			
	check = printQuery2(fileId[Q2] ,queries, genres,title);									// Se imprime en el archivo de respuesta el resultado del query 2
	if (check == FALSE)																		// Si hubo un error
	{			
		closeFileId(fileId);																// Se cierran los archivos abiertos
		return ALLOC_ERROR;																	// Se retorna que hubo un error de alocamiento
	}			
	check = printQuery4(fileId[Q4], queries,title);											// Se imprime en el archivo de respuesta el resultado del query 4
	if (check == FALSE)																		// Si hubo un error
	{			
		closeFileId(fileId);																// Se cierran los archivos abiertos
		return ALLOC_ERROR;																	// Se retorna que hubo un error de alocamiento
	}			
	check = printQuery5(fileId[Q5], queries, genres, title);								// Se imprime en el archivo de respuesta el resultado del query 5
	if (check == FALSE)																		// Si hubo un error
	{			
		closeFileId(fileId);																// Se cierran los archivos abiertos
		return ALLOC_ERROR;																	// Se retorna que hubo un error de alocamiento
	}			

	freeTitle(title);																		// Se libera la estructura utilizada para guardar los datos de cada obra
	closeFileId(fileId);																	// Se cierran los archivos que quedaron abiertos
	return fileCheck;																		// Retorna filecheck, que indica su hubo algún error o no	
}

static void closeFileId(FILE * fileId[TOTAL_QUERY_NUMBER]){
	for (int i = 0 ; i < TOTAL_QUERY_NUMBER ; i++){   										// Va desde 0 hasta la cantidad de archivos de respuesta
		if (fileId[i] != NULL){																// Si el archivo de respuesta se pudo crear
			fclose(fileId[i]);																// Se cierra el archivo
		}
	}
}

static int printYearlyQueries(FILE * query1, FILE * query3, queriesADT queries, allGenres * genres,titleADT title)
{
	if(query1==NULL && query3==NULL)     													// Se verifica si los 2 archivos de respuesta que iteran por año se pudieron crear
	{						
		return TRUE;																		// Si ninguno se pudo crear, se retorna 1 y esos dos archivos de respuestas quedan vacios
	}						
	toBeginYears(queries);																	//Se inicializa el iterador por años
	int hasYear=hasNextYear(queries);														// Se verifica si hay un elemento siguiente
	int check;						
	while(hasYear){																			// Mientras haya un elemento siguiente
		if(query1 != NULL)																	// Si el archivo de respuesta del query 1 se pudo crear
		{						
			printQuery1(query1, queries);													// Se imprimie en el archivo de respuesta los resultados del query 1
		}						
		if(query3 != NULL)																	// Si el archivo de respuesta del query 1 se pudo crear
		{						
			check=printQuery3(query3, queries, genres, title);      						// Se imprimie en el archivo de respuesta los resultados del query 3
			if(check == FALSE){																// Si check vale 0, entonces hubo un error
				return FALSE;																// Si hubo un error, retorna 0
			}						
		}						
		hasYear=nextYear(queries);															// Se verifica que haya un elemento siguiente
	}						
	return TRUE;																			// Si no hubo errores, retorna 1
}

static void printQuery1(FILE * query1, queriesADT queries)
{
	unsigned int year,nFilms,nSeries,nShorts;
	returnCurrentYearQ1(queries,&year,&nFilms,&nSeries,&nShorts);							//En year,nFilms,nSeries,nShorts se ponen el año, la cantidad de peliculas, de series y de cortos respectivamente
	fprintf(query1,"%d;%d;%d;%d\n",year,nFilms,nSeries,nShorts);							// Se imprime el resultado de la query en el archivo de respuesta
}

static int printQuery2(FILE * query2, queriesADT queries, allGenres * genres, titleADT title)
{
	toBeginTopAnimatedFilms(queries);																					// Se inicializa el iterador
	int check = printQueries(query2,queries,genres,title,FALSE,FALSE,hasNextTopAnimatedFilms,nextTopAnimatedFilms);		// Se imprime el resultado de la query en el archivo de respuesta
	return check;																										// check vale 1 si no hubo errores, o 0 si existió algún error
}

static int printQuery3(FILE * query3, queriesADT queries, allGenres * genres, titleADT title)
{
	toBeginYearRankings(queries);																			// Se inicializa el iterador
	int check = printQueries(query3,queries,genres,title,FALSE,TRUE,hasNextYearRanking,nextYearRanking);	// Se imprime el resultado de la query en el archivo de respuesta
	return check;																							// check vale 1 si no hubo errores, o 0 si existió algún error
}

static int printQuery4(FILE * query4, queriesADT queries, titleADT title)
{
	toBeginTopSeries(queries);																				// Se inicializa el iterador
	int check = printQueries(query4,queries,NULL,title,TRUE,TRUE,hasNextTopSeries,nextTopSeries);			// Se imprime el resultado de la query en el archivo de respuesta
	return check;																							// check vale 1 si no hubo errores, o 0 si existió algún error
}

static int printQuery5(FILE * query5, queriesADT queries,allGenres * genres, titleADT title) 
{
	toBeginWorstSeries(queries);                                                      						// Se inicializa el iterador
	int check = printQueries(query5,queries,genres,title,TRUE,TRUE,hasNextWorstSeries,nextWorstSeries);		// Se imprime el resultado de la query en el archivo de respuesta
	return check;																							// check vale 1 si no hubo errores, o 0 si existió algún error
}

static int printQueries(FILE * query, queriesADT queries,allGenres * genres, titleADT title, int isSeries, int showAnimation, int (*hasNextFunction) (queriesADT queries), int (*nextFunction)(queriesADT queries,titleADT title, int *flag))
{
	int check=TRUE;												// Si check es 0, entonces hubo un error. Si no hubo errores vale 1
	while(hasNextFunction(queries) == TRUE)						// Mientras haya un elemento siguiente en la lista
	{
		nextFunction(queries,title,&check);						// Se verifica que haya un elemento siguiente en la lista
		if(check == FALSE)										// Si hubo un error
		{
			return FALSE;										// Retorna FALSE
		}
		printStartYear(query, title);							// Imprime el año de comienzo de la obra en el archivo de respuesta 
		PRINT_SEPARATOR(query)									// Imprmime en el archivo de respuesta un separador para identificar cuando termina un campo
		if(isSeries == TRUE)									// En caso de que se trate se series (query 4 y 5)
		{
			printEndYear(query, title);							// Imprime el año de finalización de la obra en el archivo de respuesta
			PRINT_SEPARATOR(query)								// Imprmime en el archivo de respuesta un separador para identificar cuando termina un campo
		}
		check = printTitle(query, title);						// Imprime el titulo de la obra en el archivo de respuesta
		if(check == FALSE)										// Si hubo error
		{
			return FALSE;										// Retorna FALSE
		}
		PRINT_SEPARATOR(query)									// Imprmime en el archivo de respuesta un separador para identificar cuando termina un campo
		printVotes(query, title);								// Imprime la cantidad de votos de la obra en el archivo de respuesta
		PRINT_SEPARATOR(query)									// Imprmime en el archivo de respuesta un separador para identificar cuando termina un campo
		printRating(query, title);								// Imprime el rating de la obra en el archivo de respuesta
		if(genres != NULL)										// Si la lista de generos válidos no es NULL
		{
			PRINT_SEPARATOR(query);								// Imprmime en el archivo de respuesta un separador para identificar cuando termina un campo
			printGenres(query, title, genres, showAnimation);	// Imprime los generos de la obra en el archivo de respuesta
		}
		PRINT_ENTER(query)										// Imprmime en el archivo de respuesta un newLine para identificar el fin de la query
	}
	return TRUE;
} 

static int printTitle(FILE * query,titleADT title)
{
	char * str = NULL;
	int check;
	check = returnTitleName(title,&str);       				// En str queda una copia del titulo de la obra
	if(check == FALSE)										// Si check es FALSE, entonces hubo un error
	{		
		return FALSE;   		
	}		
	fprintf(query,"%s",str);								// Se imprime en el archivo de respuesta el titulo de la obra
	free(str);												// Se libera el string que se uso para guardar el titulo
	return TRUE;
}

static void printStartYear(FILE * query, titleADT title){
	fprintf(query, "%d", returnStartYear(title));  			// Imprime en el archivo el año de comienzo de la obra
}

static void printEndYear(FILE * query, titleADT title){
	int year = returnEndYear(title);  						// En year se guarda el año de finalización de la obra
	if (year <= NO_YEAR){									// Si no es un año válido
		fprintf(query, NO_ELEM_STR2);						// Se imprime en el archivo de respuesta un "\N" en el campo del año de finalización
	} else{
		fprintf(query, "%d", year); 						// En cambio, is el año es válido se imprime ese año en el archivo de respuesta
	}
}

static void printVotes(FILE * query, titleADT title){
	fprintf(query, "%d", returnVotes(title));        		// Imprime en el archivo la cantidad de votos de la obra
}

static void printRating(FILE * query, titleADT title){
	fprintf(query, "%0.1f", returnRating(title));  			// Imprime en el archivo el rating de la obra
}

static void printGenres(FILE * query, titleADT title, allGenres * genres, int printAnimations){
	int dim = returnGenCount(title);    																			// En dim se guarda la cantidad de generos de la obra
	int i;
	int addCounter = 0;																								// Indica la cantidad de géneros que se agregaron
	int genreNum;
	for (i = 0 ; i < dim ; i++){																					// Va desde 0 hasta la cantidad de generos de esa obra
		genreNum = returnGenre(title, i);																			// Se guarda en genreNum la dimensión del arreglo con los generos de cada obra
		if (((printAnimations == TRUE) || (stringCompare(genres->genresName[genreNum], Q2_GENRE_NAME) != 0))){		// Se verifica si hay que imprimir o no el género "animation"
			if (addCounter != 0){																					// Si no es el 1° género que se agrega
				fprintf(query, ",");																				// Imprime en el archivo de respuesta una coma (que separa cada género)
			}
			
			fprintf(query, "%s", genres->genresName[genreNum]);														// Imprime el género en el archivo de respuesta							
			addCounter++;																							// Incrementa el contador de géneros
		}
	}
	if (addCounter == 0){																							// Si no hay ningún género
		fprintf(query, NO_ELEM_STR2);																				// Se imprime en el archivo de respuesta el indicador de campo vacío (\N)
	}
}

