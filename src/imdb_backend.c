#include "imdb_backend.h"
#include "imdb_backend_interfaces.h"
#include <stdlib.h>

#include <stdio.h>

#define NO_ID -1
#define LIST_ALLOC_ERROR -1

// Estructura para manipular titulos que se necesitan mantener en memoria dinamica
// El programa guardara dinamicamente solo los titulos que se necesitan en un arreglo de punteros a tElement
typedef struct tElement{
	int id;													// Indice del arreglo en el cual se encuetra almacenado el puntero a tElement
	titleADT title;											// titleADT que se esta almacenando 
	unsigned char inUse;									// Indicador de cuantos queries esta utilizando el titulo almacenado. En el caso que sea cero, esto indica que se puede liberar el titulo de la memoria.
} tElement;

typedef tElement * pElement;								// Definicion del tipo de dato pElement

typedef struct titleNode * titleList;						// Definicion del tipo de dato titleList

// Para los queries que piden hacer un ranking se utiliza una lista de pElement
typedef struct titleNode {
	pElement element;										// pElement que contiene al titulo
	titleList nextTitle;									// Puntero al proximo elemento de la lista
} titleNode;

typedef struct yearNode * yearList;							// Definicion del tipo de dato yearList

// yearNode es una estructura que contiene datos ordenados anualmente.
// En la estructura principal queriesADT se guarda el primer año de un lista ordenada descendentemente por año
// Estos datos se necesitan para el Query 1 y query 3
typedef struct yearNode {
	int year;
	// Query 1												// Año de la informacion guardad por el nodo
	unsigned int nMovies;									// Numero de films de el año year
	unsigned int nSeries;									// Numero de series de el año year
	unsigned int nShorts;									// Numero de shorts de el año year
	// Query 3
	titleList topRanking;									// Mejores peliculas del año year ordenados por votos y alfabeticamente (si hay empate)
	unsigned int rankingDim;								// Cantidad de elementos en el ranking
	yearList nextYear;										// Puntero al proximo nodo (año) de la lista
}yearNode;

// queriesCDT es una estructura que contiene los datos necesitados por todos los queries
typedef struct queriesCDT{
	// Query 1, Query 3
	yearList firstYear;										// Lista ordenada por año
	// Query 2
	titleList topAnimatedFilms;								// Lista de las mejores peliculas animadas ordenada por ranking y votos (si hay empate)
	// Query 4
	titleList topSeries;									// Lista de las mejores series ordenada por ranking y votos (si hay empate)
	// Query 5
	titleList worstSeries;									// Lista de las peores series ordenada por ranking y votos (si hay empate)
	// Iteradores:
	yearList yearIterator;									// Iterar año
	titleList yearRankingIter;								// Iterar ranking del año
	titleList topAnimatedFilmsIterator;						// Iterar mejores peliculas animadas
	titleList topSeriesIterator;							// Iterar mejores series
	titleList worstSeriesIterator;							// Iterar peores series

	int lowerYearLimit;										// Año minimo para los queries 4 y 5
	int upperYearLimit;										// Año minimo para los queries 4 y 5

	unsigned int nTopAnimatedFilms;							// Cntidad de elementos en la lista de mejores peliculas
	unsigned int nTopSeries;								// Cantidad de elementos en la lista de mejores series
	unsigned int nWorstSeries;								// Cantidad de elementos en la lista de peores serie
	
	pElement currentElement;								// Estructura para manipular titulos que deben interactuar con el frontend
	pElement * storeData;									// Arreglo que guarda los punteros a los elementos utilizados por los ranking
	unsigned int storageMax;								// Tamaño maximo de almacenamiento (crece de a bloque)
	unsigned int storageDim;								// Cantidad de elementos dentro del arreglo
} queriesCDT;

// Prototipos de funciones static:

// newCurrentElement crea un nuevo pElement cuando se necesita. Esto ocurrira en el primer titulo y en la iteracion proxima de que un titulo se utilice en algun querry
// Recibe la dimension actual del arreeglo storeData para asignarlo como el ID del nuevo currentElement
static pElement newCurrentElement(unsigned int maxId);

// updateQuerries es una funcion que se engcarga de actualizar los queries
// La funcion retorna TRUE si todo salio bien y FALSE si hubo algun error de alocamiento de memoria
// Parametros de entrada y salida:
// 	queries: ADT que contiene los queries y el currentElement con el titulo que sera analizado
static int updateQueries(queriesADT queries);


