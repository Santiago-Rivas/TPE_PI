#ifndef IMDB_FRONTEND
#define IMDB_FRONTEND

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 

#include "imdb_backend_interfaces.h"
#include "imdb_backend.h"


// Errores posibles:
#define INPUT_ERROR 1				// Error provocado por argumentos invalidos al programa
#define ALLOC_ERROR 2				// Error provocado al alocar memoria
#define TITLE_FILE_ERROR 3			// Error provocado al abrir algun archivo
#define GENRES_FILE_ERROR 4			// Error provocado al abrir algun archivo
#define RETURN_FILE_ERROR 5			// Error provocado al abrir algun archivo

#define SEPARATOR ";"				// Separador utilizado en los archivos csv

#define TITLE_LINE_MAX_CHARS 500		// Cantidad maxima de caracteres leidos por linea en la lectura del archivo de titulos
#define GEN_LINE_MAX_CHARS 70			// Cantidad maxima de caracteres leido por linea en la lectura del archivo de titulos

// Se le otorga un indice a cada una de la celdas que se van a leer en el archivo de titulos.
enum fieldOrder {ID = 0,
                TITLE_TYPE, 
                PRIMARY_TITLE,
                START_YEAR,
                END_YEAR,
                GENRES,
                AVERAGE_RATING,
                NUM_VOTES,
                RUNTIME_MINUTES};


// Indicadores de que no hay informacion en la es celda especifica
#define NO_ELEM_STR1 "\\n"
#define NO_ELEM_STR2 "\\N"

#define TOTAL_QUERY_NUMBER 5			// Cantidad de queries que se van a evaluar e imprimir en diferentes archivos

enum queries {Q1 = 0,Q2,Q3,Q4,Q5};		// Indice orotgado a cada query

// Nombre de los archivos de retorno
#define RETURN_FILE_NAMES {"query1.csv",\
	"query2.csv", \
        "query3.csv", \
	"query4.csv", \
	"query5.csv"}

// Headers para cada uno de los archivos de retorno.
// Estan ordenados segun el enum.
#define RETURN_FILE_HEADERS {"year;films;series;shorts",\
	"year;film;votes;rating;genres", \
        "year;film;votes;rating;genres", \
	"startYear;endYear;series;votes;rating", \
	"startYear;endYear;series;votes;rating;genres"}

// imdb_frontend_main es una funcion llamada po main
// Parametros de entrada:
// 	titlePath: Path del archivo de titulos.
// 	genresPath: Path del archivo de generos
//	yMin: Año minmo en el cual se consideraran series para el query 4 y 5
//	yMax: Año maxim en el cual se consideraran series para el query 4 y 5
//	El año NO_YEAR indica que no se ingreso el parametro cuando se llamo al programa
// La funcion retorna:
// 	TRUE: Si salio todo bien
// 	ALLOC_ERROR: Si ocurrio un error de alocamiento.
// 	TITLE_FILE_ERROR: Si ocurrio un error al abrir el archivo de obras.
// 	GENRES_FILE_ERROR: Si ocurrio un error al abrir el archivo de géneros.
// 	RETURN_FILE_ERROR: Si ocurrio un error al abrir alguno de los archivos de retorno.
// Si alguno de los archivo de retorno se logro abrir, este sera llenado con el query correspondiente.
int imdb_frontend_main(char * titlePath, char * genresPath, unsigned int yMin, unsigned int yMax);


#endif //IMDB_FRONTEND
