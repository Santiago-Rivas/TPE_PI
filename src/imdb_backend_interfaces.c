#include "imdb_backend_interfaces.h"
#include <stdio.h>
// INTERFACES:

// titleCDT es una estructura que guarda informacion sobre un titulo
// La informacion almacenada es utilizada para los queries y tambien para mejor funcionamiento del programa
typedef struct titleCDT {
	enum titleType type;				// Tipo del titulo
	char * primaryTitle;				// Nombre del titulo
	unsigned int titleLen;				// Longitud del nombre del titulo
	unsigned int maxLen;				// Espacio máximo alocado para el primaryTitle
	int startYear;						// Año cuando salio originalmente el titulo
	int endYear;						// Año en el cual la serie se dejo de publicar (para peliuclas y para series que siguen sacando nuevos capitulos se utiliza NO_YEAR)
	unsigned int genres[MAX_GENRES]; 	// Vector que contiene los indices de los generos del titulo en relacion a la estructura allGenres		
	unsigned int cantGenres;			// Cantidad de generos que tiene el ti
	float averageRating;				// RAting del titulo
	unsigned int numVotes;				// Cantidad de votos que tiene el titulo
	int isAnimation;					// TRUE si es animacion. FALSE si no es animacion
} titleCDT;

// Funcion que crea un nuevo titleADT
titleADT newTitle(void){
	titleADT new = calloc(1, sizeof(titleCDT));
	return new;
}

// titleCopy recibe dos titleADT e iguala el segundo al primero
// Para el primaryTitle se aloca nueva memoria para que cuando se libere un titulo el otro pueda seguir siendo utilizado
int titleCopy(titleADT t1, titleADT t2){
	t1->primaryTitle = realloc(t1->primaryTitle, t2->titleLen + 1);			// Alocamiento de memoria
	if (t1->primaryTitle == NULL){											// Error de alocamiento de memoria
		return FALSE;
	}
	strcpy(t1->primaryTitle, t2->primaryTitle);								// Se copia el primaryTitle

	// Se copian los demas campos del titleADT:
	t1->titleLen = t2->titleLen;
	for (int i = 0; i < t2->cantGenres; i++)
	{
		t1->genres[i]=t2->genres[i];
	}
	t1->cantGenres=t2->cantGenres;
	t1->type = t2->type;
	t1->titleLen = t2->titleLen;
	t1->averageRating = t2->averageRating;
	t1->numVotes = t2->numVotes;
	t1->startYear = t2->startYear;
	t1->endYear = t2->endYear;
	t1->isAnimation = t2->isAnimation;
	return TRUE;
}

// addGenres actualiza una lista de genreList
// Se ordenan de manera alfabetica descendentemente
genreList addGenres(genreList firstGenre, char * genreName, int * flag)
{
	int c;
	if(firstGenre == NULL || (c=strcmp(firstGenre->genre,genreName))>0){		// Condicion para agregar un nodo a la lista
		genreList new = malloc(sizeof(genreNode));								// Alocamiento de memoria
		if(new == NULL)															// Error de alocamiento
		{
			*flag=FALSE;														// Indicador de que ocurrio un error
			return firstGenre;													// Retorna la lista intacta de manera que se pueda liberar
		}
		new->genre=malloc((strlen(genreName) +1)*sizeof(char));					// Alocamiento de memoria para guardar el string que guarda el nombre del genero
		if(new->genre == NULL)													// Error de alocamiento
		{
			*flag=FALSE;														// Indicador de que ocurrio un error
			free(new);															// Se libera el espacio previamente reservado para el nodo
			return firstGenre;													// Retorna la lista intacta de manera que se pueda liberar
		}
		strcpy(new->genre,genreName);											// Se copia el nombre del genero al espacio reservado
		new->nextGenre=firstGenre;												// se encadena el siguiente elemento de la lista
		return new;																// Se encadena el elemento actual de la lista con los elementos anteriores
	}
	if(c < 0)																	// Condicion para seguir avanzando en la lista		
	{
		firstGenre->nextGenre=addGenres(firstGenre->nextGenre,genreName, flag);	// Llamada recursiva de la funcion para seguir buscando el lugar correcto para insertar el elemento a la lista
	}
	return firstGenre;															// Se encadena el elemento actual de la lista con los elementos anteriores
}

