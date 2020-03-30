#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>
#include <string.h>
#include <dirent.h>
#include <math.h>

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
//struct que guarda los casos para los ficheros case 20 y 27
struct Dato{
	char Case[100];
	char Class[100];
	char Io[100];
	char Pa500[100];
	char Hfs[100];
	char Da[100];
	char Area[100];
	char Ada[100];
	char MaxIp[100];
	char Dr[100];
	char P[100];
	float Distancia;
    
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
		fprintf(fichero, "%d,%s,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", casos[i].id, casos[i].class, casos[i].i0, casos[i].pa500, casos[i].hfs, casos[i].da, casos[i].area, casos[i].ada, casos[i].max_ip, casos[i].dr, casos[i].p);
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
	struct Dato datos;
	struct Dato dato[1000];
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
		//leemos el fichero TestN
		FILE *fp;
		fp = fopen("pvm3/bin/LINUX/BreastTissueTestN.csv","r");
		char cadena[1000];
		char *token;
		int cont =0;
        //guardamos los datos del ficehro en variables
		if(fp){
		    while(fgets(cadena,1000,fp)!= NULL){
		        if(cont == 0){
		            strcpy(datos.Case, strtok(cadena, ","));
		            strcpy(datos.Class, strtok(NULL, ","));
		            strcpy(datos.Io, strtok(NULL, ","));
		            strcpy(datos.Pa500, strtok(NULL, ","));
		            strcpy(datos.Hfs, strtok(NULL, ","));
		            strcpy(datos.Da, strtok(NULL, ","));
		            strcpy(datos.Area, strtok(NULL, ","));
		            strcpy(datos.Ada, strtok(NULL, ","));
		            strcpy(datos.MaxIp, strtok(NULL, ","));
		            strcpy(datos.Dr, strtok(NULL, ","));
		            strcpy(datos.P, strtok(NULL, ","));
		        }
		        cont ++;
		    }
		}
		
		fclose(fp);
		
		//leemos el fichero TrainN
		FILE *f;
		f = fopen("pvm3/bin/LINUX/BreastTissueTrainN.csv", "r");
		char linea[1000];
		int i;
        //guardamos los datos del fichero en variables
		if(f){
		    
		    for(i=0;i<9;i++){
		        fgets(linea,1000,f);

		        strcpy(dato[i].Case, strtok(linea, ","));
		        strcpy(dato[i].Class, strtok(NULL, ","));
		        strcpy(dato[i].Io, strtok(NULL, ","));
		        strcpy(dato[i].Pa500, strtok(NULL, ","));
		        strcpy(dato[i].Hfs, strtok(NULL, ","));
		        strcpy(dato[i].Da, strtok(NULL, ","));
		        strcpy(dato[i].Area, strtok(NULL, ","));
		        strcpy(dato[i].Ada, strtok(NULL, ","));
		        strcpy(dato[i].MaxIp, strtok(NULL, ","));
		        strcpy(dato[i].Dr, strtok(NULL, ","));
		        strcpy(dato[i].P, strtok(NULL, ","));
		        
		        }    
		    }
		fclose(f);
        
        
        
        
		//datos del fichero 1 convertidos a float para las operaciones
		float convert[9];
		sscanf(datos.Io, "%f", &convert[0]);
		sscanf(datos.Pa500, "%f", &convert[1]);
		sscanf(datos.Hfs, "%f", &convert[2]);
		sscanf(datos.Da, "%f", &convert[3]);
		sscanf(datos.Area, "%f", &convert[4]);
		sscanf(datos.Ada, "%f", &convert[5]);
		sscanf(datos.MaxIp, "%f", &convert[6]);
		sscanf(datos.Dr, "%f", &convert[7]);
		sscanf(datos.P, "%f", &convert[8]);
        
