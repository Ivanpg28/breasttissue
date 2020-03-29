#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

struct Calculo calcularMedia(struct Caso *casos)
{
	struct Calculo media;
	media.i0 = 0;
	media.pa500 = 0;
	media.hfs = 0;
	media.da = 0;
	media.area = 0;
	media.ada = 0;
	media.max_ip = 0;
	media.dr = 0;
	media.p = 0;
	double sum = 0.0;
	int tam;
	int i = 0;
	while (casos[i].id != 0)
	{
		//Sumamos cada caso
		media.i0 += casos[i].i0;
		media.pa500 += casos[i].pa500;
		media.hfs += casos[i].hfs;
		media.da += casos[i].da;
		media.area += casos[i].area;
		media.ada += casos[i].ada;
		media.max_ip += casos[i].max_ip;
		media.dr += casos[i].dr;
		media.p += casos[i].p;
		i++;
	}
	//dividimos por el numero de casos
	media.i0 /= i;
	media.pa500 /= i;
	media.hfs /= i;
	media.da /= i;
	media.area /= i;
	media.ada /= i;
	media.max_ip /= i;
	media.dr /= i;
	media.p /= i;

	return media;
}

struct Calculo calcularDest(struct Caso *casos, struct Calculo media)
{
	struct Calculo dest;
	dest.i0 = 0;
	dest.pa500 = 0;
	dest.hfs = 0;
	dest.da = 0;
	dest.area = 0;
	dest.ada = 0;
	dest.max_ip = 0;
	dest.dr = 0;
	dest.p = 0;
	double sum = 0.0;
	int i = 0;

	while (casos[i].id != 0)
	{
		//hacemos (caso - media)̂ 2
		dest.i0 += pow(casos[i].i0 - media.i0, 2);
		dest.pa500 += pow(casos[i].pa500 - media.pa500, 2);
		dest.hfs += pow(casos[i].hfs - media.hfs, 2);
		dest.da += pow(casos[i].da - media.da, 2);
		dest.area += pow(casos[i].area - media.area, 2);
		dest.ada += pow(casos[i].ada - media.ada, 2);
		dest.max_ip += pow(casos[i].max_ip - media.max_ip, 2);
		dest.dr += pow(casos[i].dr - media.dr, 2);
		dest.p += pow(casos[i].p - media.p, 2);
		i++;
	}
	// hacemos raiz de lo anterior dividido entre el numero de casos - 1
	dest.i0 = sqrt(dest.i0 / (i - 1));
	dest.pa500 = sqrt(dest.pa500 / (i - 1));
	dest.hfs = sqrt(dest.hfs / (i - 1));
	dest.da = sqrt(dest.da / (i - 1));
	dest.area = sqrt(dest.area / (i - 1));
	dest.ada = sqrt(dest.ada / (i - 1));
	dest.max_ip = sqrt(dest.max_ip / (i - 1));
	dest.dr = sqrt(dest.dr / (i - 1));
	dest.p = sqrt(dest.p / (i - 1));

	return dest;
}

// funcion que lee los casos de un fichero dado
struct Caso *leerCasos(char *path)
{
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
			//%[^,] es una expresion regular para cualquier valor excepto la coma ,
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

//funcion que muestra los casos almacenados en un array de structs caso
void mostrarCasos(struct Caso *casos)
{
	int i = 0;
	while (casos[i].id != 0)
	{
		printf("\n%d,%s,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", casos[i].id, casos[i].class, casos[i].i0, casos[i].pa500, casos[i].hfs, casos[i].da, casos[i].area, casos[i].ada, casos[i].max_ip, casos[i].dr, casos[i].p);
		i++;
	}
	printf("\n");
}

main()
{
	int cc1, cc2, cc3, cc4, tid;
	int tarea = 1;
	int n1, n2, rsdo_train, rsdo_test;
	char fichero_train[50] = "BreastTissueTrain.csv";
	char fichero_test[50] = "BreastTissueTest.csv";

	struct Caso *casos = leerCasos(fichero_train);
	mostrarCasos(casos);

	struct Calculo media = calcularMedia(casos);
	printf("\nMedia: %lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", media.i0, media.pa500, media.hfs, media.da, media.area, media.ada, media.max_ip, media.dr, media.p);

	struct Calculo dest = calcularDest(casos, media);
	printf("\nDesviacion Estandar: %lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", dest.i0, dest.pa500, dest.hfs, dest.da, dest.area, dest.ada, dest.max_ip, dest.dr, dest.p);

	//serializamos media y desviacion para poder enviarlas al esclavo
	char media_ser[sizeof(media)];
	char dest_ser[sizeof(dest)];
	memcpy(media_ser, &media, sizeof(media));
	memcpy(dest_ser, &dest, sizeof(dest));

	printf("El id del maestro es %x\n", pvm_mytid()); /* enrolar en la PVM */
	/* arrancar 1 copia del proceso esclavo en la misma maquina */
	cc1 = pvm_spawn("esclavo", NULL, 1, "ubuntu-nodo1", 1, &tid);
	if (cc1 == 1)
	{
		pvm_initsend(PvmDataDefault); /* inicializar el buffer */

		pvm_pkstr(fichero_train);//envio del fichero
		pvm_pkbyte(media_ser, sizeof(media_ser), 1);//envio de la media
		pvm_pkbyte(dest_ser, sizeof(dest_ser), 1);//envio de la desviacion
		pvm_send(tid, tarea);   /* tarea indica al esclavo si tiene que normalizar o calcular distancias */
		cc2 = pvm_recv(-1, -1); /* recibir el resultado de la operación realizada en el esclavo */
		pvm_bufinfo(cc2, (int *)0, (int *)0, &tid);
		pvm_upkint(&rsdo_train, 1, 0);
		printf("Nomalizacion de casos fichero train: %d\n", rsdo_train);// 1 correcto
	}
	else
		printf("No se pudo iniciar el proceso esclavo\n");

	cc3 = pvm_spawn("esclavo", NULL, 1, "ubuntu-nodo2", 1, &tid);
	if (cc3 == 1)
	{
		pvm_initsend(PvmDataDefault); /* inicializar el buffer */
		pvm_pkstr(fichero_test); //envio del fichero
		pvm_pkbyte(media_ser, sizeof(media_ser), 1); //envio de la media
		pvm_pkbyte(dest_ser, sizeof(dest_ser), 1);  //envio de la desviacion
		pvm_send(tid, tarea);	/* tarea indica al esclavo si tiene que normalizar o calcular distancias */
		cc4 = pvm_recv(-1, -1); /* recibir el resultado de la operación realizada en el esclavo */
		pvm_bufinfo(cc4, (int *)0, (int *)0, &tid);
		pvm_upkint(&rsdo_test, 1, 0);
		printf("Nomalizacion de casos fichero test: %d\n", rsdo_test); // 1 correcto
	}
	else
		printf("No se pudo iniciar el proceso esclavo\n");
	pvm_exit();
	free(casos);
	exit(0);
}
