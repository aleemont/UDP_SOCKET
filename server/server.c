#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<signal.h>
#include<errno.h>
#include"sequence.h"

struct protocollo{              //Struttura che contiene le informazioni del protocollo
  unsigned int num;
  unsigned int ufficio;
  unsigned int io;
  char oggetto[100];
  char md[50];
};

void addr_init(struct sockaddr_in *addr, int port, long int ip){
  addr->sin_family = AF_INET; //Inizializzo la famiglia di indirizzi (ipv4)
  addr->sin_port = htons((u_short) port); //conversione in little-endian del numero di porta (msb first)
  addr->sin_addr.s_addr = INADDR_ANY;
}

int main(void){
  unsigned short int port = atoi("8082");
  int rc = 0;
  struct sockaddr_in server, client;
  struct in_addr a;
  struct protocollo proto;
  in_addr_t addr;
  time_t now = time(0);
  struct tm *t = gmtime(&now);
  addr = inet_addr("127.0.0.1");
  a.s_addr=addr;
  char *uffici[3]={"Presidenza", "Segreteria docenti", "Segreteria Alunni"};
  char *eu[2]={"Entrata", "Uscita"};

  memset(&server, 0, sizeof(server));   //azzeramento struttura
  memset(&client, 0, sizeof(client));   //azzeramento struttura

  addr_init (&server, port, addr);    //inizializzo l'IP
  puts("Inizializzazione riuscita");

  rc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);      //creo il socket
  if(rc < 0){
    puts("Impossibile creare socket");
    return -1;
  }

  int b = bind(rc, (struct sockaddr*) &server, sizeof(server));   //bind del socket sulla porta specificata
  if(b < 0){
    puts("Impossibile collegarsi sulla porta specificata");
    return -1;
  }

  printf("In ascolto sulla porta: %i\n", port);
  unsigned int len = sizeof(client);
  int n=0;
  while(1){
    n = recvfrom(rc, (struct protocollo*)&proto, sizeof(proto), 0, (struct sockaddr *) &client, &len);    //ricevo le informazioni inviate dal client
    printf("Ricevuta richiesta di protocollo per: \n%s\nUfficio: %s\n(%s)\nOggetto: %s\n",proto.md, uffici[proto.ufficio], eu[proto.io], proto.oggetto);
    if(t->tm_mon == 1 && t->tm_mday == 1){     //se è 1 gennaio azzera il numero di protocollo
      FILE *file;
      file = fopen("number.txt", "r+");
      if(file == NULL){
        puts("File number.txt mancante, crealo e inserisci 0 nella prima riga!");
        return -1;
      }
      fseek(file, 0, SEEK_SET);
      fprintf(file, "%07u\n", 0);
      fclose(file);
    }
    proto.num = sequence();   //assegno il numero di protocollo
    sendto(rc, (struct protocollo*)&proto, sizeof(proto), 0, (struct sockaddr *)&client, len);    //invio il protocollo completo al client
    printf("\nInviato: %07u, %s, %s, %s, %s\n", proto.num, uffici[proto.ufficio], eu[proto.io], proto.md, proto.oggetto);
			//salvo le informazioni nel file protocolli.csv
		FILE *file;
		file = fopen("protocolli.csv", "a");
		if(file == NULL){
			puts("Impossibile creare il file");
			return 0;
		}
		fprintf(file, "%07u, %d/%d/%d, %s, %s, %s\n", proto.num, t->tm_year+1900, t->tm_mon+1, t->tm_mday, eu[proto.io], uffici[proto.ufficio], proto.oggetto);
		fclose(file);
  }	
  return 0;
}