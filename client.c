
#include "cl_funz.h"



int main(int argc, char* argv[]){

    int sd;
	char ip_ser[4];
	char porta[4];	
	char file[32];	
	char comando1[6];
	char comando2[32];
	char comadno3[32];
	int tot=0;
	
    struct sockaddr_in srv_addr;// my_addr;
	struct arg_get ag;



	sd = socket(AF_INET,SOCK_DGRAM,0);
    
    /* Creazione indirizzo di bind */
	strncpy(ip_ser,argv[1],4);
	strncpy(porta,argv[2],4);

 
       
    /* Creazione indirizzo del server */
    memset(&srv_addr, 0, sizeof(srv_addr)); // Pulizia 
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(atoi(porta));
    inet_pton(AF_INET, ip_ser, &srv_addr.sin_addr);


	printf("sono disponibili i seguenti comandi:\n");
	printf("!help --> mostra l'elenco dei comandi disponibili\n");
	printf("!mode{txt|bin} --> impposta il modo di trasferimento dei files (testo o binario)\n");
	printf("!get filename nome_locale --> richiede al server il nome del <filenamme> e lo salva localment con il nome <nome_locale>\n");

	printf("!quit --> termina il client\n");

	while(1){
		scanf("%s %s %s",comando1,comando2,comando3);
		
		switch(comando1)
		case("!get"):
		strcpy(file,"copione.txt\0");
		strcpy(ag.mode,"txt\0");
		ag.srv_addr= srv_addr;	
		ag.sd=sd;
		strcpy(ag.file,file);
		tot =get(&ag);
		printf("scaricati %d byte\n",tot);
		break

	}
}