// findYear encuentra el nodo de una año en partular en una lista de años. Si no lo encuentra, crea un nuevo nodo y lo agrega a la lista
static yearList findYear(yearList first, int year, yearList * current);

// Funcion que chequea si el titulo esta dentro de los años indicados cuando se llamo al programa.
// Esto solo se necesita para el query 4 y 5
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

// La funcion rankMaker es llamada por las funciones setQuery
// Esta funcion verifica si se llego a la maxima cantidad de elemento en un ranking y se encarga de llamar a updateRank
// Parametros de entrada:
// 	ranking: Ranking que se quiere actualizar
// 	element: Elemento que contiene el titulo que se quiere agregar al ranking
// 	dim: Cantidad actual de elementos dentro del ranking
// 	max: Cantidad maxima de elementos que puede contener cada query. Estas son constantes definidas en imdb_backend.h
// 	compare: Puntero a la funcion de comparacion que depende de cada query
// Parametro de salida:
// 	removeID: Indice del elemento a remover del arreglo de pElements que se necesitan en los queries
static int rankMaker(titleList * ranking, pElement element, unsigned int dim, int max, int (*compare) (titleADT t1, titleADT t2), int * removeID);

// updateRank es una funcion recursiva que actualiza una lista de titleList
// Si la maxima cantidad de elementos que puede tener el rank es exedida (se indica con el flag) se retorna en removeID el indice del elemento encontrado por findLast
// Parametros de entrada:
// 	first: Puntero de nodo de la lista que se debe analizar en la recursion actual
// 	title: Titulo el cual se quiere agregar a la lista
// 	compare: Puntero a la funcion de comparacion que depende de cada query
// Parametro de entrada y salida:
// 	flag: Indicador de si la lista tiene la cantidad maxima de elementos posibles (determinado por la funcion rankMaker). 
// 		Retorna:
// 			TRUE si se agrego un elemento
// 			FALSE si no se agrego un elemento o si solo se remplazo un elemento
// 			LIST_ALLOC_ERROR si ocurrio un error de alocamiento cuando se reserva el nuevo nodo.
// 	removeID: Indice del elemento a remover del arreglo de pElements que se necesitan en los queries. En el caso de que no se deba liberar ningun pElement la variable retorna NO_ID
static titleList updateRank(titleList first, pElement title, int (*compare) (titleADT t1, titleADT t2), int * flag, int * removeID);

// findLast es una funcion recursiva que encuentra el ultimo elemento de una lista. La funcoin es llamada solo cuando se agrega una elemento a una lista.
// Parametros de enrada:
// 	list: Elemnto proximo al elemento agregado a la lista.
// Parametros de salida:
// 	flag: Id del ultimo pElement de la lista. Si el pElement sigue siendo utilizado en otro query (indicado por inUse) la funcion retorna NO_ID
static titleList findLast(titleList list, int * flag);

// addCurrentToStorage agrega el curretnElement del parametro queries al arreglo con todos los pElements que estan en uso
static int addCurrentToStorage(queriesADT queries);

// removeId es una funcion que libera de memoria el elemento que esta en la arreglo de pElements en el indice indicado por replaceID
static void removeId(queriesADT queries, int replaceID);

// La funcion updateStorage se encarga de agregar y liberar pElements del arreglo con todos los pEleemnts que se estan utilizando por lo queries
static int updateStorage(queriesADT queries, int replaceID1, int replaceID2);

// Funciones de comparacion:
// Esta funcion compara dos titleADT segun su rating. A igualdad de rating se compara el numero de votos.
static int compareRatingVotes(titleADT t1, titleADT t2);


// Esta funcion compara dos titleADT segun su numero de votos. A igualdad de numero de votos se comparan los nombres de los titulos alfabeticamente.
static int compareVotesNames(titleADT t1, titleADT t2);

// Funcion de comparacion para el query 5. Llama a compareRatingVotes, pero con los parametros de entrada cambiados de lugar.
static int compareQ5(titleADT t1, titleADT t2);


// ITERADORES:
void toBeginYears(queriesADT queries);

void toBeginYearRankings(queriesADT queries);

void toBeginTopAnimatedFilms(queriesADT queries);

void toBeginTopSeries(queriesADT queries);

void toBeginWorstSeries(queriesADT queries);

int hasNextYear(queriesADT queries);

static int hasNext(titleList nTitle);

