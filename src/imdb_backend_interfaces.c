#include "imdb_backend_interfaces.h"
#include <stdio.h>
// INTERFACES:

typedef struct titleCDT {
	enum titleType type;		// Tipo del titulo
	char * primaryTitle;		// Nombre del titulo
	unsigned int titleLen;		// Longitud del nombre del titulo
	int startYear;				// Año cuando salio originalmente el titulo
	int endYear;				// Año en el cual la serie se dejo de publicar (no aplica a peliculas)
	unsigned int genres[MAX_GENRES]; // Vector que contiene los indices de los generos del titulo en relacion a la estructura allGenres		
	unsigned int cantGenres;	// Cantidad de generos que tiene el ti
	float averageRating;		// RAting del titulo
	unsigned int numVotes;		// Cantidad de votos que tiene el titulo

	unsigned int isAnimation;
} titleCDT;

titleADT newTitle(void){
	titleADT new = calloc(1, sizeof(titleCDT));
	return new;
}

/*
static int genresToVec(allGenres * genres, genreList titleGenres, unsigned int ** vec){
	int dim = 0;
	int i = 0;
	int c;
	while (i < genres->dim && titleGenres != NULL){
		if ((c = strcmp(genres->genresName[i], titleGenres->genre) == 0)){
			if (dim % BLOCK == 0){
				*vec = realloc(*vec, (dim + BLOCK) * sizeof(unsigned int));
				if (*vec == NULL){
					return ADD_GENRE_ERROR;
				}
			}
			*vec[dim] = i;
			(dim)++;
			i++;
			titleGenres = titleGenres->nextGenre;
		}
		else if (c > 0){
			titleGenres = titleGenres->nextGenre;
		}
		else {
			i++;
		}
	}
	*vec = realloc(*vec, (dim) * sizeof(unsigned int));
	return dim;
}
*/
/*
// La podemos llamar createValidTitle porque crea un titulo que cumple con las condiciones de lo queries
titleADT createTitle(char * titleName, enum titleType type, char * startYear, char * endYear, allGenres * genres, genreList titleGenres, float rating, int votes, int * flag){
	if (votes == 0 || strcmp(startYear, "\\N") == 0){			// Verificar la segunda opcion
		*flag = 1;
		return NULL;
	}
	titleADT new = malloc(sizeof(titleCDT));
	if (new == NULL){
		*flag = 0;
		return NULL;
	}
	new->type = type;
	new->titleLen = strlen(titleName);
	new->primaryTitle = malloc(new->titleLen + 1);
	if (new->primaryTitle == NULL){
		*flag = 0;
		free(new);
		return NULL;
	}
	strcpy(new->primaryTitle, titleName);
	new->startYear = atoi(startYear);
	new->endYear = atoi(endYear);

	int check;
	new->genres = genresToVec(genres, titleGenres, &check);
	if (check == ADD_GENRE_ERROR){
		*flag = 0;
		free(new);
		return NULL;
	}
	new->cantGenres = check;
	new->averageRating = rating;
	new->numVotes = votes;
    return new;
}
*/
int titleCopy(titleADT t1, titleADT t2){
	t1->primaryTitle = realloc(t1->primaryTitle, t2->titleLen + 1);
	if (t1->primaryTitle == NULL){
		return 0;
	}
	strcpy(t1->primaryTitle, t2->primaryTitle);
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
	return 1;
}

genreList addGenres(genreList firstGenre, char * genreName, int * flag)
{
	int c;
	if(firstGenre == NULL || (c=strcmp(firstGenre->genre,genreName))>0){
		genreList new = malloc(sizeof(genreNode));
		if(new == NULL)
		{
			*flag=FALSE;
			return firstGenre;
		}
		new->genre=malloc((strlen(genreName) +1)*sizeof(char));
		if(new->genre == NULL)
		{
			*flag=0;
			free(new);
			return firstGenre;
		}
		strcpy(new->genre,genreName);
		new->nextGenre=firstGenre;
		return new;
	}

	if(c<0)
	{
		firstGenre->nextGenre=addGenres(firstGenre->nextGenre,genreName, flag);
	}
	return firstGenre;
}

void freeGenreList(genreList list){
	if(list != NULL){
		freeGenreList(list->nextGenre);
		free(list);
	}
}

void setGenres(titleADT title, allGenres * genres, genreList titleGenres){
	int dim = 0;
	int i = 0;
	int c;
	while (dim<MAX_GENRES && i < MAX_GENRES && i<genres->dim && titleGenres != NULL){
		if ((c = strcmp(genres->genresName[i], titleGenres->genre)) == 0){
			if (strcmp(titleGenres->genre, "animation") == 0){
				title->isAnimation = TRUE;
			}
			title->genres[dim] = i;
			dim++;
			i++;
			titleGenres = titleGenres->nextGenre;
		}
		else if (c > 0){
			titleGenres = titleGenres->nextGenre;
		}
		else {
			i++;
		}
	}
	title->cantGenres = dim;
}

int setTitleName(titleADT title, char * str){
	int dim = strlen(str);
	title->primaryTitle = realloc(title->primaryTitle, (dim + 1) * sizeof(char));
	if (title->primaryTitle == NULL){
		return 0;
	}
	strcpy(title->primaryTitle, str);
	title->titleLen = dim;
	return 1;
}

void setTitleType(titleADT title, enum titleType type){
	title->type = type;
}

void setStartYear(titleADT title, int year){
	title->startYear = year;
}

void setEndYear(titleADT title, int year){
	title->endYear = year;
}

void setRanking(titleADT title, float ranking){
	title->averageRating = ranking;
}

void setVotes(titleADT title, unsigned int votes){
	title->numVotes = votes;
}



///////


int returnTitleName(titleADT title, char ** str){
	*str = realloc(*str, title->titleLen + 1);
	if (*str == NULL){
		return 0;
	}
	strcpy(*str, title->primaryTitle);
	return 1;
}

unsigned int returnType(titleADT title){
	return title->type;
}

int returnStartYear(titleADT title){
	return title->startYear;
}

int returnEndYear(titleADT title){
	return title->endYear;
}

float returnRating(titleADT title){
	return title->averageRating;
}

unsigned int returnVotes(titleADT title){
	return title->numVotes;
}

unsigned int returnGenCount(titleADT title){
	return title->cantGenres;
}

unsigned int returnIsAnimation(titleADT title){
	return title->isAnimation;
}

int returnGenre(titleADT title, unsigned int index){
	if (index >= title->cantGenres){
		return INVALID_INDEX;
	}
	return title->genres[index];
}


int compareTitleNames(titleADT t1, titleADT t2){
	return strcmp(t1->primaryTitle, t2->primaryTitle);
}

int compareNumVotes(titleADT t1, titleADT t2){
	return t1->numVotes - t2->numVotes;
}

float compareRating(titleADT t1, titleADT t2){
    return t1->averageRating - t2->averageRating;
}

void freeTitle(titleADT title){
	if (title->primaryTitle!=NULL)
	{
		free(title->primaryTitle);
	}
	
	free(title);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
