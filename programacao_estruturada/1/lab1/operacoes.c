#include <stdio.h>
int main(void){
	int numero1, numero2, soma, subtracao, multiplicacao,resto;
	float divisao;

//interface inicial
	printf("Digite dois valores:\n");
	scanf("%d", &numero1);
	scanf("%d", &numero2);

//operacao soma
	soma = numero1 + numero2;
	printf("A soma é %d\n", soma);

//operacao multiplicacao
	multiplicacao = numero1 * numero2;
	printf("A diferença é %d\n", multiplicacao);

//operacao subtracao
	subtracao = numero1 - numero2;
	printf("A diferença é %d\n", subtracao);

//operacao divisao
	if (numero2 != 0) {
			divisao = (float)numero1 / numero2;
			printf("O quociente é %d\n", (int)divisao);
		} else {
			printf("infinito");
		}
		
//operacao resto
	resto = numero1%numero2;
	printf("O resto é %d\n", resto);
}