// freeGenreList es una funcion que libera la memoria de listas de genreList
void freeGenreList(genreList list){
	if(list != NULL){								// Condicion para seguir avanzando
		freeGenreList(list->nextGenre);				// Llamada recursiva para llegar al final de la lista
		free(list->genre);							// Se libera el string que contiene el nombre del genero
		free(list);									// Se libera el elemento actual de la lista
	}
}

// stringCompare compara dos strings ignorando mayusculas 
int stringCompare(char * str1, char * str2){
    int c = tolower(*str1) - tolower(*str2);				// Resta entre dos letras minusculas
	if (*str1 != 0 && *str2 != 0 && c == 0){				// Ninguno de los strings se terminaron y las letras analizadas en esta recursion son la misma
		return stringCompare(str1+1, str2+1);				// LLamada recursiva de la funcion para comparar las proximas letras
	}
	return c;												// Retorna la resta entre las letras que no son iguales
}

// Funciones para modificar los campos de un titleADT:

// setGenres es una funcion que recibe un titulo, los generos validos y una lista de generos pertenecientes al titulo actual
// La funcion actualiza el titulo para que contenga los generos a los cual pertenece
// Los generos a los cual un titulo pertenece son almacenados en un arreglo estatico con un tamaño de MAX_GENRES
// El arreglo guarda los indices de los generos al cual un titulo pertenece dentro del arreglo de nombres de generos validos de allGenres
void setGenres(titleADT title, allGenres * genres, genreList titleGenres){
	int dim = 0;																		// Cantidad de generos agregados
	int i = 0;																			// Indice que indica que genero valido se esta analizando
	int c;																				// Constante para guardar el resultado de comparaciones
	while (dim<MAX_GENRES && i < MAX_GENRES && i<genres->dim && titleGenres != NULL){	// Ciclo hasta que no hayan mas genereros validos para analizar o hasta que no hayan mas generos pertenecientes al titulo
		if ((c = stringCompare(genres->genresName[i], titleGenres->genre)) == 0){		// Se enceuntra un genero valido dentro de la lista de generos pertenecientes a un titulo
			if (stringCompare(titleGenres->genre, "animation") == 0){					// El titulo pertenece al genero Animation
				title->isAnimation = TRUE;												// Se actualiza el campo para indicar que el titulo es una animacion
			}
			title->genres[dim] = i;														// En el arreglo de 
			dim++;																		// Se incrementa el contador de generos agregados
			i++;																		// Se incrementa el indice para analizar el proximo genero valido
			titleGenres = titleGenres->nextGenre;										// Se avanza al proximo elemento de genero de la lista de generos del titulo
		}
		else if (c > 0){																// El genero del titulo es menor alfabeticamente que el titulo actual de los generos validos
			titleGenres = titleGenres->nextGenre;										// Se avanza al proximo elemento de genero de la lista de generos del titulo
		}
		else {																			// El genero del titulo es mayor alfabeticamente que el titulo actual de los generos validos
			i++;																		// Se incrementa el indice para analizar el proximo genero valido
		}
	}
	title->cantGenres = dim;															// Se actualiza el numero de generos valido que tiene un titulo
}

// setTitleName actualiza el campo primaryTitle, alocando la memoria necesaria para gaurdar el nombre del titulo
int setTitleName(titleADT title, char * str){
	int dim = strlen(str);															// Se encuentra la longitud del nombre del titulo
	if (dim > title->maxLen){
		title->primaryTitle = realloc(title->primaryTitle, (dim + 1) * sizeof(char));	// Se aloca la memoria necesaria para copiar el nombre del titulo
		title->maxLen = dim;
	}
	if (title->primaryTitle == NULL){												// Error de alocamiento
		return FALSE;
	}
	strcpy(title->primaryTitle, str);												// Se copia el nombre del titulo al campo de la estructura titleADT
	title->titleLen = dim;															// Se guarda la longitud del nombre del titulo en el titleADT
	return TRUE;
}

