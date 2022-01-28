#ifndef IMDB_BACKEND_INTERFACES
#define IMDB_BACKEND_INTERFACES

#include <stdlib.h>
#include <string.h>

#define MAX_GENRES 32 		// Cantidad maxima de generos que se evaluaran
#define BLOCK 5
#define ADD_GENRE_ERROR -1  // Error cuando se agregan los generos a un titulo
#define NO_YEAR -1			// Constante que se usa cuando no se especifica un año
#define DUPLICATE_TITLE_ERROR -1
#define NEW_TITLE_NODE_ERROR -1
#define COPY_GEN_VEC_ERROR -1
#define COPY_STRING_ERROR -1
#define INVALID_INDEX -1
#define INVALID_TYPE -1
#define NO_YEAR_LIMIT -1

#define TRUE 1
#define FALSE 0

enum titleType {MOVIE = 0, SHORT, TV_SERIES, TV_MINI_SERIES}; 	// Enum con los tipos de titulos que se pueden tener

typedef struct allGenres {		//Estructura con todos los generos
	char ** genresName;			//Vector de strings de los generos
	unsigned int * nameLengths;
	unsigned int dim;			//Dimension del vector
} allGenres;

typedef struct genreNode * genreList;	//Lista de generos de un titulo

typedef struct genreNode{	//Nodo de la lista de generos
	char * genre;			//String de genero
	genreList nextGenre;	//Puntero al siguiente nodo
}genreNode;

typedef struct titleCDT * titleADT;	//ADT de obra

titleADT newTitle(void);	//Crea una nueva estructura para pasar los datos a backend

//titleADT createTitle(char * titleName, enum titleType type, char * startYear, char * endYear, allGenres * genres, genreList titleGenres, float rating, int votes, int * flag);

// titleCopy copia todos los elementos del segundo titleADT en el primer titleADT
int titleCopy(titleADT t1, titleADT t2);


genreList addGenres(genreList firstGenre, char * genreName, int * flag);

void freeList(genreList list);

// setGenres llena un vector con indices que indican que generos tiene un determinado titulo
// Recibe:
// 	titleADT title: Puntero a estructura donde se van a efectuar los cambios necesarios
// 	allGenres: Vector con todos los generos validos
// 	titleGenres: Lista con los generos pertenecientes al titulo
void setGenres(titleADT title, allGenres * genres, genreList titleGenres);

// setTitleName cambia el nombre del titulo
// Recibe:
// 	titleADT title: Puntero a estructura donde se van a efectuar los cambios necesarios
//  str: string con el nuevo nombre del titulo
// Devuelve:
//  0 ante un error de reserva de memoria
//  1 si hubo exito en la operacion
int setTitleName(titleADT title, char * str);

// setTitleType cambia el tipo del titulo ingresado
// recibe:
// 	titleADT: Puntero a estructura donde se van a efectuar los cambios necesarios
//  enum titleType: Valores posibles {MOVIE = 0, SHORT, TV_SERIES, TV_MINI_SERIES}
void setTitleType(titleADT title, enum titleType type);

void setStartYear(titleADT title, int year);

void setEndYear(titleADT title, int year);

// setRanking cambia el valor de ranking del titulo
// Recibe:
// 	titleADT: Puntero a estructura donde se van a efectuar los cambios necesarios
//  float ranking: Nuevo valor de ranking
void setRanking(titleADT title, float ranking);

// setVotes cambia el valor de votos del titulo
// Recibe:
// 	titleADT: Puntero a estructura donde se van a efectuar los cambios necesarios
//  vote: Nuevo valor de votos
void setVotes(titleADT title, unsigned int votes);

// returnTitleName copia en str el nombre del titleADT title
// Recibe:
// 	titleADT: Puntero a estructura donde se van a obtener los datos necesarios
//  str: puntero a string donde se devolvera el nombre pedido
// Retorna:
// 	1 si salio todo bien
// 	0 si hubo algun error
int returnTitleName(titleADT title, char ** str);

// returnType retorna el tipo del titleADT title
// Recibe:
// 	titleADT: Puntero a estructura donde se van a obtener los datos necesarios
// Retorna:
// 	enum titleType: Valores posibles {MOVIE = 0, SHORT, TV_SERIES, TV_MINI_SERIES} 
unsigned int returnType(titleADT title);

// returnStartYear retorna el año en el que salio el titulo
// Recibe:
// 	titleADT: Puntero a estructura donde se van a obtener los datos necesarios
// Retorna:
// 	Un año (entero)
int returnStartYear(titleADT title);

// returnEndYear retorna el año en el que la serie termino de publicarse
// Recibe:
// 	titleADT: Puntero a estructura donde se van a obtener los datos necesarios
// Retorna:
// 	Un año (entero)
int returnEndYear(titleADT title);

// returnRating retorna el rating del titulo
// Recibe:
// 	titleADT: Puntero a estructura donde se van a obtener los datos necesarios
// Retorna:
// 	float entre 0 y 10 que indica el rating de la serie
float returnRating(titleADT title);

// returnVotes retorna la cantidad de votos de un titulo
// Recibe:
// 	titleADT: Puntero a estructura donde se van a obtener los datos necesarios
// Retorna:
// 	Cantidad de votos de un determinado titulo (unsigned int)
unsigned int returnVotes(titleADT title);

// returnGenCount retorna la cantidad de generos de un titulo
// Recibe:
// 	titleADT: Puntero a estructura donde se van a obtener los datos necesarios
// Retorna:
// 	Cantidad de generos de un determinado titulo (unsigned int)
unsigned int returnGenCount(titleADT title);

// returnGenres llena un arreglo con los indices de los generos validos
// Recibe:
// 	titleADT: Puntero a estructura donde se van a obtener los datos necesarios
//  vec: arreglo que sera almacenado con los indices de los generos correspondientes al vector de la estructura allGenres
// Retorna:
// 	dimension del arreglo si salio todo bien
// 	COPY_GEN_VEC_ERROR si algo salio mal
int returnGenre(titleADT title, unsigned int index);

// Funcion de comparacion de nombre de titulos
// Recibe:
// 	Dos titleADT para ser comparados
// Retorna:
// > 0 si t1 > t2
// < 0 si t1 < t2
int compareTitleNames(titleADT t1, titleADT t2);

// Funcion de comparacion de cantidad de votos de titulos
// Recibe:
// 	Dos titleADT para ser comparados
// Retorna:
// > 0 si t1 > t2
// < 0 si t1 < t2
int compareNumVotes(titleADT t1, titleADT t2);

// Funcion de comparacion del rating de titulos titulos
// Recibe:
// 	Dos titleADT para ser comparados
// Retorna:
// > 0 si t1 > t2
// < 0 si t1 < t2
float compareRating(titleADT t1, titleADT t2);

// Libera un titleADT
void freeTitle(titleADT title);

#endif // IMDB_BACKEND_INTERFACES