int hasNextYearRanking(queriesADT queries);

int hasNextTopAnimatedFilms(queriesADT queries);

int hasNextTopSeries(queriesADT queries);

int hasNextWorstSeries(queriesADT queries);

static int nextItem(titleList * nTitle,titleADT title);

// Iterador de años.
int nextYear(queriesADT queries);

// Iterador para las mejores peliculas (query 4)
int nextTopAnimatedFilms(queriesADT queries,titleADT title, int *flag);

// Iterador para las peores series (query 5)
int nextWorstSeries(queriesADT queries, titleADT title, int *flag);

// FUNCIONES DE RETORNO:
// returnCurrentYearQ1 retorna la cantidad de films, series y shorts en el año en el cual apunta el iterador.
// Funcion de retorno para el query 1
void returnCurrentYearQ1(queriesADT queries, unsigned int * year,unsigned int * nFilms, unsigned int * nSeries, unsigned int * nShorts);

// Funciones de liberacion de memoria:
// freeElement libera el elemento utilizado para guardar los titulos y su informacion
static void freeElement(pElement element);

// freeList libera las listas utilizadas para los ranking
// Notar que esta funcion de libera los contenidos de la lista ya que eso lo hace freeStorage
static void freeList(titleList list);

// freeYear libera la lista de años
static void freeYears(yearList year);

// freeStorage libera todos los elementos dentro del arreglo y el arreglo 
static void freeStorage(pElement * vec,unsigned int dim);

// freeQueries libera toda la informacion almacenada por queries
void freeQueries(queriesADT queries);

////////////////////////////////////////////////// Definicion de Funciones //////////////////////////////////////////////////


// Funcion que crea nuevos queriesADT
queriesADT newQueries(int yMin, int yMax){
	queriesADT new = calloc(1, sizeof(queriesCDT));								// Todo inicializado en 0 y NULL
	new->lowerYearLimit = yMin;
	new->upperYearLimit = yMax;
	return new;
}

// processData es una funcion que recibe un titulo y se encarga de decidir si ese titulo cumple las condiciones generales para ser evaluada por al menos el query 1. Estas condiciones son que tenga un tipo valido y que tenga un startYear valido
// Por otro lado, se copia el titulo actual al currentElement de la estructura queries
// Luego, una vez que se copiaron todos los datos del titulo se le ingresan los generos y se identifica si el titulo es o no una animacion
int processData(queriesADT queries, titleADT title, genreList titleGenres, allGenres * validGenres){

	if (title == NULL) {														// Se introdujo un titulo invalido
		return TRUE;															// Retorna TRUE
	}
	if (returnType(title) == NO_VALID_TYPE) {									// Se verifica que el tipo de dato sea válido
		return TRUE;															// Retorna TRUE
	}
	if (returnVotes(title) == 0) {												// Se ignoran los títulos con 0 votos
		return TRUE;															// Retorna TRUE
	}
	if (returnStartYear(title) <= NO_YEAR){										// startYear invalido
		return TRUE;															// Retorna TRUE
	}
	

	if (queries->currentElement == NULL){										// Si el currentElement no apunta nada significa que se debe crear uno nuevo
		queries->currentElement = newCurrentElement(queries->storageDim);
		if (queries->currentElement == NULL){
			return FALSE;
		}
	}

	int check = titleCopy(queries->currentElement->title, title);				// Como el titulo es valido es copiado a currentElement para ser analizado
	if (check == FALSE){
		return FALSE;
	}

	setGenres(queries->currentElement->title, validGenres, titleGenres);		// Se actualiza el arreglo de generos dentro de title. Si title es una animacion se actualiza el camp isAnimation a TRUE
	check = updateQueries(queries);												// Funcion encargada de actualizar los queries
	
	return check;
}

// newCurrentElement crea un nuevo puntero a tElement y le otorga un id
static pElement newCurrentElement(unsigned int maxId){			
	pElement new = malloc(sizeof(tElement));									// Alocamiento de memoria
	if (new == NULL){
		return NULL;															
	}
	new->title = newTitle();													// Se crea un nuevo titleADT y se lo asigna
	if (new->title == NULL){
		free(new);
		return NULL;
	}
	new->inUse = 0;																// No esta en uso
	new->id = maxId;															// Asignacion de indice para la posible posicion que podria tener en el arreglo de elementos
	return new;
}

