#include <stdio.h>
int main(void){
	int numero1, numero2, soma, subtracao, multiplicacao,resto;
	float divisao;
//interface inicial
	printf("digite os valores inteiros(2): \n");
	scanf("%d", &numero1);
	scanf("%d", &numero2);
//operacao soma
	soma = numero1 + numero2;
	printf("soma: %d\n", soma);

//operacao multiplicacao
	multiplicacao = numero1 * numero2;
	printf("multiplicacao: %d\n", multiplicacao);
//operacao subtracao
	subtracao = numero1 - numero2;
	printf("subtracao: %d\n", subtracao);

//operacao divisao
	divisao = (float)numero1;

//operacao resto

//	resto = numero1%numero2;
}
