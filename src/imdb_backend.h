#ifndef IMDB_BACKEND
#define IMDB_BACKEND

#include "imdb_backend_interfaces.h"

#define ZERO 0.0001   		            // Los resultados se van a devolver con dos puntos decimales
#define POSITIVE 1                      // Para indicar valores positivos
#define NEGATIVE -1                     // Para indicar valores negativos
#define MAX_TOP_YEAR 100	            // Cantidad de titulo en el ranking del query 3
#define MAX_TOP_ANIMATED_FILMS 500	    // Cantidad de titulos en el ranking del query 4
#define MAX_TOP_SERIES 250	            // Cantidad de titulos en el ranking del query 5
#define MAX_WORST_SERIES 50	            // Cantidad de titulos en el ranking del query 6
#define MIN_VOTES_Q2 20000              // Numero minimo de votos para que un titulo sea considerado en el query 2
#define MIN_VOTES_Q4 50000              // Numero minimo de votos para que un titulo sea considerado en el query 4
#define MIN_VOTES_Q5 5000               // Numero minimo de votos para que un titulo sea considerado en el query 5

typedef struct queriesCDT * queriesADT;

// newQueries reserva memoria para la estructura queriesADT
// Devuelve: 
// queriesADT:Un puntero a la estructura, que apunta a una zona de memoria valida
queriesADT newQueries(int yMin, int yMax);

// processData se encarga de procesar cada linea leída del archivo
// Recibe:
// queries: Un puntero a la estructura donde se almacena toda la información necesaria para las queries
// title: Un puntero al ADT de la obra
// yearLowerLimit: Representa el año minimo (para los queries 5 y 6)
// yearLowerLimit: Representa el año máximo (para los queries 5 y 6)
int processData(queriesADT queries, titleADT title, genreList titleGenres, allGenres * validGenres);

// ITERADORES:
// toBeginYears hace apuntar el iterador de años al primer año de la lista
// Recibe:
// queries:Un puntero a la estructura donde se almacena toda la información necesaria para las queries
void toBeginYears(queriesADT queries);

// toBeginYearRanking hace apuntar el iterador del ranking por año al primer año de la lista
// Recibe:
// queries:Un puntero a la estructura donde se almacena toda la información necesaria para las queries
void toBeginYearRankings(queriesADT queries);

// toBeginTopFilms hace apuntar el iterador de  mejores peliculas a la primer pelicula de la lista.
// Recibe:
// queries:Un puntero a la estructura donde se almacena toda la información necesaria para las queries
void toBeginTopAnimatedFilms(queriesADT queries);

// toBeginTopSeries hace apuntar el iterador de mejores series a la primer serie de la lista.
// Recibe:
// queries:Un puntero a la estructura donde se almacena toda la información necesaria para las queries
void toBeginTopSeries(queriesADT queries);

// toBeginWorstSeries hace apuntar el iterador de peores series a la primer serie de la lista
// Recibe:
// queries:Un puntero a la estructura donde se almacena toda la información necesaria para las queries
void toBeginWorstSeries(queriesADT queries);

// hasNextYear se fija si hay un próximo elemento en la lista de años
// Recibe:
// queries:Un puntero a la estructura donde se almacena toda la información necesaria para las queries
// Devuelve:
// Un 1 si hay un próximo elemento, 0 si no hay
int hasNextYear(queriesADT queries);

// hasNextYearRanking se fija si hay un próximo elemento en el iterador de ranking por año 
// Recibe:
// queries:Un puntero a la estructura donde se almacena toda la información necesaria para las queries
// Devuelve:
// Un 1 si hay un próximo elemento, 0 si no hay
int hasNextYearRanking(queriesADT queries);

// hasNextTopAnimatedFilms se fija si hay un próximo elemento en el iterador de ranking de peliculas animadas
// Recibe:
// queries:Un puntero a la estructura donde se almacena toda la información necesaria para las queries
// Devuelve:
// Un 1 si hay un próximo elemento, 0 si no hay
int hasNextTopAnimatedFilms(queriesADT queries);

