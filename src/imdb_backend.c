#include "imdb_backend.h"
#include "imdb_backend_interfaces.h"
#include <stdlib.h>

#include <stdio.h>

#define NO_ID -2
#define NO_NEW_NODE -3

// Estructura para manipular titulos que se necesitan mantener en memoria dinamica
typedef struct tElement{
	int id;
	titleADT title;
	unsigned char inUse;
} tElement;

typedef tElement * pElement;

typedef struct titleNode * titleList;

typedef struct titleNode {
	pElement element;
	titleList nextTitle;
} titleNode;

typedef struct yearNode * yearList;

// yearNode es una estructura que contiene datos ordenados anualmente.
// Estos datos se necesitan para el Query 1, Query 2 y Query 3
typedef struct yearNode {
	int year;
	// Query 1										// Año de la informacion guardad por el nodo
	unsigned int nMovies;							// Numero de films de el año year
	unsigned int nSeries;							// Numero de series de el año year
	unsigned int nShorts;							// Numero de shorts de el año year
	// Query 3
	titleList topRanking;							// Mejores peliculas del año year ordenados por votos y alfabeticamente (si hay empate)
	unsigned int rankingDim;						// Cantidad de elementos en el ranking
	// Proximo elemento de la lista de años
	yearList nextYear;								// Puntero al proximo nodo (año) de la lista
}yearNode;

// queriesCDT es una estructura que contiene los datos necesitados por todos los queries
typedef struct queriesCDT{
	// Query 1, Query 3
	yearList firstYear;								// Lista ordenada por año
	// Query 2
	titleList topAnimatedFilms;						// Lista de las mejores peliculas animadas ordenada por ranking y votos (si hay empate)
	// Query 4
	titleList topSeries;							// Lista de las mejores series ordenada por ranking y votos (si hay empate)
	// Query 5
	titleList worstSeries;							// Lista de las peores series ordenada por ranking y votos (si hay empate)
	// Iteradores:
	yearList yearIterator;							// Iterar año
	titleList yearRankingIter;						// Iterar ranking del año
	titleList topAnimatedFilmsIterator;				// Iterar mejores peliculas animadas
	titleList topSeriesIterator;					// Iterar mejores series
	titleList worstSeriesIterator;					// Iterar peores series


	// Con la nueva implementacion de updateRank se pueden sacar ya que se cuentan internamente en la funcion.
	// El tema es que haria recursiones 5050 * 4 recursiones de mas.
	unsigned int nTopAnimatedFilms;							// Cntidad de elementos en la lista de mejores peliculas
	unsigned int nTopSeries;						// Cantidad de elementos en la lista de mejores series
	unsigned int nWorstSeries;						// Cantidad de elementos en la lista de peores serie
	
	pElement currentElement;						// Estructura para manipular titulos que deben interactuar con el frontend
	pElement * storeData;							// Arreglo de estructuras que guarda los punteros a los elementos utilizados por los ranking
	unsigned int storageMax;						// Tamaño maximo de almacenamiento (crece de a bloque)
	unsigned int storageDim;						// Cantidad de elementos dentro del arreglo
} queriesCDT;

// Prototipos de funciones static:


static pElement newCurrentElement(unsigned int maxId);

static int updateQueries(queriesADT queries, int yearLowerLimit, int yearUpperLimit);


// findYear encuentra el nodo de una año en partular en una lista de años. Si no lo encuentra, crea un nuevo nodo y lo agrega a la lista
static yearList findYear(yearList first, int year, yearList * current);

static int checkYearCondition(int startYear, int endYear, int yMin, int yMax);

// setQuery1 actualiza los datos que pertenecen al query1
static void setQuery1(yearList current, enum titleType type);

// setQuery2 actualiza los datos que pertenecen al query2
static int setQuery2(queriesADT queries, pElement title, int * removeID);

// setQuery3 actualiza los datos que pertenecen al query3
static int setQuery3(yearList current, pElement title, int * removeID);

// setQuery4 actualiza los datos que pertenecen al query4
static int setQuery4(queriesADT queries, pElement title, int * removeID);

// setQuery5 actualiza los datos que pertenecen al query5
static int setQuery5(queriesADT queries, pElement title, int * removeID);


