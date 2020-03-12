#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

struct Calculo calcularMedia(struct Caso *casos) {
    struct Calculo media;
    float sum = 0.0;
    int tam;
    int i = 0;
    while(casos[i].id != 0) {
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

struct Calculo calcularDest(struct Caso *casos, struct Calculo media) {
    struct Calculo dest;
    int i = 0;

    while(casos[i].id != 0) {
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

	dest.i0 = sqrt(dest.i0 / i);
	dest.pa500 = sqrt(dest.pa500 / i);
	dest.hfs = sqrt(dest.hfs / i);
	dest.da = sqrt(dest.da / i);
	dest.area = sqrt(dest.area / i);
	dest.ada = sqrt(dest.ada / i);
	dest.max_ip = sqrt(dest.max_ip / i);
	dest.dr = sqrt(dest.dr / i);
	dest.p = sqrt(dest.p / i);

	 return dest;
}

struct Caso *leerCasos(char *path)
{
	FILE *fichero = NULL;
	int dim = 100;
	int i = 0;
	struct Caso *casos = malloc(dim * sizeof(struct Caso));

    for (int j = 0; j < dim; j++)
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

void mostrarCasos(struct Caso *casos)
{
    char ch;
    int i = 0;
    while (casos[i].id != 0)
    {
        printf("\n%d,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f", casos[i].id, casos[i].class, casos[i].i0, casos[i].pa500, casos[i].hfs, casos[i].da, casos[i].area, casos[i].ada, casos[i].max_ip, casos[i].dr, casos[i].p);
        i++;
    }
    printf("\n");
}

main()
{
	int cc1, cc2, tid;
	int tarea = 1; //tarea = 1 suma, tarea = 2 resta
	int n1, n2, rsdo;
	char *fichero = "BreastTissueTrain.csv";

	struct Caso *casos = leerCasos(fichero);
	mostrarCasos(casos);

   	struct Calculo media = calcularMedia(casos);
   	printf("\nMedia: %f,%f,%f,%f,%f,%f,%f,%f,%f", media.i0, media.pa500, media.hfs, media.da, media.area, media.ada, media.max_ip, media.dr, media.p);
	
	struct Calculo dest = calcularDest(casos, media);
	printf("\nDesviacion Estandar: %f,%f,%f,%f,%f,%f,%f,%f,%f", dest.i0, dest.pa500, dest.hfs, dest.da, dest.area, dest.ada, dest.max_ip, dest.dr, dest.p);

   
	printf("El id del maestro es %x\n", pvm_mytid()); /* enrolar en la PVM */
	/* arrancar 1 copia del proceso esclavo en otra maquina */
	cc1 = pvm_spawn("esclavo", NULL, 1, "ubuntu-nodo-1", 1, &tid);
	if (cc1 == 1)
	{
		pvm_initsend(PvmDataDefault); /* inicializar el buffer */
		char tmp[sizeof(media)];
    	memcpy(tmp, &media, sizeof(media));
		pvm_pkstr(fichero);
		pvm_pkbyte(tmp, 1, 0);		  /* empaquetar datos y enviar al esclavo*/
		//pvm_pkint(&n2, 1, 0);
		pvm_send(tid, tarea);   /* tarea indica al esclavo si debe sumar o restar */
		cc2 = pvm_recv(-1, -1); /* recibir el resultado de la operación realizada en el esclavo */
		pvm_bufinfo(cc2, (int *)0, (int *)0, &tid);
		pvm_upkint(&rsdo, 1, 0);
		printf("El resultado de la operación es: %d\n", rsdo);
	}
	else
		printf("No se pudo iniciar el proceso esclavo\n");
	pvm_exit();
	free(casos);
	exit(0);
}