		//datos del fichero 2 convertidos a float para las operaciones
		int j=0;
		float convert2[81];
		i=0;
		do{
		    
		    sscanf(dato[i].Io, "%f", &convert2[j]);
		    j++;
		    sscanf(dato[i].Pa500, "%f", &convert2[j]);
		    j++;
		    sscanf(dato[i].Hfs, "%f", &convert2[j]);
		    j++;
		    sscanf(dato[i].Da, "%f", &convert2[j]);
		    j++;
		    sscanf(dato[i].Area, "%f", &convert2[j]);
		    j++;
		    sscanf(dato[i].Ada, "%f", &convert2[j]);
		    j++;
		    sscanf(dato[i].MaxIp, "%f", &convert2[j]);
		    j++;
		    sscanf(dato[i].Dr, "%f", &convert2[j]);
		    j++;
		    sscanf(dato[i].P, "%f", &convert2[j]);
		    i++;
		    j++;
		    
		    
		}while(j!=81);
        
		//variables para las operaciones
		float potencia=2;
		
		int x=0;
		int y =0;
		
		float Io[10];
		float Pa500[10];
		float Hfs[10];
		float Da[10];
		float Area[10];
		float Ada[10];
		float MaxIp[10];
		float Dr[10];
		float P[10];
		
		do{
			Io[x] = pow(convert2[y] - convert[0],potencia);
			y++;
			Pa500[x] = pow(convert2[y] - convert[1],potencia);
			y++;
			Hfs[x] = pow(convert2[y] - convert[2],potencia);
			y++;
			Da[x] = pow(convert2[y] - convert[3],potencia);
			y++;
			Area[x] = pow(convert2[y] - convert[4],potencia);
			y++;
			Ada[x] = pow(convert2[y] - convert[5],potencia);
			y++;
			MaxIp[x] = pow(convert2[y] - convert[6],potencia);
			y++;
			Dr[x] = pow(convert2[y] - convert[7],potencia);
			y++;
			P[x] = pow(convert2[y] - convert[8],potencia);
			y++;
			x++;
		
		}while(x<9);

        //calculamos las distancias
		float distancia[9];
		int z;
		for(z=0;z<9;z++){
			distancia[z] = sqrt(Io[z]+Pa500[z]+Hfs[z]+Da[z]+Area[z]+Ada[z]+MaxIp[z]+Dr[z]+P[z]);
		
		}
        
		//creamos el fichero con las distancias
		FILE *fw;
		fw = fopen("pvm3/bin/LINUX/Case20.csv", "w");
		int a;
		for(a=0;a<9;a++){
		    fprintf(fw, "%s,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", datos.Case,datos.Class,Io[a],Pa500[a],Hfs[a],Da[a],Area[a],Ada[a],MaxIp[a],Dr[a],P[a],distancia[a]);
		
		}
		
