# TPE_PI
Trabajo Práctico Final - Programación Imperativa - ITBA - Febrero 2022

Integrantes:

Santiago Rivas Betancourt, Legajo: 61007, Usuario GitHub: srivas

Thomás Germán Busso Zungri, Legajo: 62519, Usuario GitHub: tbussozungri


Contenido:
  	1) Uso del makeFile incluido.
  	2) Ejecución del programa por consola
  	3) Salida del programa.

1) Uso del makefile incluido:
	Para utilizar el makeFile se debe tener instalado el programa make.
	Se asume que la carpeta del proyecto fue descomprimida de el directorio ~/Desktop.
	Para compilar el programa utilizando el comando make seguir los siguientes pasos:
    		(1) cd ~/Desktop/IMDB_DATA
    		(2) make
	Para borrar los archivos objeto generados por el makeFile:
    		make clean

2) Ejecución del programa por consola:
	Para llamar al programa se debe cambiar al directorio donde se encuentra el ejecutable IMDB.
	Al llamar al programa se deben ingresar obligatoriamente dos parametros y opcionalmente se pueden agregar dos mas.
	Parametros necesarios:
		Los parametros necesarios son los paths a los archivos de obras y generos
	



  Para ejecutar el programa se deben utilizar las siguientes instrucciones en la Terminal:
  a) Primero, ir a la carpeta del ejecutable. Suponiendo que el archivo comprimido se descargó en el escritorio, será:
    cd ~/Desktop/IMDb
  b) Luego, compilar el programa, si es necesario, utilizando el comando (2) (y opcionalmente el (3)) descriptos en la sección
     2) de este archivo.
  c) Finalmente, ejecutar el programa con la instrucción:
    ./imdb path_a_archivo_csv path_a_archivo_generos
  Ver que path_a_archivo_csv es la ruta hacia el archivo CSV a procesar y path_a_archivo_generos es la ruta al archivo donde
  se encuentran los géneros de las obras a procesar.
  El programa, por tanto, espera 2 parámetros con rutas a archivo válidas. Si el programa detecta menos de 2 argumentos abortará
  la ejecución (mostrando un error adecuado), si detecta más de 2 argumentos, los sobrantes serán ignorados; y si las rutas a
  archivos son inválidas también abortará (mostrando un error adecuado). Estos errores se detallan en la sección 6).

3) Salida del programa:
  El programa generará 3 archivos .csv, 1 por cada query solicitada:
    a) query1.csv contiene la cantidad de películas, series y cortos por año.
    b) query2.csv contiene el ranking de las 500 mejores películas animadas.
    c) query3.csv contiene las 100 peliculas con mas votos de cada año.
    d) query4.csv contiene las 250 series con mejor calificacion dentro de los limites de anios especificados
    e) query5.csv contiene las 50 series con peor calificacion dentro de los limites de anios especificados
  Estos archivos se encontrarán en el mismo directorio que el ejecutable anteriormente generado
