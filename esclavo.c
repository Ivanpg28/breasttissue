#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>
#include <string.h>

struct Caso
{
	short id;
	char class[3];
	float i0;
	float pa500;
	float hfs;
	float da;
	float area;
	float ada;
	float max_ip;
	float dr;
	float p;
};

struct Calculo
{
	float i0;
	float pa500;
	float hfs;
	float da;
	float area;
	float ada;
	float max_ip;
	float dr;
	float p;
};

void logger(char *texto)
{
	FILE *fichero = NULL;
	fichero = fopen("pvm3/bin/LINUX/esclavo.log", "a");
	fprintf(fichero, "%s\n", texto);
	fclose(fichero);
}

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
			fscanf(fichero, "%hd,%[^,],%f,%f,%f,%f,%f,%f,%f,%f,%f", &casos[i].id, casos[i].class, &casos[i].i0, &casos[i].pa500, &casos[i].hfs, &casos[i].da, &casos[i].area, &casos[i].ada, &casos[i].max_ip, &casos[i].dr, &casos[i].p);
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

struct Caso normalizarCaso(struct Caso caso, struct Calculo media, struct Calculo dest)
{
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

void escribirCasos(struct Caso *casos, char *path)
{
	FILE *fichero = NULL;
	int i = 0;
	fichero = fopen(path, "w");

	while (casos[i].id != 0)
	{
		fprintf(fichero, "\n%d,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f", casos[i].id, casos[i].class, casos[i].i0, casos[i].pa500, casos[i].hfs, casos[i].da, casos[i].area, casos[i].ada, casos[i].max_ip, casos[i].dr, casos[i].p);
		i++;
	}
	fclose(fichero);
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
	char fichero_destino[50] = "pvm3/bin/LINUX/BreastTissueTrainN.csv";
	char b[sizeof(media)], c[sizeof(dest)];

	ptid = pvm_parent();
	logger("conecta padre");
	int bufid = pvm_recv(ptid, -1);
	pvm_bufinfo(bufid, &nbytes, &msgtag, &tid); //msgtag será 1 o 2 dependiendo de lo que envíe el maestro
	pvm_upkstr(fichero);
	logger("desempaqueta fichero");
	pvm_upkbyte(b, sizeof(media), 0);
	logger("desempaqueta media");
	memcpy(&media, b, sizeof(media));
	pvm_upkbyte(c, sizeof(dest), 0);
	logger("desempaqueta dest");
	memcpy(&dest, c, sizeof(dest));

	if (msgtag == 1)
	{
		casos = leerCasos("pvm3/bin/LINUX/BreastTissueTrain.csv");
		logger("sale leer");
		casos_normalizados = normalizarCasos(casos, media, dest);
		logger("normaliza datos");
		escribirCasos(casos, fichero_destino);
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