#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>
#include <string.h>

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

main() {
	int ptid,nbytes,msgtag,tid;
	int n1, n2, rsdo = 1;
    struct Calculo media;
   	char *b;
	
	
	ptid=pvm_parent();
	int bufid=pvm_recv(ptid,-1);
	pvm_bufinfo(bufid,&nbytes,&msgtag,&tid);//msgtag será 1 o 2 dependiendo de lo que envíe el maestro
	pvm_upkbyte(b,1,0); //desempaqueto 1 dato desde la posicion 0
	//pvm_upkint(&n2,1,0);
	if(msgtag==1){
        memcpy(&media, b, sizeof(media));
        printf("\nMedia: %f,%f,%f,%f,%f,%f,%f,%f,%f", media.i0, media.pa500, media.hfs, media.da, media.area, media.ada, media.max_ip, media.dr, media.p);
	}
	else if (msgtag==2){
		rsdo=n1-n2;
	}
	pvm_initsend(PvmDataDefault); /* inicializar el buffer */
	//pvm_pkint(&rsdo,1,0);  /* empaqueta el resultado y lo envía al padre*/
	pvm_pkint(&rsdo,1,0);  /* empaqueta el resultado y lo envía al padre*/
	pvm_send(ptid,0); 
	pvm_exit();
	exit(0); 
}