static yearList findYear(yearList first, int year, yearList * current){
	if (first == NULL || first->year < year){									// No se encontro un nodo con el año buscado
		yearList new = calloc(1, sizeof(yearNode));								// Alocamiento de memoria para un nuevo yearNode
		if (new == NULL){														// Se produjo un error de alocamiento de memoria
			*current = NULL;													// Devuelve un puntero a NULL
			return NULL;														// No se agregan nodos a la lista
		}
		new->year = year;														// Se asigna el nuevo año
		new->nextYear = first;													// Se conecta el nodo con el resto de la lista
		*current = new;															// Se asigna el puntero current como parametro de salida
		return new;																// Se retorna el nodo para conectar con el principio de la lista
	}
	if (first->year == year){													// Se ecnotro el año buscado
		*current = first;														// Se asigna el parametro de salida
	}else {
		first->nextYear = findYear(first->nextYear, year, current);				// Todavia no se encontro el año y tampoco encontro uno mas chico
	}
	return first;																// Conecto con nodos anteriores de la lista
}

// updateQueres actualiza los datos correspondiente a cada titulo
// Recibe:
// 	queriesADT: Puntero a la estructura que contiene todos los datos.
// 	yearLowerLimit: Año minimo para analisis de titulos para el query 4 y query 5
// 	yearUpperLimit: Año maximo para analisis de titulos para el query 4 y query 5
// 	Si el yearUpperLimit es menor al yearLowerLimit se consideran todos los años
// Devuelve:
// 	TRUE: Salio todo bien
// 	FALSE: Error de alocamiento de memoria
static int updateQueries(queriesADT queries){
	unsigned int check = TRUE; 																					// Verificador de error. 1: Todo salio bien. 0 Algo salio mal.

	int titleStartYear = returnStartYear(queries->currentElement->title);										// Se guarda el start
	int titleEndYear = returnEndYear(queries->currentElement->title);						
	unsigned int titleType = returnType(queries->currentElement->title);						
	unsigned int titleVotes = returnVotes(queries->currentElement->title);						

	yearList current;																							// Puntero al nodo del año al cual el titulo pertenece
	queries->firstYear = findYear(queries->firstYear, titleStartYear, &current);								// Se encuentra el nodo del año del titulo o se crea uno nuevo si es necesario
	if (current == NULL) { 																						// Si current es NULL significa que hubo un error
		return FALSE;						
	}						

	setQuery1(current, titleType); 																				// Se analiza el titulo y se actualizan los datos del query 1
	
	int replaceID1 = NO_ID;
	int replaceID2 = NO_ID;

	if (titleType == MOVIE) {																					// El query 2 y el query 3 son en relacion a film unicamente
		if ((returnIsAnimation(queries->currentElement->title) == TRUE) && (titleVotes >= MIN_VOTES_Q2)){ 
			check = setQuery2(queries, queries->currentElement, &replaceID1); 									// Se analiza el titulo y se actualiza el top ranking del año si es necesario (query 3)
			if (check == FALSE) {																				// Verifica si hubo algun error de alocamiento de memoria
				return FALSE;
			}
		}
		check = setQuery3(current, queries->currentElement, &replaceID1);										// Se analiza el titulo y se actualiza el top ranking de films
		if (check == FALSE) {																					// Verifica si hubo algun error de alocamiento de memoria
			return FALSE;
		}
	}

	// El query 4 y 5 son en relación a series unciamente entre los años limites especificados
	// checkYearCondition es llamada para verificar los años del titulo
	// Se llama al final del if para que solo sea llamado si el titulo es una serie y no trate de chequear shorts
	else if ((titleType == TV_SERIES || titleType == TV_MINI_SERIES) && (checkYearCondition(titleStartYear, titleEndYear, queries->lowerYearLimit, queries->upperYearLimit) == TRUE)) {
		if (titleVotes >= MIN_VOTES_Q4){
			check = setQuery4(queries, queries->currentElement, &replaceID1); 									// Se analiza el titulo y se actualiza el top ranking de series
			if (check == FALSE) {																				// Verifica si hubo algun error de alocamiento de memoria
				return FALSE;						
			}						
		}						
		if (titleVotes >= MIN_VOTES_Q5){						
			check = setQuery5(queries, queries->currentElement, &replaceID2); 									// Se analiza el titulo y se actualiza el worst ranking de series
			if (check == FALSE) {																				// Verifica si hubo algun error de alocamiento de memoria
				return FALSE;						
			}						
		}						
	}						
	check = updateStorage(queries, replaceID1, replaceID2);														// Actualizar el arreglo de elementos
	if (check == FALSE){
		return FALSE;
	}

	return TRUE;
}


