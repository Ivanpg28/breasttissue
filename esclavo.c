#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>
#include <string.h>

//struct que guarda cada uno de los casos
struct Caso
{
	short id;
	char class[3];
	double i0;
	double pa500;
	double hfs;
	double da;
	double area;
	double ada;
	double max_ip;
	double dr;
	double p;
};
//struct que guarda la media o la desviacion estandar
struct Calculo
{
	double i0;
	double pa500;
	double hfs;
	double da;
	double area;
	double ada;
	double max_ip;
	double dr;
	double p;
};
//funcion para poder debuggear la ejecucion del esclavo
void logger(char *texto)
{
	FILE *fichero = NULL;
	fichero = fopen("pvm3/bin/LINUX/esclavo.log", "a");
	fprintf(fichero, "%s\n", texto);
	fclose(fichero);
}

// funcion que lee los casos de un fichero dado
struct Caso *leerCasos(char *path)
{
	logger("entra leer");
	FILE *fichero = NULL;
	int dim = 100;
	int i = 0;
	struct Caso *casos = malloc(dim * sizeof(struct Caso));
	int j;
	for (j = 0; j < dim; j++)
	{
		casos[j].id = 0;
	}

	fichero = fopen(path, "r");
	if (!fichero == NULL)
	{
		while (!feof(fichero))
		{
			//%[^|] es una expresion regular para cualquier valor excepto la tuberia |
			fscanf(fichero, "%hd,%[^,],%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", &casos[i].id, casos[i].class, &casos[i].i0, &casos[i].pa500, &casos[i].hfs, &casos[i].da, &casos[i].area, &casos[i].ada, &casos[i].max_ip, &casos[i].dr, &casos[i].p);
			i++;
			if (i == dim - 1)
			{
				casos = realloc(casos, dim + 100);
				if (!casos)
				{
					printf("Algo salio mal");
				}
			}
		}
	}
	return casos;
}

//funcion para nomalizar cada caso dado
struct Caso normalizarCaso(struct Caso caso, struct Calculo media, struct Calculo dest)
{
	//(caso - media) / desviacion
	caso.i0 = (caso.i0 - media.i0) / dest.i0;
	caso.pa500 = (caso.pa500 - media.pa500) / dest.pa500;
	caso.hfs = (caso.hfs - media.hfs) / dest.hfs;
	caso.da = (caso.da - media.da) / dest.da;
	caso.area = (caso.area - media.area) / dest.area;
	caso.ada = (caso.ada - media.ada) / dest.ada;
	caso.max_ip = (caso.max_ip - media.max_ip) / dest.max_ip;
	caso.dr = (caso.dr - media.dr) / dest.dr;
	caso.p = (caso.p - media.p) / dest.p;

	return caso;
}

//funcion que itera por el array de structs y llama a la funcion normalizarCaso con cada uno
struct Caso *normalizarCasos(struct Caso *casos, struct Calculo media, struct Calculo dest)
{
	int i = 0;

	while (casos[i].id != 0)
	{
		casos[i] = normalizarCaso(casos[i], media, dest);
		i++;
	}

	return casos;
}

//funcion para escribir los casos en un fichero dado
void escribirCasos(struct Caso *casos, char *path)
{
	logger(path);
	FILE *fichero = NULL;
	int i = 0;
	fichero = fopen(path, "w");

	while (casos[i].id != 0)
	{
		fprintf(fichero, "\n%d,%s,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", casos[i].id, casos[i].class, casos[i].i0, casos[i].pa500, casos[i].hfs, casos[i].da, casos[i].area, casos[i].ada, casos[i].max_ip, casos[i].dr, casos[i].p);
		i++;
	}
	fclose(fichero);
}

//funcion que concatena 2 strings
char *concat(char *str1, char *str2)
{

	int i = 0;
	int j = 0;
	while (str1[i] != '\0')
	{
		i++;
	}

	while (str2[j] != '\0')
	{
		str1[i] = str2[j];
		i++;
		j++;
	}
	str1[i] = '\0';
	return str1;
}

//funcion que añade la letra N al final del fichero enviado por el maestro
char *generarFicheroDestino(char *fichero)
{
	int i = 0;
	int j = 0;
	char *fichero_destino = malloc(50);
	while (fichero[i] != '\0')
	{
		if (fichero[i] == '.')
		{
			fichero_destino[j] = 'N';
			j++;
		}
		fichero_destino[j] = fichero[i];
		i++;
		j++;
	}
	fichero_destino[j] = '\0';
	return fichero_destino;
}

main()
{
	logger("inicia");
	int ptid, nbytes, msgtag, tid;
	int n1, n2, rsdo = 1, i, j = 0;
	struct Calculo media;
	struct Calculo dest;
	struct Caso *casos;
	struct Caso *casos_normalizados;
	char path[50] = "pvm3/bin/LINUX/";
	char fichero[50];
	char *fichero_origen;
	char *fichero_destino;
	char media_ser[sizeof(media)] = "", dest_ser[sizeof(dest)] = "";

	ptid = pvm_parent();
	logger("conecta padre");
	int bufid = pvm_recv(ptid, -1);
	pvm_bufinfo(bufid, &nbytes, &msgtag, &tid); //msgtag será 1 o 2 dependiendo de lo que envíe el maestro
	if (msgtag == 1)
	{
		pvm_upkstr(fichero);
		logger("desempaqueta fichero");
		pvm_upkbyte(media_ser, sizeof(media), 1);
		logger("desempaqueta media");
		memcpy(&media, media_ser, sizeof(media));
		pvm_upkbyte(dest_ser, sizeof(dest), 1);
		logger("desempaqueta dest");
		memcpy(&dest, dest_ser, sizeof(dest));
		fichero_origen = concat(path, fichero);
		fichero_destino = generarFicheroDestino(fichero_origen);
		casos = leerCasos(fichero_origen);
		logger("sale leer");
		casos_normalizados = normalizarCasos(casos, media, dest);
		logger("normaliza datos");
		escribirCasos(casos_normalizados, fichero_destino);
		logger("escribe datos");
	}
	else if (msgtag == 2)
	{
		//Parte 2
	}

	pvm_initsend(PvmDataDefault); /* inicializar el buffer */
	pvm_pkint(&rsdo, 1, 0);		  /* empaqueta el resultado y lo envía al padre*/
	pvm_send(ptid, 0);
	pvm_exit();
	exit(0);
}