static titleList findLast(titleList list, int * flag);

static titleList updateRank(titleList first, pElement title, int (*compare) (titleADT t1, titleADT t2), int * flag, int * removeID);

static int rankMaker(titleList * ranking, pElement element, unsigned int dim, int max, int (*compare) (titleADT t1, titleADT t2), int * removeID);


static int addCurrentToStorage(queriesADT queries);

static void removeId(queriesADT queries, int replaceID);

static int updateStorage(queriesADT queries, int replaceID1, int replaceID2);


// Funciones de comparacion

static int compareRatingVotes(titleADT t1, titleADT t2);

static int compareVotesNames(titleADT t1, titleADT t2);

static int compareQ5(titleADT t1, titleADT t2);


static int hasNext(titleList nTitle);

static int nextItem(titleList * nTitle,titleADT title);

static void freeElement(pElement element);

static void freeList(titleList list);

static void freeYears(yearList year);

static void freeStorage(pElement * vec, unsigned int dim);

void freeQueries(queriesADT queries);

////////////////////////////////////////////////////////////


// Funcion que crea nuevos queriesADT
queriesADT newQueries(void){
	queriesADT new = calloc(1, sizeof(queriesCDT));	// Todo inicializado en 0 y NULL
	return new;
}

int processData(queriesADT queries, titleADT title, genreList titleGenres, allGenres * validGenres, int yearLowerLimit, int yearUpperLimit){

	if (title == NULL) {															// Se introdujo un titulo invalido
		return TRUE;
	}
	if (returnType(title) < MOVIE) {												// Titulo de tipo invalido
		return TRUE;
	}
	if (returnVotes(title) == 0) {													// Ignorar titulos con cero votos
		return TRUE;
	}
	if (returnStartYear(title) <= NO_YEAR){		// startYear invalido
		return TRUE;
	}
	
	setGenres(title, validGenres, titleGenres);

	if (queries->currentElement == NULL){											// Si el currentElement no apunta nada significa que se debe crear uno nuevo
		queries->currentElement = newCurrentElement(queries->storageDim);
		if (queries->currentElement == NULL){
			return FALSE;
		}
	}

	int check = titleCopy(queries->currentElement->title, title);					// Como el titulo es valido es copiado a currentElement para ser analizado
	if (check == FALSE){
		return FALSE;
	}

	check = updateQueries(queries, yearLowerLimit, yearUpperLimit);					// Funcion encargada de actualizar los queries
	
	return check;
}

// newCurrentElement crea un nuevo puntero a tElement y le otorga un id
static pElement newCurrentElement(unsigned int maxId){			
	pElement new = malloc(sizeof(tElement));							// Alocamiento de memoria	
	if (new == NULL){
		return NULL;
	}
	new->title = newTitle();											// Se crea un nuevo titleADT y se lo asigna
	if (new->title == NULL){
		free(new);
		return NULL;
	}
	new->inUse = 0;														// No esta en uso
	new->id = maxId;													// Asignacion de indice para la posible posicion que podria tener en el arreglo de elementos
	return new;
}

static yearList findYear(yearList first, int year, yearList * current){
	if (first == NULL || first->year < year){				// No se encontro un nodo con el año buscado
		yearList new = calloc(1, sizeof(yearNode));			// Alocamiento de memoria para un nuevo yearNode
		if (new == NULL){									// Se produjo un error de alocamiento de memoria
			*current = NULL;					// Devuelve un puntero a NULL
			return NULL;						// No se agregan nodos a la lista
		}
		new->year = year;						// Se asigna el nuevo año
		new->nextYear = first;						// Se conecta el nodo con el resto de la lista
		*current = new;							// Se asigna el puntero current como parametro de salida
		return new;							// Se retorna el nodo para conectar con el principio de la lista
	}
	if (first->year == year){						// Se ecnotro el año buscado
		*current = first;						// Se asigna el parametro de salida
	}else {
		first->nextYear = findYear(first->nextYear, year, current);	// Todavia no se encontro el año y tampoco encontro uno mas chico
	}
	return first;								// Conecto con nodos anteriores de la lista
}

