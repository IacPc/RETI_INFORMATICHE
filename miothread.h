#include "tftp.h"
#include<pthread.h>
#include<stdbool.h>


//Codice sorgente del thread

struct arg_mt{

	struct sockaddr_in cl_addr;

	pthread_t* pt;

	char cartella[64]; // contiene la destinazioen in cui cercare il file
	struct R_Wrq_pkt rw;
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


	struct ACK_pkt 	 ack; 
	struct Data_pkt  dp;
	struct Err_pkt	 er;

	struct arg_mt* arg = (struct arg_mt*) a;
	

	FILE *fptr;
	
	addrlen = sizeof(arg->cl_addr);
	
	//creo un nuovo socket per questo trasferimento

	ns = socket(AF_INET, SOCK_DGRAM, 0);

	strcpy(buf_dest,arg->cartella);

	//strcat(buf_dest,arg->rw.filename);
	printf("cerco il file:%s\n",buf_dest);
	dp.Data =(char*)malloc(DATA_P_FULL);
	
	if ((fptr = fopen(buf_dest, "r")) == NULL){


		er.Opcode = ERR_OPC;
		er.Err_Numb =1;

		len=serializza_Err_pkt(buf_err,&er);

		sendto(ns, buf_err,len, 0,
						(struct sockaddr*)&arg->cl_addr,sizeof(arg->cl_addr));
	    printf("Doc non trovato,inviato pacchetto errore \n");
		goto fine;
    }		




	while(1){


		int i=0;

		//trasferimento testuale
		if (strcmp(arg->rw.mode,"netascii")==0){

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
		//trasferimento binario
		if (strcmp(arg->rw.mode,"octet")==0){

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

		dp.Opcode= DATA_OPC;// costante definita in tftp.h

		memcpy(dp.Data,buf_d,i);
		dp.Block_numb= nb;
		nb++;

		len =serializza_Data_pkt(buf_d,&dp,i);

		sendto(ns, buf_d,len, 0,(struct sockaddr*)&arg->cl_addr,
														sizeof(arg->cl_addr));



        printf("%d byte del blocco %d inviati al client \n",len,dp.Block_numb);	

		recvfrom(ns, buf_ack,ACK_SIZE, 0,(struct sockaddr*)&arg->cl_addr,&addrlen);
		
		deserializza_ACK_pkt(buf_ack,&ack);
		
		printf("ricevuto ack blocco numero:%d\n",ack.Block_numb);
		

		if(ultimo|| i<512){
			rewind(fptr);
			break;	
		}
	}


fine:
	free((void*)dp.Data);
	free((void*)a);
	pthread_exit(NULL);
}