// hasNextTopSeries se fija si hay un próximo elemento en el iterador de mejores series
// Recibe:
// queries:Un puntero a la estructura donde se almacena toda la información necesaria para las queries
// Devuelve:
// Un 1 si hay un próximo elemento, 0 si no hay
int hasNextTopSeries(queriesADT queries);

// hasNextWorstSeries se fija si hay un próximo elemento en el iterador de de peores series
// Recibe:
// queries:Un puntero a la estructura donde se almacena toda la información necesaria para las queries
// Devuelve:
// Un 1 si hay un próximo elemento, 0 si no hay
int hasNextWorstSeries(queriesADT queries);

// Iterador de años.
// nextYear devuelve si hay un proximo año
// Recibe:
// queries: Un puntero a la estructura donde se almacena toda la información necesaria para las queries
// Devuelve:
// Un 1 si hay un proximo año, 0 si no
int nextYear(queriesADT queries);

// Iterador de las mejores peliculas del año en el cual se esta iterando
// nextYearranking pone en title el elemento al que apunta el iterador de mejores peliculas por año
// Recibe:
// queries: Un puntero a la estructura donde se almacena toda la información necesaria para las queries
// title: Un puntero al ADT de la obra
// Devuelve:
// Un 1 si lo pudo agregar, 0 si no pudo
int nextYearRanking(queriesADT queries, titleADT title, int * flag);

// Iterador para las mejores peliculas (query 4)
// nextTopFilms pone en title el elemento al que apunta el iterador de mejores peliculas
// Recibe:
// queries: Un puntero a la estructura donde se almacena toda la información necesaria para las queries
// title: Un puntero al ADT de la obra
// Devuelve:
// Un 1 si lo pudo agregar, 0 si no pudo
int nextTopAnimatedFilms(queriesADT queries, titleADT title, int * flag);

// Iterador para las mejores series (query 5)
// nextTopSeries pone en title el elemento al que apunta el iterador de mejores series
// Recibe:
// queries: Un puntero a la estructura donde se almacena toda la información necesaria para las queries
// title: Un puntero al ADT de la obra
// Devuelve:
// Un 1 si lo pudo agregar, 0 si no pudo
int nextTopSeries(queriesADT queries, titleADT title, int * flag);

// Iterador para las peores series (query 6)
// nextWorstSeries pone en title el elemento al que apunta el iterador de peores series
// Recibe:
// queries: Un puntero a la estructura donde se almacena toda la información necesaria para las queries
// title: Un puntero al ADT de la obra
// Devuelve:
// Un 1 si lo pudo agregar, 0 si no pudo
int nextWorstSeries(queriesADT queries, titleADT title,int * flag);

// FUNCIONES DE RETORNO:
// Funcion de retorno para el query 1
// returnCurrentYearQ1 retorna la cantidad de films, series y shorts en el año en el cual apunta el iterador.
// Recibe:
// queries: Un puntero a la estructura donde se almacena toda la información necesaria para las queries
// year: Es el año al que apunta el iterador de años
// nFilms: Variable de entrada/salida donde se guarda la cantidad de peliculas en el año al que apunta el iterador
// nSeries: Variable de entrada/salida donde se guarda la cantidad de series en el año al que apunta el iterador
// nShorts: Variable de entrada/salida donde se guarda la cantidad de cortos en el año al que apunta el iterador
void returnCurrentYearQ1(queriesADT queries,unsigned int * year, unsigned int * nFilms, unsigned int * nSeries, unsigned int * nShorts);


// freeQueries libera todos los datos almacenados dentro de la estructura queries
// Recibe: 
// queries: Un puntero a la estructura donde se almacena toda la información necesaria para las queries
void freeQueries(queriesADT queries);

#endif //IMDB_BACKEND