// updateQueres actualiza los datos correspondiente a cada titulo
// Recibe:
// 	queriesADT: Puntero a la estructura que contiene todos los datos.
// 	yearLowerLimit: Año minimo para analisis de titulos para el query 5 y query 6
// 	yearUpperLimit: Año maximo para analisis de titulos para el query 5 y query 6
// 	Si el yearUpperLimit es menor al yearLowerLimit se consideran todos los años
// Devuelve:
// 	TRUE: Salio todo bien
// 	FALSE: Error de alocamiento de memoria
static int updateQueries(queriesADT queries, int yearLowerLimit, int yearUpperLimit){
	unsigned int check = TRUE; 													// Verificador de error. 1: Todo salio bien. 0 Algo salio mal.

	int titleStartYear = returnStartYear(queries->currentElement->title);			// Se guarda el start
	int titleEndYear = returnEndYear(queries->currentElement->title);
	unsigned int titleType = returnType(queries->currentElement->title);
	unsigned int titleVotes = returnVotes(queries->currentElement->title);

	printf("%d\n",titleStartYear);
	printf("%d\n",titleEndYear);
	printf("%d\n",yearLowerLimit);
	printf("%d\n",yearUpperLimit);
	
	yearList current;															// Puntero al nodo del año al cual el titulo pertenece
	queries->firstYear = findYear(queries->firstYear, titleStartYear, &current);		// Se encuentra el nodo del año del titulo o se crea uno nuevo si es necesario
	if (current == NULL) { 														// Si current es NULL significa que hubo un error
		return 0;
	}
	setQuery1(current, titleType); 			// Se analiza el titulo y se actualizan los datos del query 1
	
	int replaceID1 = NO_ID;
	int replaceID2 = NO_ID;
	//printf("ANTES DE IFS\n");
	if (titleType == MOVIE) {					// El query 3 y el query 4 son en relacion a film unicamente
		if ((returnIsAnimation(queries->currentElement->title) == TRUE) && (titleVotes >= MIN_VOTES_Q2)){
			printf("						entre Q2\n");
			check = setQuery2(queries, queries->currentElement, &replaceID1); 				// Se analiza el titulo y se actualiza el top ranking del año si es necesario (query 3)
			if (check == NEW_TITLE_NODE_ERROR) {														// Verifica si hubo algun error de alocamiento de memoria
				return 0;
			}
		}
				printf("							entre Q3\n");
		check = setQuery3(current, queries->currentElement, &replaceID1);			// Se analiza el titulo y se actualiza el top ranking de films
		if (check == NEW_TITLE_NODE_ERROR) {													// Verifica si hubo algun error de alocamiento de memoria
			return 0;
		}
	} 																			// El querry 5 y 6 son en relacion a series unciamente entre los años limites especificados
	else if ((titleType == TV_SERIES || titleType == TV_MINI_SERIES) && (checkYearCondition(titleStartYear, titleEndYear, yearLowerLimit, yearUpperLimit) == 1)) {
		if (titleVotes >= MIN_VOTES_Q4){
			printf("							entre Q4\n");
			check = setQuery4(queries, queries->currentElement, &replaceID1); 				// Se analiza el titulo y se actualiza el top ranking de series
			if (check == NEW_TITLE_NODE_ERROR) {														// Verifica si hubo algun error de alocamiento de memoria
				return 0;
			}
		}
		if (titleVotes >= MIN_VOTES_Q5){
			printf("							entre Q5\n");
			check = setQuery5(queries, queries->currentElement, &replaceID2); 				// Se analiza el titulo y se actualiza el worst ranking de series
			if (check == NEW_TITLE_NODE_ERROR) {														// Verifica si hubo algun error de alocamiento de memoria
				return 0;
			}
		}
	}
	check = updateStorage(queries, replaceID1, replaceID2);						// Actualizar el arreglo de elementos
	if (check == 0){
		return 0;
	}

	return 1;
}


// Esta funcion verifica si el titulo cumple las condicion para el query 5 y el query 6
//
static int checkYearCondition(int startYear, int endYear, int yMin, int yMax){
	if (yMin == 0){
		printf("			ENTRE 1\n");
		return 1;
	}
	if ((yMax == 0) && (startYear >= yMin)){
		printf("			ENTRE 2\n");
		return 1;
	}
	if ((startYear >= yMin) && (startYear <= yMax)){
		printf("			ENTRE 3\n");
		return 1;
	}
	if ((endYear != NO_YEAR) && (endYear >= yMin) && (endYear <= yMax)){
				printf("			ENTRE 4\n");

		return 1;
	}
	if ((startYear <= yMax) && (endYear == NO_YEAR)){
		return 1;
	}
	printf("Sali!\n");
	return 0;
}

