#include "tftp.h"

struct arg_get{

	char mode[9];
	struct sockaddr_in srv_addr;
	int sd;
	char file[32];
	char* percorso_loc;
};


int get(struct arg_get* ag){

	int ret;
	//numero di blocco iniziale =1 come previsto da rfc 1350 pag 4
	int bn=1;

	//totale byte ricevuti dal server
	int tot=0;

	int da_inviare;
	uint16_t opc;
	socklen_t addrlen= sizeof(ag->srv_addr);

	struct R_Wrq_pkt rw;
	struct ACK_pkt ack; 
	struct Data_pkt dp;
	struct Err_pkt er;

	FILE* fptr=NULL;

	char buf[516];
	char percorso[128];
	char buf_ack[ACK_SIZE];



	strcpy(percorso,ag->percorso_loc);

	strcat(percorso,ag->file);
	printf("il percorso è %s\n",percorso);

	dp.Data = (char*)malloc(512);
	if(!dp.Data)
		exit(0); 

	rw.Opcode = RRQ_OPC;
	strcpy(rw.filename,ag->file);
	strcpy(rw.mode,ag->mode);

	da_inviare=serializza_R_Wrq_pkt(buf,&rw);

	printf("invio richiesta al server\n");

	ret = sendto(ag->sd, buf, da_inviare, 0,(struct sockaddr*)&ag->srv_addr,
																sizeof(ag->srv_addr));
	if(ret==-1){
		printf("errore invio richiesta al server\n");
		return -1;
	}
	printf("richiesta al server inviata\n");
	//ASPETTO IL PACCHETTO DATI


	while(1){

		ret=recvfrom(ag->sd,buf,DATA_P_FULL,0,(struct sockaddr*)&ag->srv_addr,&addrlen);
			
		tot+=ret;
		printf("ricevuti %d byte\n",ret);
		
		memcpy(&opc,buf,2);	
		

		if(opc==DATA_OPC){
			
			deserializza_Data_pkt(buf,&dp,ret);


			if(!fptr)
				fptr = fopen(percorso, "w");
				
			
			if(fptr == NULL){
				printf("creazione file fallita necessari permessi di root\n");
				goto fine;
			}

			fwrite(dp.Data, ret-4, 1, fptr);
		
			//costruisco e invio l'ack

			costruisci_ack_pkt(&ack,bn);
			serializza_ACK_pkt(buf_ack,&ack);

			sendto(ag->sd,buf_ack,sizeof(ack),0,(struct sockaddr*)&ag->srv_addr,
														sizeof(ag->srv_addr));
			printf("ack num. %d inviato\n",ack.Block_numb);
			
			if(ret<516){
				printf("download completato\n");
				fclose(fptr);
				printf("file chiuso\n");
				break;
			}

			bn++;
		}
		else{
			printf("ricevuto pacchetto errore\n");
			deserializza_err_pkt(&er,buf);
			printf("errore numero %d %s\n",er.Err_Numb,er.Err_Msg);
			break;
		}
		

	}
	fine:
	free(dp.Data);
	return tot;
}
