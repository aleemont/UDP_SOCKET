#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>

struct protocollo			//Struttura che contiene le informazioni del protocollo
{     
  unsigned int num;
  unsigned int ufficio;
  unsigned int io;
  char oggetto[100];
  char md[30];
};
void addr_init(struct sockaddr_in *addr, int port, long int ip)
{
  addr->sin_family = AF_INET; //Inizializzo la famiglia di indirizzi (ipv4)
  addr->sin_port = htons((u_short) port); //conversione in little-endian del numero di porta (msb first)
  addr->sin_addr.s_addr = ip;
}

int main(int argc, char *argv[])
{
	int n_err = 0;
  int port = atoi("8082");
  int sd = 0;
  in_addr_t addr;
  struct in_addr a;
  struct sockaddr_in server;
  struct protocollo proto = {0, 0, 0, "", "I.T.T. \"G. Giorgi\" - Brindisi"};
  time_t now = time(0);
  struct tm *t = gmtime(&now);
  char *uffici[3]={"Presidenza", "Segreteria docenti", "Segreteria Alunni"};
  char *eu[2]={"Entrata", "Uscita"};
  char temp;
  memset(&addr, 0, sizeof(addr));
  addr = inet_addr("127.0.0.1");
  a.s_addr=addr;

  memset(&server, 0, sizeof(server));     //azzeramento struttura
	
  printf("IP associato a 0x%x: %s\n", addr, inet_ntoa(a)); //inet_ntoa() trasforma l'ip nella notazione puntata standard
  addr_init(&server, port, addr);   //inizializzo l'IP
  puts("Inizializzazione riuscita");

  sd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  //creo il socket
  if(sd < 0)
	{
    puts("Impossibile creare socket");
    return -1;
  }
  puts("Socket creato");

  //input delle informazioni del documento da protocollare
  puts("\nDocumento in: \n[0]Entrata\n[1]Uscita");
  scanf("%u", &proto.io);
	if(proto.io > 1 || proto.io < 0)
	{
		n_err = 0;
		while(proto.io < 0 || proto.io > 1)
		{
		if(n_err++ == 3)
		{
			puts("Troppi tentativi non validi.");
			return -1;
		}
		puts("[ERRORE] Hai inserito un valore non valido");
		puts("\nDocumento in: \n[0]Entrata\n[1]Uscita");
		scanf("%u", &proto.io);
		}
	}
  puts("\nSelezionare ufficio:\n[0]Presidenza\n[1]Segreteria Docenti\n[2]Segreteria Alunni");
  scanf("%u", &proto.ufficio);
	if(proto.ufficio < 0 || proto.ufficio >2)
	{
		n_err = 0;
		while(proto.ufficio < 0 || proto.ufficio > 2)
		{
			if(n_err++ == 3)
		{
			puts("Troppi tentativi non validi.");
			return -1;
		}
			puts("[ERRORE] Hai inserito un valore non valido");
			puts("\nSelezionare ufficio:\n[0]Presidenza\n[1]Segreteria Docenti\n[2]Segreteria Alunni");
			scanf("%u", &proto.ufficio);
		}
	}
  printf("\nInserire Oggetto: ");
  scanf("%c", &temp);
  scanf("%[^\n]", proto.oggetto);
  unsigned int len = sizeof(server);

  //invio le informazioni del documento al server

  int n = sendto(sd, (struct protocollo*)&proto, sizeof(proto), 0, (struct sockaddr *) &server, len);
  if(n < 0)
	{
    printf("Inviati: %d byte\n", n);
    puts("sendto() ha inviato un numero errato di byte!\n");
    return -1;
  }
  printf("\nInvio al server: \n\nUfficio: %s\n(%s)\nOggetto:%s\nMittente/Destinatario: %s\n", uffici[proto.ufficio], eu[proto.io], proto.oggetto, proto.md);

  //ricezione del protocollo

  recvfrom(sd, (struct protocollo*)&proto, sizeof(proto), 0, (struct sockaddr *) &server, &len);
  printf("\nRicevuto protocollo: \n\n");
	for(register int i=0; i<(strlen(proto.md)+strlen(uffici[proto.ufficio])+5); ++i)
  	printf("*");
	puts("**");
  printf("* %s - %s", proto.md, uffici[proto.ufficio]);
  printf("\n* Prot: %07u del %02d/%02d/%d", proto.num, t->tm_mday, t->tm_mon+1, t->tm_year+1900);
  printf("\n* (%s)\n", eu[proto.io]);
 	for(register int i=0; i<(strlen(proto.md)+strlen(uffici[proto.ufficio])+5); ++i)
  	printf("*");
	puts("**");

  shutdown(sd, 2);  //chiudo il socket
  return 0;
}