// Funcion para agrega el currentElement al final del arreglo de pElement
static int addCurrentToStorage(queriesADT queries){
	if (queries->storageDim == queries->storageMax){												// Verifica si hay espacio
		queries->storageMax += BLOCK;																// Se incrementa la dimension maxima
		queries->storeData = realloc(queries->storeData, queries->storageMax * sizeof(tElement));	// En el caso de que no haya espacio se reserva de a bloque
		if (queries->storeData == NULL){															// Error de alocamiento
			return FALSE;
		}
	}
	queries->storeData[queries->storageDim] = queries->currentElement;								// Guardo el currentElement en la ultima posicion del arreglo
	queries->currentElement = NULL;																	// Blanquear el currentElement ya que no se necesita mas para esta iteracion
	queries->storageDim += 1;																		// Se incrementa el contador de la dimension del arreglo
	return TRUE;
}

// Funcion para remover del arreglo el elemento de un id especifico
static void removeId(queriesADT queries, int replaceID){
	pElement aux = queries->storeData[replaceID];													// Elemento a eliminar y a liberar
	queries->storageDim -= 1;																		// Se saca un elemento de la lista entonces disminuye la dimension
	queries->storeData[replaceID] = queries->storeData[queries->storageDim];						// Se remplaza el elemento a remover por el ultimo elemento del arreglo
	queries->storeData[queries->storageDim] = NULL;													// Se pone NULL como ultimo elemento del arreglo
	queries->storeData[replaceID]->id = replaceID;	
	freeElement(aux);
}

// Funcion para agreagr o eleiminar elementos del arreglo con todos los elementos utilizados
static int updateStorage(queriesADT queries, int replaceID1, int replaceID2){
	if (queries->currentElement->inUse != 0){														// Verifica que se utilizo el currentElement
		int check = addCurrentToStorage(queries);													// Agrega a current element al final del arreglo
		if (check == 0){
			return FALSE;
		}
		if (replaceID1 > NO_ID){
			// Verifica si se debe liberar un elemento
			removeId(queries, replaceID1);															// Libera elemento
		}
		if (replaceID2 > NO_ID){
			// Verifica si se debe liberar otro elemento
			removeId(queries, replaceID2);															// Libera elemento
		}
	}																								// En el caso de no haberse utilizado entonces no se debe agregar ni tampoco sacar elementos del arreglo
	return TRUE;
}

// setQuery1 actualiza los datos relaiconados al query 1
static void setQuery1(yearList current, enum titleType type){
	switch (type)
	{
		case MOVIE:
			current->nMovies += 1;  												//Si es una film, incrementa la cantidad de peliculas en ese año
			break;
		case SHORT:            													//Si es un short, incrementa la cantidad de cortos en ese año
			current->nShorts += 1;
			break;
		case TV_SERIES:        													//Si es una serie o una mini serie, incrementa la cantidad de series en ese año
		case TV_MINI_SERIES:
			current->nSeries += 1;
			break;
		default:																// Cualquier otro caso no hace nada
			;
	}
}

//static titleList updateRank(titleList first, pElement element, int (*compare) (titleADT t1, titleADT t2), int * flag){
//	int c;
//	if (first == NULL || (c = compare(first->element->title, element->title)) <= 0){
//		titleList new = malloc(sizeof(titleNode));
//		if (new == NULL){
//			*flag = NEW_TITLE_NODE_ERROR;
//			return first;			
//		}
//		new->element = element;
//		new->nextTitle = first;
//		new->element->inUse += 1;
//		if (*flag == 1){
//			if (first == NULL){
//			    *flag = NO_ID;
//			    return first;
//			} else {
//				new->nextTitle = findLast(first, flag);
//			}
//		}
//		else {
//			*flag = NO_NEW_NODE;
//		}
//		return new;
//	}
//	else if (c > 0){
//		first->nextTitle = updateRank(first->nextTitle, element, compare, flag);
//	}
//	else {
//		*flag = 1;
//	}
//	return first;
//}

