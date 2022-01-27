#ifndef IMDB_FRONTEND
#define IMDB_FRONTEND

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <ctype.h>  
#include "imdb_backend_interfaces.h"

#define TRUE 1
#define FALSE 0
#define GEN_LINE_MAX_CHARS 70

int imdb_frontend_main(char * moviePath, char * genresPath, int yMin, int yMax);


#endif //IMDB_FRONTEND