// Esta funcion verifica si el titulo cae dentro de los años especificados para el query 4 y el query 5
static int checkYearCondition(int startYear, int endYear, int yMin, int yMax){
	if (yMin == NO_YEAR){														// No hay restricciones de año
		return TRUE;
	}
	if ((yMax == NO_YEAR) && (startYear >= yMin)){								// No hay restricciones de año maximo y la serie comenzo a publicarse despues del limite inferior
		return TRUE;
	}
	if ((startYear >= yMin) && (startYear <= yMax)){							// La serie comenzo a publicarse dentro de los limites
		return TRUE;
	}
	if ((endYear != NO_YEAR) && (endYear >= yMin) && (startYear <= yMax)){		// La serie tiene alguna temporada dentro de los limites
		return TRUE;
	}
	if ((startYear <= yMax) && (endYear == NO_YEAR)){							// Siguen saliendo temporadas del titulo y la serie comenzo a salir antes del limite superior
		return TRUE;
	}
	return FALSE;																// Ninguna de las condiciones se cumplen entonces la serie no esta dentro del rango de años especificados
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
	queries->storeData[queries->storageDim] = queries->currentElement;			// Guardo el currentElement en la ultima posicion del arreglo
	queries->currentElement = NULL;												// Blanquear el currentElement ya que no se necesita mas para esta iteracion
	queries->storageDim += 1;													// Se incrementa el contador de la dimension del arreglo
	return TRUE;
}

// Funcion para remover del arreglo el elemento de un id especifico
static void removeId(queriesADT queries, int replaceID){
	pElement aux = queries->storeData[replaceID];								// Elemento a eliminar y a liberar
	queries->storageDim -= 1;													// Se saca un elemento de la lista entonces disminuye la dimension
	queries->storeData[replaceID] = queries->storeData[queries->storageDim];	// Se remplaza el elemento a remover por el ultimo elemento del arreglo
	queries->storeData[queries->storageDim] = NULL;								// Se pone NULL como ultimo elemento del arreglo
	queries->storeData[replaceID]->id = replaceID;								// Se le da el id del elemento viejo al elemento intercambiado
	freeElement(aux);
}

// Funcion para agreagr o eleiminar elementos del arreglo con todos los elementos utilizados
static int updateStorage(queriesADT queries, int replaceID1, int replaceID2){
	if (queries->currentElement->inUse != 0){									// Verifica que se utilizo el currentElement
		int check = addCurrentToStorage(queries);								// Agrega a current element al final del arreglo
		if (check == 0){
			return FALSE;
		}
		if (replaceID1 > NO_ID){												// Verifica si se debe liberar un elemento
			removeId(queries, replaceID1);										// Libera elemento
		}
		if (replaceID2 > NO_ID){												// Verifica si se debe liberar otro elemento
			removeId(queries, replaceID2);										// Libera elemento
		}
	}																			// En el caso de no haberse utilizado entonces no se debe agregar ni tampoco sacar elementos del arreglo
	return TRUE;
}

// setQuery1 actualiza los datos relaiconados al query 1
static void setQuery1(yearList current, enum titleType type){
	switch (type)
	{
		case MOVIE:
			current->nMovies += 1;  											// Si es una film, incrementa la cantidad de peliculas en ese año
			break;
		case SHORT:            													// Si es un short, incrementa la cantidad de cortos en ese año
			current->nShorts += 1;
			break;
		case TV_SERIES:        													// Si es una serie o una mini serie, incrementa la cantidad de series en ese año
		case TV_MINI_SERIES:
			current->nSeries += 1;
			break;
		default:																// Cualquier otro caso no hace nada
			;
	}
}

