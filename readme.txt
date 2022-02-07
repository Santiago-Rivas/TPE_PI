# TPE_PI
Trabajo Práctico Final - Programación Imperativa - ITBA - Febrero 2022

Integrantes:

Santiago Rivas Betancourt, Legajo: 61007, Usuario GitHub: srivas

Thomás Germán Busso Zungri, Legajo: 62519, Usuario GitHub: tbussozungri


Contenido:
  	1) Uso del makeFile incluido.
  	2) Ejecución del programa por consola
  	3) Salida del programa.

1) Uso del makeFile incluido:
	Para utilizar el makeFile se debe tener instalado el programa make.
	Se asume que la carpeta del proyecto fue descomprimida de el directorio ~/Desktop.
	Para compilar el programa utilizando el comando make seguir los siguientes pasos:
    		(1) cd ~/Desktop/IMDB_DATA
    		(2) make
	Para borrar los archivos objeto generados por el makeFile:
    		make clean

2) Ejecución del programa por consola:
	Para llamar al programa se debe cambiar al directorio donde se encuentra el ejecutable IMDB.
    		(1) cd ~/Desktop/IMDB_DATA
	Al llamar al programa se deben ingresar obligatoriamente dos parámetros y opcionalmente se pueden agregar dos mas.
	Parámetros necesarios:
	Los parámetros necesarios son los paths a los archivos de obras y géneros.
		./imdb /PATH/ARCHIVO/OBRAS.csv /PATH/ARCHIVO/GÉNEROS.csv
	Parámetros opcionales:
	Opcionalmente se pueden agregar dos parámetros mas. Estos parámetros deben ser números naturales que representan el rango de años para los cuales se tomara encuentra títulos para los queries 4 y 5.
	En el caso de no utilizar parámetros opcionales el query 4 y 5 tomaran en consideración todos los títulos.
	En el caso de que se utilice un solo parámetro adicional, el query 4 y 5 analizaran series que hayan tenido al menos una temporada posterior al año introducido. En el siguiente ejemplo se consideran las series con temporadas que salieron después del año 2000:
		./imdb /PATH/ARCHIVO/OBRAS.csv /PATH/ARCHIVO/GÉNEROS.csv 2000 
	Si se utilizan dos parámetros opcionales, el query 4 y 5 analizaran series que hayan tenido al menos una temporada entre el rango de años introducido. En el siguiente ejemplo se consideran las series que sacaron alguna temporada entre los años 2000 y 2001:
		./imdb /PATH/ARCHIVO/OBRAS.csv /PATH/ARCHIVO/GÉNEROS.csv 2000 2001


3) Salida del programa:
	El programa generará cinco archivos .csv, uno por cada query solicitada:
		a) query1.csv contiene la cantidad de películas, series y cortos por año.
    		b) query2.csv contiene el ranking de las 500 mejores películas animadas.
    		c) query3.csv contiene las 100 películas con mas votos de cada año.
    		d) query4.csv contiene las 250 series con mejor calificación dentro de los limites de años especificados
    		e) query5.csv contiene las 50 series con peor calificación dentro de los limites de años especificados
	Estos archivos se encontrarán en el mismo directorio que el ejecutable anteriormente generado.
	El programa puede retornar una de las siguientes salidas:
		0: Si no ocurrió ningún error.
		1: Si hubo un error con los parámetros ingresados.
		2: Si hubo un error de alocamiento de memoria.
		3: Si hubo un error al abrir el archivo de obras.
		4: Si hubo un error al abrir el archivo de géneros.
		5: Si hubo un error al abrir alguno de los archivo de retorno.
	Nota: Si alguno de los archivo de retorno se logro abrir, este sera llenado con el query correspondiente.