static titleList updateRank(titleList first, pElement element, int (*compare) (titleADT t1, titleADT t2), int * flag, int * removeID){
	int c;
	printf("1\n");
	if (first == NULL){
		printf("2\n");
		if (*flag == 1){
			*flag = 0;			// No se agrego ningun elemento
			return first;
		}
	}
	if (first == NULL || ((c = compare(first->element->title, element->title)) <= 0)){
		printf("3\n");
		titleList new = malloc(sizeof(titleNode));
		if (new == NULL){
			*flag = NEW_TITLE_NODE_ERROR;
			return first;			
		}
		new->element = element;
		new->nextTitle = first;
		new->element->inUse += 1;
		if (*flag == 1){
			*flag=0;
			new->nextTitle = findLast(first, removeID);
		}
		else
		{
			*flag = 1;
		}
		return new;
	}
	if (c > 0){
		printf("4\n");
		first->nextTitle = updateRank(first->nextTitle, element, compare, flag, removeID);
	}
	//else {
	//	*flag = 0; 			// Se encontro un elemento repetido, no se hace nada.
	//}
	printf("sali\n");
	return first;
}

static int rankMaker(titleList * ranking, pElement element, unsigned int dim, int max, int (*compare) (titleADT t1, titleADT t2), int * removeID){
	int flag;
	if (dim == max){
		flag = 1;
	} else {
		flag = 0;
	}
	printf("flag ingreso %d\n", flag);
	*ranking = updateRank(*ranking, element, compare, &flag, removeID);
	return flag;
}

//static titleList findLast(titleList list, int * flag){  // Se encuentra el ultimo elemento del ranking
//	if (list->nextTitle == NULL){	                    //Caso base: si el siguiente elemento al actual es NULL (no hay)
//		list->element->inUse -= 1;                      //Se le quita un uso al elemento
//		if (list->element->inUse == 0){                 //Y si se quedo sin usos
//			*flag = list->element->id;                  //Hay que eliminarlo
//		} else {                                        //Si tiene al menos un uso no lo elimino
//			*flag = NO_ID;
//		}
//		free(list);																		// Se libera el nodo
//		return NULL;																		// Se retorna NULL indicando el nuevo ultimo elemento
//	}
//	list->nextTitle = findLast(list->nextTitle, flag);										// No se encuentra el ultimo elemento, se avanza al priximo elemento de la lista
//	return list;
//}


static titleList findLast(titleList list, int * flag){  // Se encuentra el ultimo elemento del ranking
	if (list->nextTitle == NULL){	                    //Caso base: si el siguiente elemento al actual es NULL (no hay)
		list->element->inUse -= 1;                      //Se le quita un uso al elemento
		if (list->element->inUse == 0){                 //Y si se quedo sin usos
			*flag = list->element->id;                  //Hay que eliminarlo
		} else {                                        //Si tiene al menos un uso no lo elimino
			*flag = NO_ID;
		}
		free(list);																		// Se libera el nodo
		return NULL;																		// Se retorna NULL indicando el nuevo ultimo elemento
	}
	list->nextTitle = findLast(list->nextTitle, flag);										// No se encuentra el ultimo elemento, se avanza al priximo elemento de la lista
	return list;
}

// serQuery2
static int setQuery2(queriesADT queries, pElement element, int * removeID){
	int flag = 0;
	flag = rankMaker(&queries->topAnimatedFilms, element, queries->nTopAnimatedFilms, MAX_TOP_ANIMATED_FILMS, compareRatingVotes, removeID);
	queries->nTopAnimatedFilms += flag;
	////printf("%d	%d\n", flag, queries->nTopAnimatedFilms);
	
	return flag;
}

// setQuery3
static int setQuery3(yearList currentYear, pElement element, int * removeID){
	int flag = 0;
	flag = rankMaker(&currentYear->topRanking, element, currentYear->rankingDim, MAX_TOP_YEAR, compareVotesNames, removeID);
	if (flag == 1){
		currentYear->rankingDim += 1;
	}
	return flag;
}