// updateRank actualiza la lista que contiene a los elementos de un ranking. En el caso de que se deba devolver el indice del elemento que esta de
static titleList updateRank(titleList first, pElement element, int (*compare) (titleADT t1, titleADT t2), int * flag, int * removeID){
	int c;																					// Constante para guardar el resultado de la funcion de comparacion
	if ((first == NULL) && (*flag == TRUE)){												// Llego al final y no hay se debe agregar un nuevo elemento a la lista
		*flag = FALSE;																		// No se agrego ningun elemento
		return first;																		// Retorna la lista sin modificar
	}
	if (first == NULL || ((c = compare(first->element->title, element->title)) <= 0)){		// Se llego al final de la lista o se cumple la condicion para agregar un nuevo elemento a la lista
		titleList new = malloc(sizeof(titleNode));											// Alocamiento de memoria
		if (new == NULL){																	// Error de alocamiento de memoria
			*flag = LIST_ALLOC_ERROR;														// Indicador de error
			return first;																	// Retorna la lista intacta para que pueda ser liberada si es necesario
		}	
		new->element = element;																// El nuevo nodo de la lista apunta al elemento que se quiere agregar
		new->nextTitle = first;																// Se encaden el resto de la lista con el elemento actual
		new->element->inUse += 1;															// Se incrementa el contador que indica cuantas queries utilizan el elemento
		if (*flag == TRUE){																	// El ranking tiene cantidad maxima de elementos
			*flag = FALSE;																	// No se debe incrementar la cantidad de elementos de la lista, por que solo se esta remplazando un elemento
			new->nextTitle = findLast(first, removeID);										// Se encuentra el ultimo elemento de la lista para desencadenarlo y liberarlo si es necsario
		}
		else
		{
			*flag = TRUE;																	// Se agrego un nuevo elemento a la lista
		}
		return new;																			// Se encadena el elemento actual con los elementos anteriores de la lista
	}
	if (c > 0){																				// Todavia no se llego al final de la lista ni se llego a la condicion para agregar un elemento a la lista
		first->nextTitle = updateRank(first->nextTitle, element, compare, flag, removeID);
	}
	return first;																			// Se encadena el elemento acutal a los elementos anteriores de la lista
}

// rankMaker prepara los datos que necesita updateRank para actualizar un ranking especifico
// Se encarga de ver si se llego a la maxima cantidad de elementos que puede tener el rank para indicarselo por medio del flag a updateRank
static int rankMaker(titleList * ranking, pElement element, unsigned int dim, int max, int (*compare) (titleADT t1, titleADT t2), int * removeID){
	int flag;
	if (dim == max){														// Se llego a la maxima cantidad de elementos que puede tener el ranking
		flag = TRUE;
	} else {																// Todavia no se llego a la maxima cantidad de elementos del ranking
		flag = FALSE;
	}
	*ranking = updateRank(*ranking, element, compare, &flag, removeID);		// Se llama a la funcion para modificar el ranking
	return flag;
}

// findLast encuentra el ultimo elemento del ranking
// En el caso de que el elemento no se utiliza en ningun otro ranking se retorna el indice del elemento por medio del flag
static titleList findLast(titleList list, int * flag){  
	if (list->nextTitle == NULL){	               	 	    				// Caso base: si el siguiente elemento al actual es NULL (no hay)
		list->element->inUse -= 1;                      					// Se le quita un uso al elemento
		if (list->element->inUse == 0){                 					// Y si se quedo sin usos
			*flag = list->element->id;                  					// Hay que eliminarlo
		} else {                                        					// Si tiene al menos un uso no lo elimino
			*flag = NO_ID;
		}
		free(list);															// Se libera el nodo
		return NULL;														// Se retorna NULL indicando el nuevo ultimo elemento
	}
	list->nextTitle = findLast(list->nextTitle, flag);						// No se encuentra el ultimo elemento, se avanza al priximo elemento de la lista
	return list;
}

// Las siguientes funciones setQuery son funciones intermediarias utilizadas para llamar a rankMaker
// Cada una llama a rankMaker pasandole por parametros los datos y listas pertenecientes al query especifico
// Ademas cada funcion se encarga de ver si se agrego un elemento a la lista, actualizando el contador si es necesario
// La funcion retorna TRUE si todo salio bien y FALSE si hubo algun error de alocamiento

// serQuery2
static int setQuery2(queriesADT queries, pElement element, int * removeID){
	int flag = 0;
	flag = rankMaker(&queries->topAnimatedFilms, element, queries->nTopAnimatedFilms, MAX_TOP_ANIMATED_FILMS, compareRatingVotes, removeID);
	queries->nTopAnimatedFilms += (flag == TRUE);
	return flag != LIST_ALLOC_ERROR;
}

