#include "tftp.h"
#include<pthread.h>
#include<stdbool.h>
#define MAX_THREADS 16



struct arg_mt{

	struct sockaddr_in cl_addr;

	int sd;					//socket
	pthread_t* pt;
	char cartella[64];
	char buf_rq[R_WRQ_SIZE];
};

void* routine_mt(void* a){

	char buf_d[512];
	char buf_ack[ACK_SIZE];
	char buf_err[128];
	char buf_dest[100];

	int curs =0;
	int nb=1;
	int ns;
	int len=0;
	bool ultimo=0;

	socklen_t addrlen;

	struct R_Wrq_pkt rw;
	struct ACK_pkt 	 ack; 
	struct Data_pkt  dp;
	struct Err_pkt	 er;

	struct arg_mt* arg = (struct arg_mt*) a;
	

	FILE *fptr;

	deserializza_R_Wrq_pkt(arg->buf_rq,&rw);

	
	addrlen = sizeof(arg->cl_addr);

	ns = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(buf_dest,arg->cartella);

	strcat(buf_dest,rw.filename);
	printf("cerco il file:%s\n",buf_dest);
	
	if ((fptr = fopen(buf_dest, "r")) == NULL){


		er.Opcode = ERR_OPC;
		er.Err_Numb =1;

		len=serializza_Err_pkt(buf_err,&er);

		sendto(ns, buf_err,len, 0,
						(struct sockaddr*)&arg->cl_addr,sizeof(arg->cl_addr));
	    printf("Doc non trovato,inviato pacchetto errore di %d byte\n",len);
		goto fine;
    }		


	dp.Data =(char*)malloc(516);

	while(1){


		int i=0;

		if (strcmp(rw.mode,"netascii")==0){
			printf("inizio trasf. testuale\n");
			for (;i<512;i++){
				fseek(fptr,curs,SEEK_SET);
				buf_d[i]=(char)fgetc(fptr);
				curs++;
				if(buf_d[i]==EOF){
					ultimo = 1;
					break;
				}
			}
		}

		if (strcmp(rw.mode,"octet")==0){
			printf("inizio trasf. binario\n");
			for (;i<512;i++){
				fseek(fptr,curs,SEEK_SET);
				len=fread(&buf_d[i],1,1,fptr);
				curs++;
				if(len==0){
					ultimo = 1;
					break;
				}
			}
		}

		dp.Opcode= DATA_OPC;

		memcpy(dp.Data,buf_d,i);
		dp.Block_numb= nb;
		nb++;

		len =serializza_Data_pkt(buf_d,&dp,i);

		sendto(ns, buf_d,len, 0,(struct sockaddr*)&arg->cl_addr,
														sizeof(arg->cl_addr));



        printf("%d byte del blocco %d inviati al clienti \n",
															len,dp.Block_numb);	

		recvfrom(ns, buf_ack,ACK_SIZE, 0,
						(struct sockaddr*)&arg->cl_addr,&addrlen);
		
		deserializza_ACK_pkt(buf_ack,&ack);
		
		printf("ricevuto ack blocco numero:%d\n",ack.Block_numb);
		

		if(ultimo){
			rewind(fptr);
			break;	
		}
	}


fine:
	free((void*)dp.Data);
	free((void*)a);
	pthread_exit(NULL);
}