// serQuery4
static int setQuery4(queriesADT queries, pElement element, int * removeID){
	int flag = 0;
	flag = rankMaker(&queries->topSeries, element, queries->nTopSeries, MAX_TOP_SERIES, compareRatingVotes, removeID);
	if (flag == 1){
		queries->nTopSeries += 1;
	}
	return flag;
}

// serQuery5
// Hace una lista de elementos ordenada:
// Ascendente por puntaje, y a igualdad de puntaje ascendente por cantidad de votos.
static int setQuery5(queriesADT queries, pElement element, int * removeID){
	int flag = 0;
	flag = rankMaker(&queries->worstSeries, element, queries->nWorstSeries, MAX_WORST_SERIES, compareQ5, removeID);
	if (flag == 1){
		queries->nWorstSeries += 1;
	}
	return flag;
}



// Funcion de comparacion para los queries 2
// Retorna:
// > 0 si t1 > t2
// < 0 si t1 < t2
static int compareRatingVotes(titleADT t1, titleADT t2){ 
    float c = compareRating(t1, t2);
	if ((c * c) <= (ZERO * ZERO)){							// t1 y t2 son iguales
		return compareNumVotes(t1, t2);						// Comparacion por votos
	}
	else if (c > 0){										// t1 > t2
		return POSITIVE;
	}
	else {
		return NEGATIVE;									// t1 < t2
	}
}


// Funcion de comparacion del query 3
// Retorna:
// > 0 si t1 > t2
// < 0 si t1 < t2
static int compareVotesNames(titleADT t1, titleADT t2){
	int c;
	if ((c = compareNumVotes(t1, t2)) != 0){				// Comparacion por votos
		return c;
	} else {
		return compareTitleNames(t1, t2);					// Comparacion por nombre
	}
}

// Funcion de comparacion para el query 6
// Notar que es lo opuesto a lo que se quiere en el query 4 y en el query 5
// Retorna:
// > 0 si t1 < t2
// < 0 si t1 > t2
static int compareQ5(titleADT t1, titleADT t2){
	return compareRatingVotes(t2, t1);																// Llamado de la funcion de comparacion del query 4 y query 5 pero con los titulos invertidos
}



// FUNCIONES DE RETORNO:
// returnCurrentYearQ1 retorna la cantidad de films, series y shorts en el año en el cual apunta el iterador.
// Funcion de retorno para el query 1
void returnCurrentYearQ1(queriesADT queries, unsigned int * year,unsigned int * nFilms, unsigned int * nSeries, unsigned int * nShorts){
	if(queries->yearIterator==NULL){
		return;
	}
	*year=queries->yearIterator->year;
	*nFilms = queries->yearIterator->nMovies; 												// Retorno de cantidad de films ese año
	*nSeries = queries->yearIterator->nSeries;												// Retorno de cantidad de series ese año
	*nShorts = queries->yearIterator->nShorts;												// Retorno de cantidad de shorts ese año
}

// ITERADORES:

void toBeginYears(queriesADT queries){
	queries->yearIterator = queries->firstYear;	
}

void toBeginYearRankings(queriesADT queries){
	queries->yearRankingIter = queries->yearIterator->topRanking;
}

void toBeginTopAnimatedFilms(queriesADT queries){
	queries->topAnimatedFilmsIterator = queries->topAnimatedFilms;
}

void toBeginTopSeries(queriesADT queries){
	queries->topSeriesIterator = queries->topSeries;
}

void toBeginWorstSeries(queriesADT queries){
	queries->worstSeriesIterator = queries->worstSeries;
}

int hasNextYear(queriesADT queries){
	return queries->yearIterator != NULL;
}

static int hasNext(titleList nTitle){
	return nTitle != NULL;
}

int hasNextYearRanking(queriesADT queries){

	return hasNext(queries->yearRankingIter);
}

int hasNextTopAnimatedFilms(queriesADT queries){

	return hasNext(queries->topAnimatedFilmsIterator);
}

int hasNextTopSeries(queriesADT queries){

	return hasNext(queries->topSeriesIterator);
}

int hasNextWorstSeries(queriesADT queries){

	return hasNext(queries->worstSeriesIterator);
}

static int nextItem(titleList * nTitle,titleADT title){
	int check = titleCopy(title,(*nTitle)->element->title);
	(*nTitle) = (*nTitle)->nextTitle;
	return check;
}

