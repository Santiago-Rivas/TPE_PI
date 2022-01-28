# makefile TPE_PI
# Usar make para construir el proyecto
# Usar make clean para eliminar los archivos objeto luego de compilar

#ejecutable
imdb: imdb_main.o imdb_frontend.o  imdb_backend.o imdb_backend_interfaces.o
	gcc -Wall -pedantic -std=c99 -fsanitize=address -g -o imdb imdb_main.o imdb_frontend.o imdb_backend.o imdb_backend_interfaces.o 

#main
imdb_main.o:
	gcc -c -Wall -pedantic -std=c99 -fsanitize=address -g src/imdb_main.c

#frontend
imdb_frontend.o:
	gcc -c -Wall -pedantic -std=c99 -fsanitize=address -g src/imdb_frontend.c

#backend
imdb_backend.o:
	gcc -c -Wall -pedantic -std=c99 -fsanitize=address -g src/imdb_backend.c

#interfaces
imdb_backend_interfaces.o:
	gcc -c -Wall -pedantic -std=c99 -fsanitize=address -g src/imdb_backend_interfaces.c

################
clean:
	rm *.o
