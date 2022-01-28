#ifndef IMDB_FRONTEND
#define IMDB_FRONTEND

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <ctype.h>  
#include "imdb_backend_interfaces.h"
#include "imdb_backend.h"

#define TITLE_LINE_MAX_CHARS 500
#define GEN_LINE_MAX_CHARS 70

#define NO_ELEM_STR1 "\\n"
#define NO_ELEM_STR2 "\\N"

enum fieldOrder {ID = 0,
                TITLE_TYPE, 
                PRIMARY_TITLE,
                START_YEAR,
                END_YEAR,
                GENRES,
                AVERAGE_RATING,
                NUM_VOTES,
                RUNTIME_MINUTES};

int imdb_frontend_main(char * moviePath, char * genresPath, unsigned int yMin, unsigned int yMax);


#endif //IMDB_FRONTEND
