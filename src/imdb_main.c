#include "imdb_frontend.h"

#define MAX_ARG 4
#define MIN_ARG 2

static int validatePath(char * path);								// Función auxiliar que valida los paths  	

int main( int argc, char *argv[] )  {
	if( argc < MIN_ARG + 1 ) {                                  	// Se verifica que haya una mínima cantidad de argumentos
		fprintf(stderr, "Error: Muy pocos argumentos\n");
		return INPUT_ERROR;
	}
	else if( argc  > MAX_ARG + 1 ) {								// Se verifica que la cantidad de argumentosno supere la cantidad máxima
		fprintf(stderr, "Error: Demasiados argumentos\n");
		return EXIT_FAILURE;
	}

	if(validatePath(argv[1])==FALSE || validatePath(argv[2])==FALSE){
		fprintf(stderr, "Error: Paths inválidos\n");
		return INPUT_ERROR;
	}
	int check, yMin, yMax;
	int yearCheck = 1;
	if (argc == MIN_ARG + 1){                                       // Si no se pasan años como parametros, los inicializa ambos en cero
		yMin = 0;
		yMax = 0;
	}
	if (argc > MIN_ARG + 1){										// Verifica que se pase un año de inicio válido
		if ((yMin = atoi(argv[3])) <= 0){
			yearCheck = 0;
		}

	}
	if (argc == MAX_ARG + 1){										// Verifica que se pase un año de inicio válido
		yMax = atoi(argv[4]);
		if ((yMax = atoi(argv[4])) <= 0){
			yearCheck = 0;
		}
	}

	if (yearCheck == 0){											// Si alguno de los años es inválido, no se procesa la información
		fprintf(stderr, "Error: Los años deben ser numeros naturales\n");
		return INPUT_ERROR;
	}

	if((argc == MAX_ARG + 1) && yMax < yMin){												// Si alguno de los años es inválido, no se procesa la información
		fprintf(stderr, "Error: El año de inicio es mayor al de finalización\n");
		return INPUT_ERROR;
	}

	check = imdb_frontend_main(argv[1], argv[2], yMin, yMax);
	return (check == TRUE) ? EXIT_SUCCESS : check;						// Si check es TRUE significa que no ocurrio ningun error. En el caso contrario significa que ocurrio un error de alocamiento o al abrir un archivo. En estos dos casos check tiene el valor asociado a ese error
}

static int validatePath(char * path){ 								//Devuelve false si el path contiene un caracter invalido de los listados a continuacion, true en caso contrario (path valido)
    char notValid[] = {':', '*', '?', '\"', '<', '>', '|'}; 		//Estos son los caracteres no validos para un path. Notar que \ y / son validos (\ para Windows)
    if (*path == '\0'){ 												//Si se llego al final del string, devolver que el path es valido (caso base 1)
        return TRUE;
    }
    for (int i = 0 ; i < sizeof(notValid)/sizeof(notValid[0]) ; i++){ //Si se encontro un caracter invalido, devolver que el path es invalido (caso base 2)
        if (*path == notValid[i]){
            return FALSE;
        }
    }
    return validatePath(path + 1); 									//Si no sucedio nada de lo anterior, continuar leyendo el path por el siguiente caracter
}
