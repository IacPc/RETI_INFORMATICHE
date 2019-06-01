#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define RRQ_OPC		0x0001
#define DATA_OPC	0x0003
#define ACK_OPC		0x0004
#define ERR_OPC 	0x0005
#define R_WRQ_SIZE  64
#define ACK_SIZE	4
#define DATA_P_FULL 516
		
struct R_Wrq_pkt{
		uint16_t Opcode;
		char filename[32];
		char mode[9];
};



int  serializza_R_Wrq_pkt(char* buf,struct R_Wrq_pkt* rw){
	
		uint16_t n_op=rw->Opcode;

		memcpy(buf,&n_op,sizeof(n_op));

		strcpy(&buf[2],rw->filename);

		//next=primo indice libero dopo aver copiato il nome del file
		//alla lunghezza di filename va aggiunto 1 perchè strlen
		//non include il fine stringa 
		int next = 2+(strlen(rw->filename)+1);
		
		strcpy(&buf[next],rw->mode);
		next = next+(strlen(rw->mode)+1)+1;
		return next;

}

void  deserializza_R_Wrq_pkt(char* buf,struct R_Wrq_pkt* rw){
	int next;	

	memcpy(&rw->Opcode,buf,2);
	
	strcpy(rw->filename,&buf[2]);
	
	//next = inizio stringa mode(txt o bin)
	//pos inizio filename + lung filename + \0	
	//     |						|				
	//	   V						V				
	next = 2 	+		(strlen(rw->filename)+1);

	strcpy(rw->mode,&buf[next]);
	printf("rw->mode = %s\n",rw->mode);
}

struct Data_pkt{

		uint16_t Opcode;
		uint16_t Block_numb;
		char* Data;

};

//restituisce la lunghezza del buffer

int  serializza_Data_pkt(char* buf,struct Data_pkt* dp,int len){
	
		uint16_t n_op=dp->Opcode;

		memcpy(buf,&n_op,sizeof(n_op));
				
		
		n_op=dp->Block_numb;

		memcpy(buf+2,&n_op,sizeof(n_op));

		memcpy(&buf[4],dp->Data,len);

		return 2+2+len;
};


void  deserializza_Data_pkt(char* buf,struct Data_pkt* dp,int l){

		uint16_t n_op;

		memcpy(&n_op,buf,2);
		dp->Opcode = n_op;
		
		memcpy(&n_op,&buf[2],2);
		dp->Block_numb = n_op;
		memcpy(dp->Data,&buf[4],l-4);
		
}

struct ACK_pkt{

		uint16_t Opcode;
		uint16_t Block_numb;

};



void  serializza_ACK_pkt(char* buf,struct ACK_pkt* ack){
	
		uint16_t n_op=ack->Opcode;

		memcpy(buf,&n_op,sizeof(n_op));
				
		n_op=htons(ack->Block_numb);

		memcpy(buf+2,&n_op,sizeof(n_op));

};

void  deserializza_ACK_pkt(char* buf,struct ACK_pkt* ack){

	uint16_t n_op;

	memcpy(&n_op,buf,2);
	ack->Opcode= n_op;

	memcpy(&n_op,&buf[2],2);
	ack->Block_numb= ntohs(n_op);

}


void costruisci_ack_pkt(struct ACK_pkt* ack,int bn){
	ack->Opcode=ACK_OPC;
	ack->Block_numb= bn;

}



struct Err_pkt{
		uint16_t Opcode;
		uint16_t Err_Numb;
		char Err_Msg[32];

};

//restituisce la lunghezza del buffer

int  serializza_Err_pkt(char* buf,struct Err_pkt* er){
		

		uint16_t n_op=er->Opcode;
		int len=0;
		memcpy(buf,&n_op,sizeof(n_op));
		memcpy(&buf[2],&er->Err_Numb,sizeof(n_op));
		if(er->Err_Numb==0X0001){
			strcpy(&buf[4],"File not found\0");
		}
		else{
			strcpy(&buf[4],"Illegal tftp operation\0");
			
		}		
		len = strlen(&buf[4])+1;

		int quanti = 2*sizeof(uint16_t)+len;

		
		return quanti;
};

void deserializza_err_pkt(struct Err_pkt* er,char* buf){
	
	uint16_t nh;
	memcpy(&nh,&buf[0],2);
	
	er->Opcode = nh;

	memcpy(&nh,&buf[2],2);
	
	er->Err_Numb = nh;
	

	strcpy(er->Err_Msg,&buf[4]);
	
	

}


