#include <stdlib.h>
#include <stdio.h>

int main(void){
	//Definindo variaveis
	FILE *arquivo;
	int entrada;
	int seed;
	int soma = 0;
	int numero;
	float media;
	//Input
	printf("Digite a quantidade de números:\n");
	scanf("%d", &entrada);
        printf("Digite o valor da seed:\n");
	scanf("%d", &seed);

	//Criar arquivo
	arquivo = fopen("exemplo.txt", "w");
	if (arquivo == NULL){
		printf("Erro ao abrir o arquivo\n");
		return 1;
	}
	fclose(arquivo);

	//Append valores
	arquivo = fopen ("exemplo.txt", "a");
        if (arquivo == NULL){
                printf("Erro ao abrir o arquivo\n");
		return 1;
	}
	srand(seed);
	for (int i = 0; i<entrada; i++){
		fprintf(arquivo, "%d ", rand()%11);
	}
	fclose(arquivo);

	//leitura do arquivo
	arquivo  = fopen("exemplo.txt", "r");
	while (fscanf(arquivo, "%d", &numero)==1){
		printf("%d ",numero);
		soma += numero;
		
	}
	printf("\n");
	fclose(arquivo);

	media = (float)soma/entrada;
//	printf("%d\n",soma);
	printf("Média = %.2f\n", media);
}