		fclose(fw);
	}
	else if(msgtag==3){
		//leemos el fichero TestN
		FILE *fp;
		fp = fopen("pvm3/bin/LINUX/BreastTissueTestN.csv","r");
		char cadena[1000];
		char *token;
		int cont =0;
		//guardamos los datos en variables
		if(fp){
		    while(fgets(cadena,1000,fp)!= NULL){
				//como contador es 1 leemos la segunda linea del fichero
		        if(cont==1){
		            strcpy(datos.Case, strtok(cadena, ","));
		            strcpy(datos.Class, strtok(NULL, ","));
		            strcpy(datos.Io, strtok(NULL, ","));
		            strcpy(datos.Pa500, strtok(NULL, ","));
		            strcpy(datos.Hfs, strtok(NULL, ","));
		            strcpy(datos.Da, strtok(NULL, ","));
		            strcpy(datos.Area, strtok(NULL, ","));
		            strcpy(datos.Ada, strtok(NULL, ","));
		            strcpy(datos.MaxIp, strtok(NULL, ","));
		            strcpy(datos.Dr, strtok(NULL, ","));
		            strcpy(datos.P, strtok(NULL, ","));
		        }
		        cont++;
		    }

		}
		fclose(fp);

		//leemos el fichero TrainN
		FILE *f;
		f = fopen("pvm3/bin/LINUX/BreastTissueTrainN.csv", "r");
		char linea[1000];
		int i;
		//guardamos los datos en variables
		if(f){
		    
		    for(i=0;i<9;i++){
		        fgets(linea,1000,f);

		        strcpy(dato[i].Case, strtok(linea, ","));
		        strcpy(dato[i].Class, strtok(NULL, ","));
		        strcpy(dato[i].Io, strtok(NULL, ","));
		        strcpy(dato[i].Pa500, strtok(NULL, ","));
		        strcpy(dato[i].Hfs, strtok(NULL, ","));
		        strcpy(dato[i].Da, strtok(NULL, ","));
		        strcpy(dato[i].Area, strtok(NULL, ","));
		        strcpy(dato[i].Ada, strtok(NULL, ","));
		        strcpy(dato[i].MaxIp, strtok(NULL, ","));
		        strcpy(dato[i].Dr, strtok(NULL, ","));
		        strcpy(dato[i].P, strtok(NULL, ","));
		        
		        }
		            
		    }
		fclose(f);

		//datos del fichero 1 convertidos a float para las operaciones
		float convert[9];
		sscanf(datos.Io, "%f", &convert[0]);
		sscanf(datos.Pa500, "%f", &convert[1]);
		sscanf(datos.Hfs, "%f", &convert[2]);
		sscanf(datos.Da, "%f", &convert[3]);
		sscanf(datos.Area, "%f", &convert[4]);
		sscanf(datos.Ada, "%f", &convert[5]);
		sscanf(datos.MaxIp, "%f", &convert[6]);
		sscanf(datos.Dr, "%f", &convert[7]);
		sscanf(datos.P, "%f", &convert[8]);
		
		//datos del fichero 2 convertidos a float para las operaciones
		int j=0;
		float convert2[81];
		i=0;
		do{
		    
		    sscanf(dato[i].Io, "%f", &convert2[j]);
		    j++;
		    sscanf(dato[i].Pa500, "%f", &convert2[j]);
		    j++;
		    sscanf(dato[i].Hfs, "%f", &convert2[j]);
		    j++;
		    sscanf(dato[i].Da, "%f", &convert2[j]);
		    j++;
		    sscanf(dato[i].Area, "%f", &convert2[j]);
		    j++;
		    sscanf(dato[i].Ada, "%f", &convert2[j]);
		    j++;
		    sscanf(dato[i].MaxIp, "%f", &convert2[j]);
		    j++;
		    sscanf(dato[i].Dr, "%f", &convert2[j]);
		    j++;
		    sscanf(dato[i].P, "%f", &convert2[j]);
		    i++;
		    j++;
		    
		}while(j!=81);
        
		//variables para las operaciones
		float potencia=2;
		int x=0;
		int y =0;
		
		float Io[10];
		float Pa500[10];
		float Hfs[10];
		float Da[10];
		float Area[10];
		float Ada[10];
		float MaxIp[10];
		float Dr[10];
		float P[10];
		
		do{

			Io[x] = pow(convert2[y] - convert[0],potencia);
			y++;
			Pa500[x] = pow(convert2[y] - convert[1],potencia);
			y++;
			Hfs[x] = pow(convert2[y] - convert[2],potencia);
			y++;
			Da[x] = pow(convert2[y] - convert[3],potencia);
			y++;
			Area[x] = pow(convert2[y] - convert[4],potencia);
			y++;
			Ada[x] = pow(convert2[y] - convert[5],potencia);
			y++;
			MaxIp[x] = pow(convert2[y] - convert[6],potencia);
			y++;
			Dr[x] = pow(convert2[y] - convert[7],potencia);
			y++;
			P[x] = pow(convert2[y] - convert[8],potencia);
			y++;
			x++;
		
		}while(x<9);
        
        //calculamos las distancias
		float distancia[9];
		int z;
		for(z=0;z<9;z++){
			distancia[z] = sqrt(Io[z]+Pa500[z]+Hfs[z]+Da[z]+Area[z]+Ada[z]+MaxIp[z]+Dr[z]+P[z]);
		
		}
        
		//creamos el fichero con las distancias
		FILE *fc;
		fc = fopen("pvm3/bin/LINUX/Case27.csv", "w");
		int a;
		for(a=0;a<9;a++){
		    fprintf(fc, "%s,%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", datos.Case,datos.Class,Io[a],Pa500[a],Hfs[a],Da[a],Area[a],Ada[a],MaxIp[a],Dr[a],P[a],distancia[a]);
		
		}
		fclose(fc);

	}

	pvm_initsend(PvmDataDefault); /* inicializar el buffer */
	pvm_pkint(&rsdo, 1, 0);		  /* empaqueta el resultado y lo envía al padre*/
	pvm_send(ptid, 0);
	pvm_exit();
	exit(0);
}