// setQuery3
static int setQuery3(yearList currentYear, pElement element, int * removeID){
	int flag = 0;
	flag = rankMaker(&currentYear->topRanking, element, currentYear->rankingDim, MAX_TOP_YEAR, compareVotesNames, removeID);
	currentYear->rankingDim += (flag == TRUE);
	return flag != LIST_ALLOC_ERROR;
}

// serQuery4
static int setQuery4(queriesADT queries, pElement element, int * removeID){
	int flag = 0;
	flag = rankMaker(&queries->topSeries, element, queries->nTopSeries, MAX_TOP_SERIES, compareRatingVotes, removeID);
	queries->nTopSeries += (flag == TRUE);
	return flag != LIST_ALLOC_ERROR;
}

// serQuery5
// Hace una lista de elementos ordenada:
// Ascendente por puntaje, y a igualdad de puntaje ascendente por cantidad de votos.
static int setQuery5(queriesADT queries, pElement element, int * removeID){
	int flag = 0;
	flag = rankMaker(&queries->worstSeries, element, queries->nWorstSeries, MAX_WORST_SERIES, compareQ5, removeID);
	queries->nWorstSeries += (flag == TRUE);
	return flag != LIST_ALLOC_ERROR;
}

// Funcion de comparacion para los queries 2 y 4
// Esta funcion de comparacion compara primero por rating y a iguladad de rating, compara por numero de votos
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
// Esta funcion de comparacion compara por numero de votos y a igualdad de votos, compara por orden alfabetico
// Retorna:
// > 0 si t1 > t2
// < 0 si t1 < t2
static int compareVotesNames(titleADT t1, titleADT t2){
	int c;
	if ((c = compareNumVotes(t1, t2)) != 0){				// Comparacion por votos
		return c;
	} else {
		return compareTitleNames(t2, t1);					// Comparacion por nombre
	}
}

// Funcion de comparacion para el query 5
// La funcion de comparacion es opuesto al query 2 y 4
// Retorna:
// > 0 si t1 < t2
// < 0 si t1 > t2
static int compareQ5(titleADT t1, titleADT t2){
	return compareRatingVotes(t2, t1);						// Llamado de la funcion de comparacion del query 2 y query 4 pero con los titulos invertidos
}

// FUNCIONES DE RETORNO:
// returnCurrentYearQ1 retorna la cantidad de films, series y shorts en el año en el cual apunta el iterador.
// Funcion de retorno para el query 1
void returnCurrentYearQ1(queriesADT queries, unsigned int * year,unsigned int * nFilms, unsigned int * nSeries, unsigned int * nShorts){
	if(queries->yearIterator==NULL){
		return;
	}
	*year=queries->yearIterator->year;
	*nFilms = queries->yearIterator->nMovies; 				// Retorno de cantidad de films ese año
	*nSeries = queries->yearIterator->nSeries;				// Retorno de cantidad de series ese año
	*nShorts = queries->yearIterator->nShorts;				// Retorno de cantidad de shorts ese año
}

////////////////////////////////////////////////// Iteradores //////////////////////////////////////////////////

// Inicializa el iterador de años
void toBeginYears(queriesADT queries){
	queries->yearIterator = queries->firstYear;	
}

// Inicializa el iterador de las mejores peliculas del año encontrado en el iterador de años
void toBeginYearRankings(queriesADT queries){
	queries->yearRankingIter = queries->yearIterator->topRanking;
}

// Inicializa el iterador de mejores peliculas animadas
void toBeginTopAnimatedFilms(queriesADT queries){
	queries->topAnimatedFilmsIterator = queries->topAnimatedFilms;
}

// Inicializa el iterador de mejores series
void toBeginTopSeries(queriesADT queries){
	queries->topSeriesIterator = queries->topSeries;
}

// Inicializa el iterador de peores series
void toBeginWorstSeries(queriesADT queries){
	queries->worstSeriesIterator = queries->worstSeries;
}

// Funcion general que retorna si un titleList es null (No hay siguiente elemento en el iterador)
static int hasNext(titleList nTitle){
	return nTitle != NULL;
}

// Retorna si hay proximo elemento en el iterador de años
int hasNextYear(queriesADT queries){
	return queries->yearIterator != NULL;
}

// Retorna si hay proximo elemento en el iterador de mejores peliculas del año
int hasNextYearRanking(queriesADT queries){
	return hasNext(queries->yearRankingIter);
}

// Retorna si hay proximo elemento en el iterador de mejores peliculas animadas
int hasNextTopAnimatedFilms(queriesADT queries){
	return hasNext(queries->topAnimatedFilmsIterator);
}