// Iterador de años.
int nextYear(queriesADT queries){
	if (hasNextYear(queries) && queries->yearIterator->nextYear != NULL){														// Verifica que haya un año actual
		queries->yearIterator = queries->yearIterator->nextYear;					// Avanza el iterador al proximo año
		return 1; 																	// Retorna el puntero al elemento al año actual
	}
	else {
		return 0;																	// Si no quedan mas años retorna NULL
	}
}

// Iterador del top ranking de cada año (query3)
int nextYearRanking(queriesADT queries,titleADT title, int *flag){
	if (hasNext(queries->yearRankingIter)){													// Verifica que haya un elemento actual
		*flag = nextItem(&(queries->yearRankingIter),title);			
		return *flag;
	}
	return 0;
}

// Iterador para las mejores peliculas (query 4)
int nextTopAnimatedFilms(queriesADT queries,titleADT title, int *flag){					
	if (hasNext(queries->topAnimatedFilmsIterator)){													// Verifica que haya un elemento actual
		*flag = nextItem(&(queries->topAnimatedFilmsIterator),title);			
		return *flag;
	}
	return 0;																		// Si no quedan mas elementos en el ranking retorn NULL
}

// Iterador para las mejores series (query 5)
int nextTopSeries(queriesADT queries,titleADT title, int * flag){
		if (hasNext(queries->topSeriesIterator)){													// Verifica que haya un elemento actual
		*flag = nextItem(&(queries->topSeriesIterator),title);			
		return *flag;
	}
	return 0;
}

// Iterador para las peores series (query 6)
int nextWorstSeries(queriesADT queries, titleADT title, int *flag){
	if (hasNext(queries->worstSeriesIterator)){													// Verifica que haya un elemento actual
		*flag = nextItem(&(queries->worstSeriesIterator),title);			
		return *flag;
	}
	return 0;
}

// freeElement libera el elemento utilizado para guardar los titulos y su informacion
static void freeElement(pElement element)
{
	if(element != NULL){
		freeTitle(element->title);						// Libera el titulo
		free(element);									// Libera la esttructura
	}
}

// freeList libera las listas utilizadas para los ranking
// Notar que esta funcion de libera los contenidos de la lista ya que eso lo hace freeStorage
static void freeList(titleList list){
	if (list != NULL){
		freeList(list->nextTitle);					// Llamada recursiva para llegar hasta el final
		free(list);									// Liber el puntero al nodo
	}
}

// freeYear libera la lista de años
static void freeYears(yearList year){
	if (year != NULL){
		freeYears(year->nextYear);					// Lamada recursiva para llegar al final
		freeList(year->topRanking);					// Llamada para liberar la lista utilizada para el ranking
		free(year);
	}
}

// freeStorage libera todos los elementos dentro del arreglo y el arreglo 
static void freeStorage(pElement * vec,unsigned int dim)
{
	for (int i = 0; i < dim; i++)
	{
		freeElement(vec[i]);							// Libera todos los elementos del arreglo
	}
	free(vec);											// Libera el arreglo
}

void freeQueries(queriesADT queries){
	freeYears(queries->firstYear);
	freeList(queries->topAnimatedFilms);
	freeList(queries->topSeries);
	freeList(queries->worstSeries);
	freeStorage(queries->storeData,queries->storageDim);
	freeElement(queries->currentElement);
	free(queries);
}


int returnCurrentTitleName(queriesADT queries, char ** str){
	return returnTitleName(queries->currentElement->title, str);
}

enum titleType returnCurrentTitleType(queriesADT queries){
	return returnType(queries->currentElement->title);
}

int returnCurrentStartYear(queriesADT queries){
	return returnStartYear(queries->currentElement->title);
}

int returnCurrentEndYear(queriesADT queries){
	return returnEndYear(queries->currentElement->title);
}

float returnCurrentRating(queriesADT queries){
	return returnRating(queries->currentElement->title);
}

int returnCurrentVotes(queriesADT queries){
	return returnVotes(queries->currentElement->title);
}

int returnCurrentGenres(queriesADT queries, unsigned int index){
	return returnGenre(queries->currentElement->title, index);
}


