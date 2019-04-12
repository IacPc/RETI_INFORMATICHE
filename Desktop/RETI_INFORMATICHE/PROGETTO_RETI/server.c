

#include "miothread.h"


int main(int argc, char** argv){

	int ret, sd; 
    socklen_t addrlen;

	struct arg_mt* arg;
	struct sockaddr_in cl_addr, my_addr;
	struct Err_pkt er;


	uint16_t opc;
	int pid;
	char buf_rq[1024];
	char percorso[128];

	pthread_t* pt;

    // Creazione socket 
    sd = socket(AF_INET, SOCK_DGRAM, 0);
	
	pid=atoi(argv[1]);

    // Creazione indirizzo di bind 
    memset(&cl_addr, 0, sizeof(cl_addr));  
    memset(&my_addr, 0, sizeof(my_addr));  
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(pid);
    my_addr.sin_addr.s_addr = INADDR_ANY;


    ret = bind(sd, (struct sockaddr*)&my_addr, sizeof(my_addr) );
    
    if( ret < 0 ){
        perror("Bind non riuscita\n");
        exit(0);
    }
    
    printf("server in fase di avvio\n");

    addrlen = sizeof(cl_addr);

	strcpy(percorso,(char*)(argv[2]));


    while(1){

		ret = recvfrom(sd, &buf_rq[0], 512, 0,
							   (struct sockaddr*)&cl_addr, &addrlen);
		if(ret==-1){
			printf("errore receive\n");
			exit(0);
		}


		printf("ricevuti %d byte dal client \n",ret);	

		memcpy(&opc,buf_rq,2);

		switch(opc){
		
			case(RRQ_OPC):
				printf("ricevuta richiesta di lettura File system\n");
				
				arg = (struct arg_mt*)malloc(sizeof(struct arg_mt));
				memcpy(arg->buf_rq,buf_rq,R_WRQ_SIZE);
				
				arg->cl_addr=cl_addr;
				strcpy(arg->cartella,percorso);
				pt = (pthread_t*)malloc(sizeof(pthread_t));
				arg->pt=pt;
				pid= pthread_create(pt,NULL,routine_mt,(void*)arg);


				if(pid!=0){
					printf("errore thread\n");
					exit(0);
				}else{
					printf("successo thread\n");
					
				}

			break;

			default:
				printf("OPCODE NON VALIDO\n");
				er.Opcode = ERR_OPC;
				er.Err_Numb= 0X0002;

				ret=serializza_Err_pkt(buf_rq,&er);

				sendto(sd, buf_rq,ret, 0,
						(struct sockaddr*)&cl_addr,sizeof(cl_addr));
	    		printf("Doc non trovato,inviato pacchetto errore di %d byte\n",ret);
			break;
		}
	}
	close(sd);		
}

	 /**/