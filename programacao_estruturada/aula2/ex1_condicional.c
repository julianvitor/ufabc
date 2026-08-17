#include <stdio.h>
int main(void){
	int entrada;
	
	printf("Insira um numero inteiro:\n");
	scanf("%d", &entrada);

	if(entrada > 10 && entrada%2 == 0 || entrada%2 != 0 && entrada < 50){		printf("SIM\n");}
	else{
		printf("NAO\n");
	}
}
