#include<stdio.h>

unsigned int sequence(void){
	FILE *file;
	unsigned int n;
	file = fopen("number.txt", "r+");
	if(file == NULL){
		puts("File number.txt mancante, crealo e inserisci 0 nella prima riga!");
		return -1;
	}
	if(fscanf(file, "%07u", &n) != 1){
		fclose(file);
		return 0;
	}
	++n;
	fseek(file, 0, SEEK_SET);
	fprintf(file,"%07u",n);
	fclose(file);

	return n;
}