// Retorna si hay proximo elemento en el iterador de mejores series
int hasNextTopSeries(queriesADT queries){
	return hasNext(queries->topSeriesIterator);
}

// Retorna si hay proximo elemento en el iterador de peores series
int hasNextWorstSeries(queriesADT queries){
	return hasNext(queries->worstSeriesIterator);
}

// nextItem copia en title el titulo del elemento actual del iterador ingresado nTitle
// Luego avanza el iterador al proximo elemento del la lista
static int nextItem(titleList * nTitle,titleADT title){
	int check = titleCopy(title,(*nTitle)->element->title);
	(*nTitle) = (*nTitle)->nextTitle;
	return check;
}

// Iterador de años.
int nextYear(queriesADT queries){
	if (hasNextYear(queries) && queries->yearIterator->nextYear != NULL){			// Verifica que haya un año actual
		queries->yearIterator = queries->yearIterator->nextYear;					// Avanza el iterador al proximo año
		return TRUE; 																// Retorna el puntero al elemento al año actual
	}
	else {
		return FALSE;																// Si no quedan mas años retorna FALSE
	}
}

// Iterador del top ranking de cada año (query3)
int nextYearRanking(queriesADT queries,titleADT title, int *flag){
	if (hasNext(queries->yearRankingIter)){											// Verifica que haya un elemento actual
		*flag = nextItem(&(queries->yearRankingIter),title);			
		return *flag;
	}
	return FALSE;
}

// Iterador para las mejores peliculas animadas (query 2)
int nextTopAnimatedFilms(queriesADT queries,titleADT title, int *flag){					
	if (hasNext(queries->topAnimatedFilmsIterator)){								// Verifica que haya un elemento actual
		*flag = nextItem(&(queries->topAnimatedFilmsIterator),title);			
		return *flag;
	}
	return FALSE;																	// Si no quedan mas elementos en el ranking retorna FALSE
}

// Iterador para las mejores series (query 4)
int nextTopSeries(queriesADT queries,titleADT title, int * flag){
		if (hasNext(queries->topSeriesIterator)){									// Verifica que haya un elemento actual
		*flag = nextItem(&(queries->topSeriesIterator),title);			
		return *flag;
	}
	return FALSE;
}

// Iterador para las peores series (query 5)
int nextWorstSeries(queriesADT queries, titleADT title, int *flag){
	if (hasNext(queries->worstSeriesIterator)){										// Verifica que haya un elemento actual
		*flag = nextItem(&(queries->worstSeriesIterator),title);			
		return *flag;
	}
	return FALSE;
}

// freeElement libera el elemento utilizado para guardar los titulos y su informacion
static void freeElement(pElement element)
{
	if(element != NULL){
		freeTitle(element->title);													// Libera el titulo
		free(element);																// Libera la esttructura
	}
}

// freeList libera las listas utilizadas para los ranking
// Notar que esta funcion de libera los contenidos de la lista ya que eso lo hace freeStorage
static void freeList(titleList list){
	if (list != NULL){
		freeList(list->nextTitle);													// Llamada recursiva para llegar hasta el final
		free(list);																	// Liber el puntero al nodo
	}
}

// freeYear libera la lista de años
static void freeYears(yearList year){
	if (year != NULL){
		freeYears(year->nextYear);													// Lamada recursiva para llegar al final
		freeList(year->topRanking);													// Llamada para liberar la lista utilizada para el ranking
		free(year);
	}
}

// freeStorage libera todos los elementos dentro del arreglo y el arreglo 
static void freeStorage(pElement * vec,unsigned int dim)
{
	for (int i = 0; i < dim; i++)
	{
		freeElement(vec[i]);														// Libera todos los elementos del arreglo
	}
	free(vec);																		// Libera el arreglo
}

void freeQueries(queriesADT queries){
	freeYears(queries->firstYear);													// Se libera la lista de años
	freeList(queries->topAnimatedFilms);											// Se libera el rankking de mejores peliculas animadas
	freeList(queries->topSeries);													// Se libera el ranking de mejores series
	freeList(queries->worstSeries);													// Se libera el ranking de peores series
	freeStorage(queries->storeData,queries->storageDim);							// Se libera el arreglo junto con todos los titulos que estan siendo almacenados
	freeElement(queries->currentElement);											// Se libara el currentElement si es necesario
	free(queries);																	// Se libara queries
}
