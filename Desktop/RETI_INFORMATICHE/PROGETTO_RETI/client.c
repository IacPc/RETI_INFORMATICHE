
#include "cl_funz.h"



int main(int argc, char* argv[]){

    int sd;
	char ip_ser[4];
	char porta[4];	
	char file[32];	

	int tot=0;
	
    struct sockaddr_in srv_addr;// my_addr;
	struct arg_get ag;



	sd = socket(AF_INET,SOCK_DGRAM,0);
    
    /* Creazione indirizzo di bind */
	strncpy(ip_ser,argv[1],4);
	strncpy(porta,argv[2],4);
/*
    memset(&my_addr, 0, sizeof(my_addr)); 
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(atoi(porta));
    my_addr.sin_addr.s_addr = INADDR_ANY;*/
 
       
    /* Creazione indirizzo del server */
    memset(&srv_addr, 0, sizeof(srv_addr)); // Pulizia 
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(atoi(porta));
    inet_pton(AF_INET, ip_ser, &srv_addr.sin_addr);
	
	strcpy(file,"copione.txt\0");
	strcpy(ag.mode,"txt\0");
	ag.srv_addr= srv_addr;	
	ag.sd=sd;
	strcpy(ag.file,file);
	tot =get(&ag);
	printf("scaricati %d byte\n",tot);
}
