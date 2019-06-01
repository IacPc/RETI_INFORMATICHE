
#include "cl_funz.h"



int main(int argc, char* argv[]){

    int sd;
	char ip_ser[4];
	char porta[4];	
	char comando1[6];
	char comando2[32];
	char comando3[64];

	int tot=0;
	
    struct sockaddr_in srv_addr;// my_addr;
	struct arg_get ag;	//argomento da passare al thread

	sd = socket(AF_INET,SOCK_DGRAM,0);
    
    /* Creazione indirizzo di bind */
	strncpy(ip_ser,argv[1],4);
	strncpy(porta,argv[2],4);

 
       
    // Creazione indirizzo del server 
    memset(&srv_addr, 0, sizeof(srv_addr));  
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(atoi(porta));
    inet_pton(AF_INET, ip_ser, &srv_addr.sin_addr);

	//inizializzo parametro per la get
	
	strcpy(ag.mode,"octet\0");	//modalità predefinita trasferinto binario
	ag.percorso_loc= NULL;		//dove salvo i file scaricati
	ag.sd = sd;					//socket per la comunicazione
	ag.srv_addr= srv_addr;


	printf("digitare help per i comandi disponibili:\n");

	while(1){
		scanf("%s",comando1);
		

		if(strcmp(comando1,"!get")==0){
			scanf("%s %s",comando2,comando3);

			ag.sd=sd;
			strcpy(ag.file,comando2);
			ag.percorso_loc = malloc(sizeof(comando3));
			strcpy(ag.percorso_loc,comando3);
			tot =get(&ag);	
		
			printf("scaricati %d byte\n",tot);
			continue;
		}

		if(strcmp(comando1,"!mode")==0){
			scanf("%s",comando2);

			if (strcmp(comando2,"bin")==0){
				strcpy(ag.mode,"octet\0");
				printf("Modo di trasferimento %s impostato\n",ag.mode);
				continue;
			}

			if (strcmp(comando2,"txt")==0){
				strcpy(ag.mode,"netascii\0");
				printf("Modo di trasferimento %s impostato\n",ag.mode);
				continue;
			}
			printf("Modo di trasferimento errato \n");
			continue;
		}
		
		if(strcmp(comando1,"!help")==0){

			printf("sono disponibili i seguenti comandi:\n");

			printf("!help --> mostra l'elenco dei comandi disponibili\n");

			printf("!mode{txt|bin} --> imposta il modo di trasferimento dei files (testo o binario)\n");

			printf("!get filename nome_locale --> richiede al server il nome del <filename> e lo salva localment con il nome <nome_locale>\n");

			printf("!quit --> termina il client\n");


			continue;
		
		}

		if(strcmp(comando1,"!quit")==0)
			break;

		printf("comando non valido riprovare\n");
		
	}

	free(ag.percorso_loc);
	close(sd);
}
