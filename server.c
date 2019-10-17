

#include "miothread.h"


int main(int argc, char** argv){

	int ret, sd; 
    socklen_t addrlen;

	struct arg_mt* arg;
	struct sockaddr_in cl_addr, my_addr;
	struct Err_pkt er;
	struct R_Wrq_pkt rw2;

	uint16_t opc;
	int pid; //id porta
	char buf_rq[1024];
	char percorso[128];

	pthread_t* pt;

    // Creazione socket 
    sd = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(argc<3){
    	printf("inserire argomenti obbligatori\n");
    	return;
    }

	pid=atoi(argv[1]);
	strcpy(percorso,(char*)(argv[2]));

	

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




    while(1){

		//attendo richieste di lettura
		ret = recvfrom(sd, &buf_rq[0], 128, 0,
							   (struct sockaddr*)&cl_addr, &addrlen);

		if(ret==-1){
			printf("errore receive\n");
			exit(0);
		}


		printf("ricevuti %d byte dal client \n",ret);	

		//lettura opcode
		memcpy(&opc,buf_rq,2);
		
		switch(opc){
		
			case(RRQ_OPC):
				printf("ricevuta richiesta di lettura File system\n");
				
				printf("nome:");
				for(int i=2;i<20;i++)
					printf("%c",buf_rq[i]);
				printf("\n");

				//inizializzo argomenti da passare al thread
				//struttura arg definita in miothread.h
				arg = (struct arg_mt*)malloc(sizeof(struct arg_mt));
	
				deserializza_R_Wrq_pkt(buf_rq,&rw2);
			
				arg->cl_addr=cl_addr;
				strcpy(arg->cartella,percorso);
		
				pt = (pthread_t*)malloc(sizeof(pthread_t));
				arg->pt=pt;
				strcpy(arg->rw.mode,rw2.mode);
				strcpy(arg->rw.filename,rw2.filename);
				
				pid= pthread_create(pt,NULL,routine_mt,(void*)arg);


				if(pid!=0){
					printf("errore thread\n");
					exit(0);
				}
					
				

			break;

			default:
				printf("OPCODE NON VALIDO\n");
				er.Opcode = ERR_OPC;
				er.Err_Numb= 0X0002;

				ret=serializza_Err_pkt(buf_rq,&er);

				sendto(sd, buf_rq,ret, 0,
						(struct sockaddr*)&cl_addr,sizeof(cl_addr));
			break;
		}
	}

}


