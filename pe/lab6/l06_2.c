#include <stdio.h>
int popula_vetor(int tamanho_vetor, int *ponteiro_vetor);//funcao com declaracao de ponteiro
int imprime_vetor(int tamanho_vetor, int *ponteiro_vetor);

int main(void){
	int tamanho_vetor;
	printf("Digite o número de elementos do vetor:\n");
	scanf("%d",&tamanho_vetor);

	int vetor[tamanho_vetor];
	int *ponteiro_vetor ;
	ponteiro_vetor = &vetor[0];// notação opcional &vetor[0] apenas "vetor" retorna vetor[0]

	printf("Digite os elementos do vetor:\n");

	//popula o vetor usando aritimiteica de ponteiro
	popula_vetor(tamanho_vetor, ponteiro_vetor);
	//saida para o usuario do vetor
	imprime_vetor(tamanho_vetor, ponteiro_vetor);

return 0;
}

int popula_vetor(int tamanho_vetor, int *ponteiro_vetor){
	//popula o vetor usando aritimiteica de ponteiro
        for (int indice = 0; indice<tamanho_vetor; indice++ ){
        	scanf("%d",(ponteiro_vetor+indice)); //aritimetica ponteiro leia(endereço memoria + avança i)
        }
}

int imprime_vetor(int tamanho_vetor, int *ponteiro_vetor){
	//saida para o usuario do vetor
	for (int indice = 0; indice< tamanho_vetor; indice++){
                printf("O elemento na posição %d é %d.\n",indice,*(ponteiro_vetor + indice));
	}
}