// setTitleType actualiza el campo type del titleADT ingresado
void setTitleType(titleADT title, enum titleType type){
	title->type = type;
}

// setStartYear actualiza el campo startYear del titleADT ingresado
void setStartYear(titleADT title, int year){
	title->startYear = year;
}

// setEndYear actualiza el campo startYear del titleADT ingresado
void setEndYear(titleADT title, int year){
	title->endYear = year;
}

// setRaintg actualiza el campo averageRating del titleADT ingresado
void setRating(titleADT title, float rating){
	title->averageRating = rating;
}

// setVotes actualiza el campo numVotes del titleADT ingresado
void setVotes(titleADT title, unsigned int votes){
	title->numVotes = votes;
}

// setFalseAnimation cambia el valor del campo isAnimation del titleADT ingresado a FALSE
void setFalseAnimation(titleADT title){
	title->isAnimation = FALSE;
}


// Funciones de retorno:

// returnTitleName retorna en un string ingresado como segundo argumento el campo primaryTitle
int returnTitleName(titleADT title, char ** str){
	*str = realloc(*str, title->titleLen + 1);				// Realoca memoria para almacenar el nombre del titulo
	if (*str == NULL){										// Error de alocamiento de memoria
		return FALSE;
	}
	strcpy(*str, title->primaryTitle);						// Copia el nombre del titulo al string ingresado como argumento
	return TRUE;
}

// returnType retorna el campo type del titleADT ingresado
unsigned int returnType(titleADT title){
	return title->type;
}

// returnStartYear retorna el campo startYear del titleADT ingresado
int returnStartYear(titleADT title){
	return title->startYear;
}

// returnEndYear retorna el campo endYear del titleADT ingresado
int returnEndYear(titleADT title){
	return title->endYear;
}

// returnRating retorna el campo averageRating del titleADT ingresado
float returnRating(titleADT title){
	return title->averageRating;
}

// returnVotes retorna el campo numVotes del titleADT ingresado
unsigned int returnVotes(titleADT title){
	return title->numVotes;
}

// returnGenCount retorna el numero de generos valido del titleADT ingresado
unsigned int returnGenCount(titleADT title){
	return title->cantGenres;
}

// returnIsAnimation retorna el campo isAnimation del titleADT ingresado
unsigned int returnIsAnimation(titleADT title){
	return title->isAnimation;
}

// returnGenre retorna el indice de un genero valido dentro de allGenres
// El indice que retorna es el que esta en la posicion index del arreglo genres dentro del titleADT ingresado
int returnGenre(titleADT title, unsigned int index){
	if (index >= title->cantGenres){						// Se ingreso un index mayor a la cantidad de generos validos pertenecientes a un titulo
		return INVALID_INDEX;
	}
	return title->genres[index];
}

// Funciones de comparacion:

// Copmaracion entre el nombre de dos titulos
int compareTitleNames(titleADT t1, titleADT t2){
	return strcmp(t1->primaryTitle, t2->primaryTitle);
}

// Comparacion entre la cantidad de votos de dos titulos
int compareNumVotes(titleADT t1, titleADT t2){
	return t1->numVotes - t2->numVotes;
}

// Copmaracion entre el rating de dos titulos
float compareRating(titleADT t1, titleADT t2){
    return t1->averageRating - t2->averageRating;
}

// Funcion de liberacion:
// freeTitle libera el espacio reservado de memoria del titleADT ingresado
void freeTitle(titleADT title){
	if (title !=NULL)
	{
		if (title->primaryTitle!=NULL)						// En el caso de que haya memoria reservada para el nombre del titulo, liberar el espacio de memoria
		{
			free(title->primaryTitle);
		}
		free(title);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
